// OpenCL volumetric raycast kernel
__constant const sampler_t ColorMapSampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

enum SamplingMode
{
    SM_WeightedAdditive = 0,
    SM_Average
};

float filterValue(float value, float minValue, float maxValue )
{
	if(value < minValue)
		return 0.0;
	if(value > maxValue)
		return 0.0;
	return 1.0;
}

float4 sampleVolume(image3d_t volume, sampler_t volumeSampler, float4 point, image1d_t colorMap, float invColorMapSize, float filterMinValue, float filterMaxValue)
{

	float value = read_imagef(volume, volumeSampler, point).x;
	float4 mappedValue = read_imagef(colorMap, ColorMapSampler, value*(1.0f - invColorMapSize) + invColorMapSize*0.5f);
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

float4 integrate(__read_only image3d_t volume, float segmentLength, float4 startPoint, float4 endPoint, int minNumberOfSamples, int maxNumberOfSamples, float lengthSamplingFactor, float boxLength, float lengthScale, float4 cubeViewRegionMin, float4 cubeViewRegionMax, sampler_t cubeSampler, 
image1d_t colorMap, float invColorMapSize, float filterMinValue, float filterMaxValue,
    
    int averageSamples,
    float4 sampleColorIntensity
)
{
	// Compute the number of samples and the step size to use.
	float integrationLength = segmentLength;
	int numberOfSteps = clamp((int)ceil(lengthSamplingFactor*integrationLength * (maxNumberOfSamples - 1) / boxLength),  minNumberOfSamples, maxNumberOfSamples); 
	float scaleFactor = integrationLength;
	float stepSize = 1.0 / (numberOfSteps - 1);
	

	// Endpoints for the Simpson's rule
	float4 result = sampleColorIntensity*sampleVolume(volume, cubeSampler, startPoint, colorMap, invColorMapSize, filterMinValue, filterMaxValue);

	// Sample the inner points
	for(int i = 1; i < numberOfSteps-1; ++i) {
		float4 point = mix(startPoint, endPoint, i*stepSize);
		float factor = (i & 1) ? 4.0f : 2.0f; 
		result += factor*sampleColorIntensity*sampleVolume(volume, cubeSampler, point, colorMap, invColorMapSize, filterMinValue, filterMaxValue);
	}
	result += sampleColorIntensity*sampleVolume(volume, cubeSampler, endPoint, colorMap, invColorMapSize, filterMinValue, filterMaxValue);

    if(averageSamples)
    	result *= stepSize  / 3.0f;
    else
	    result *= stepSize * scaleFactor / 3.0f;


	// Sample the inner points
    /*float4 result = (float4) (0,0,0,0);
	for(int i = 0; i < numberOfSteps; ++i) {
		float4 point = mix(startPoint, endPoint, i*stepSize);
		float4 sample = sampleVolume(volume, cubeSampler, point, colorMap, invColorMapSize, filterMinValue, filterMaxValue);
        sample.w *= 0.01;
        result = (float4) (result.xyz + sample.xyz*sample.w*(1.0 - result.w), result.w + (1.0 - result.w)*sample.w);
	}*/

	result.w = 1.0f;
	return result;
}

// Cube volume rendering
__kernel void raycastVolume(__read_only image3d_t volume, __write_only image2d_t renderBuffer,
    // Camera information
    float4 nearTopLeft, float4 nearTopRight, float4 nearBottomLeft, float4 nearBottomRight,
    float4 farTopLeft, float4 farTopRight, float4 farBottomLeft, float4 farBottomRight,

    // Cube parameters
	float4 boxMin, float4 boxMax, 
	float4 cubeViewRegionMin, float4 cubeViewRegionMax,
    float lengthScale,

    // Sampling
	int minNumberOfSamples,
	int maxNumberOfSamples,
	float lengthSamplingFactor,
	sampler_t cubeSampler,

    // Color mapping
	image1d_t colorMap, float invColorMapSize, float filterMinValue, float filterMaxValue,

    // Color correction
	float invGammaCorrectionFactor,

    // Extra modes
    int averageSamples,
    float4 sampleColorIntensity
)
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

	// Compute the point location in the near and the far plane
    float4 nearPoint = mix(mix(nearBottomLeft, nearBottomRight, uvCoord.x), mix(nearTopLeft, nearTopRight, uvCoord.x), uvCoord.y);
    float4 farPoint = mix(mix(farBottomLeft, farBottomRight, uvCoord.x), mix(farTopLeft, farTopRight, uvCoord.x), uvCoord.y);

    // Compute the ray.
    float3 rayOrigin = nearPoint.xyz;
    float3 rayTarget = farPoint.xyz;
	float3 rayDirection = normalize(rayTarget - rayOrigin);
	float3 rayInverseDirection = 1.0f / rayDirection;
    float rayMaxParameter = dot(rayTarget - rayOrigin, rayDirection);

	// Compute the ray intersection points.	
	float3 intersection = rayBoxIntersection(rayOrigin, rayDirection, rayInverseDirection, viewMin.xyz, viewMax.xyz);
	float4 color = (float4) (0.0, 0.0, 0.0, 1.0);
	if(intersection.z == 1.0 && intersection.y >= 0.0)
	{
        // Compute the start and end points in world space.
		float3 startPoint = rayOrigin + rayDirection*max(intersection.x, 0.0f);
		float3 endPoint = rayOrigin + rayDirection*min(intersection.y, rayMaxParameter);
		
		float4 startPointCube = convertToCubeCoordinates(startPoint, boxMin, boxMax);
		float4 endPointCube = convertToCubeCoordinates(endPoint, boxMin, boxMax);
		color = integrate(volume, length(endPoint - startPoint)/lengthScale, startPointCube, endPointCube, minNumberOfSamples, maxNumberOfSamples, lengthSamplingFactor, boxLength, lengthScale, cubeViewRegionMin, cubeViewRegionMax, cubeSampler, colorMap, invColorMapSize, filterMinValue, filterMaxValue,
        averageSamples, sampleColorIntensity);
	}

	write_imagef(renderBuffer, coord,  pow(color, invGammaCorrectionFactor));
}

