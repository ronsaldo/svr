// OpenCL volumetric raycast kernel
__constant const sampler_t ColorMapSampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

float filterValue(float value, float minValue, float maxValue )
{
	if(value < minValue)
		return 0.0;
	if(value > maxValue)
		return 0.0;
	return 1.0;
}

float4 sampleVolume(image3d_t volume, sampler_t volumeSampler, float4 point, image1d_t colorMap, float filterMinValue, float filterMaxValue)
{
	float value = read_imagef(volume, volumeSampler, point).x;
	float4 mappedValue = read_imagef(colorMap, ColorMapSampler, value);
	return ((float4) (mappedValue.xyz, mappedValue.w*value))*filterValue(value, filterMinValue, filterMaxValue);
}


// Ray-Box intersection.
// Taken from implementation located in: https://github.com/hpicgs/cgsee/wiki/Ray-Box-Intersection-on-the-GPU
float3 rayBoxIntersection(float3 rayOrigin, float3 rayDirection, float3 rayInverseDirection, float3 boxMin, float3 boxMax)
{
	float3 boxBounds[2] = {
		 boxMin, 
		 boxMax,
	};
	
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	int raySignX = (rayInverseDirection.x) < 0 ? 1 : 0;
	int raySignY = (rayInverseDirection.y) < 0 ? 1 : 0;
	int raySignZ = (rayInverseDirection.z) < 0 ? 1 : 0;
		
	tmin = (boxBounds[raySignX].x - rayOrigin.x) * rayInverseDirection.x;
	tmax = (boxBounds[1-raySignX].x - rayOrigin.x) * rayInverseDirection.x;
	tymin = (boxBounds[raySignY].y - rayOrigin.y) * rayInverseDirection.y;
	tymax = (boxBounds[1-raySignY].y - rayOrigin.y) * rayInverseDirection.y;
	tzmin = (boxBounds[raySignZ].z - rayOrigin.z) * rayInverseDirection.z;
	tzmax = (boxBounds[1-raySignZ].z - rayOrigin.z) * rayInverseDirection.z;

	tmin = max(max(tmin, tymin), tzmin);
	tmax = min(min(tmax, tymax), tzmax);
	return (float3) (tmin, tmax, (tmin < tmax) ? 1.0 : 0.0);
}

// Utility to convert from world space into the texture space cube coordinates.
float4 convertToCubeCoordinates(float3 point, float4 boxMin, float4 boxMax)
{
	float4 inputExtent = boxMax - boxMin;
	float4 point4 = (float4) (point, 0.0);
	float4 result =  (point4 -  boxMin) / inputExtent;
	result.w = 0.0;
	return result;
}

float4 integrate(__read_only image3d_t volume, float segmentLength, float4 startPoint, float4 endPoint, int minNumberOfSamples, int maxNumberOfSamples, float lengthSamplingFactor, float boxLength, float lengthScale, float alphaScale, float4 cubeViewRegionMin, float4 cubeViewRegionMax, sampler_t cubeSampler, 
image1d_t colorMap, float filterMinValue, float filterMaxValue)
{
	// Compute the number of samples and the step size to use.
	float integrationLength = segmentLength;
	int numberOfSteps = clamp((int)ceil(lengthSamplingFactor*integrationLength * (maxNumberOfSamples - 1) / boxLength),  minNumberOfSamples, maxNumberOfSamples); 
	float scaleFactor = integrationLength;
	float stepSize = 1.0 / (numberOfSteps - 1);
	
	// Endpoints for the trapezoidal rule
	float4 result = sampleVolume(volume, cubeSampler, startPoint, colorMap, filterMinValue, filterMaxValue);

	// Sample the inner points
	for(int i = 1; i < numberOfSteps; ++i) {
		float4 point = mix(startPoint, endPoint, i*stepSize);
		float factor = (i & 1) ? 4.0f : 2.0f; 
		result += factor*sampleVolume(volume, cubeSampler, point, colorMap, filterMinValue, filterMaxValue);
	}
	result += sampleVolume(volume, cubeSampler, endPoint, colorMap, filterMinValue, filterMaxValue);
	result *= stepSize * scaleFactor / 3.0;
	//result *= stepSize  / 3.0;
	result.w = 1.0;
	return result;
}

float4 integrateSolid(__read_only image3d_t volume, float segmentLength, float4 startPoint, float4 endPoint, int minNumberOfSamples, int maxNumberOfSamples, float lengthSamplingFactor, float boxLength, float lengthScale, float alphaScale, float4 cubeViewRegionMin, float4 cubeViewRegionMax, sampler_t cubeSampler, 
image1d_t colorMap, float filterMinValue, float filterMaxValue)
{
	// Compute the number of samples and the step size to use.
	float integrationLength = segmentLength;
	int numberOfSteps = clamp((int)ceil(lengthSamplingFactor*integrationLength * (maxNumberOfSamples - 1) / boxLength),  minNumberOfSamples, maxNumberOfSamples); 
	float scaleFactor = integrationLength;
	float stepSize = 1.0 / (numberOfSteps - 1);
	
	// Endpoints for the trapezoidal rule
	float4 result = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
	
	// Sample the inner points
	for(int i = 0; i < numberOfSteps; ++i) {
		float4 point = mix(startPoint, endPoint, i*stepSize);
		float4 sample = sampleVolume(volume, cubeSampler, point, colorMap, filterMinValue, filterMaxValue);
		float sampleAlpha = sample.w*alphaScale;
		result = (float4) (result.xyz*result.w + sample.xyz*sampleAlpha*(1.0-result.w), result.w + sampleAlpha*(1.0f - result.w));
	}
	//result *= stepSize * scaleFactor / 3.0;
	//result *= stepSize  / 3.0;
	result.w = 1.0;
	return result;
}

// Cube slice extraction
__kernel void raycastVolume(__read_only image3d_t volume, __write_only image2d_t renderBuffer,
	float4 boxMin, float4 boxMax, float lengthScale,
	int minNumberOfSamples,
	int maxNumberOfSamples,
	float lengthSamplingFactor,
	float4 center, float4 nearTopLeft, float4 nearTopRight, float4 nearBottomLeft, float4 nearBottomRight,
	float alphaScale,
	float nearDistance, float farDistance,
	float invGammaCorrectionFactor,
	float4 cubeViewRegionMin, float4 cubeViewRegionMax,
	sampler_t cubeSampler,
	image1d_t colorMap, float filterMinValue, float filterMaxValue,
	float4 viewForwardVector)
{
	// Compute data from the cube.
	float boxLength = length(boxMax - boxMin);
	float4 boxExtent = (boxMax - boxMin);
		
	// Compute the viewed cube
	float4 viewMin = cubeViewRegionMin*boxExtent + boxMin;
	float4 viewMax = cubeViewRegionMax*boxExtent + boxMin;
	
	// Compute basic thread information.
	int2 extent = (int2) (get_global_size(0), get_global_size(1));
	int2 coord = (int2) (get_global_id(0), get_global_id(1));
	float2 uvCoord = (float2) ((coord.x) / (extent.x - 1.0f), coord.y / (extent.y - 1.0f));

	// Compute the ray target point.
	float4 targetPoint = mix(mix(nearTopLeft, nearTopRight, uvCoord.x), mix(nearBottomLeft, nearBottomRight, uvCoord.x), uvCoord.y);
	float3 rayOrigin = (targetPoint.xyz - viewForwardVector.xyz ) * (1.0 - center.w) +  center.xyz*center.w;
	float3 rayDirection = normalize(targetPoint.xyz - rayOrigin);
	float3 rayInverseDirection = 1.0 / rayDirection;

	// Compute the ray intersection points.	
	float3 intersection = rayBoxIntersection(rayOrigin, rayDirection, rayInverseDirection, viewMin.xyz, viewMax.xyz);
	float4 color = (float4) (0.0, 0.0, 0.0, 1.0);
	if(intersection.z == 1.0 && intersection.y >= 0.0)
	{
		float3 startPoint = rayOrigin + rayDirection*max(intersection.x, 0.0f);
		float3 endPoint = rayOrigin + rayDirection*intersection.y;
		
		float4 startPointCube = convertToCubeCoordinates(startPoint, boxMin, boxMax);
		float4 endPointCube = convertToCubeCoordinates(endPoint, boxMin, boxMax);
		color = integrate(volume, length(endPoint - startPoint)/lengthScale, startPointCube, endPointCube, minNumberOfSamples, maxNumberOfSamples, lengthSamplingFactor, boxLength, lengthScale, alphaScale, cubeViewRegionMin, cubeViewRegionMax, cubeSampler, colorMap, filterMinValue, filterMaxValue);
	}
	
	write_imagef(renderBuffer, coord,  pow(color, invGammaCorrectionFactor));
}

