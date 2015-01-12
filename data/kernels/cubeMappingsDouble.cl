// OpenCL double precision cube mapping kernels
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__constant const sampler_t RawSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
__constant const sampler_t ColorMapSampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;


// Double sample union.
// Encode two floats as int32.
typedef union
{
	uint2 uint2Value;
	double doubleValue;
}DoubleSample;

// Utility functions
float4 intensityToColor(float mappedValue)
{
	return (float4) (mappedValue, mappedValue, mappedValue, mappedValue);
}

__kernel void cubeLinearRangeMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double scale, double offset)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double mappedValue =  rawValue* scale + offset;
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeLinearMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double mappedValue = rawValue*invMaxValue;
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeLogMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double exponent, double numberOfColors, double norm, double invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double mappedValue;
	if(exponent >= 0)
		mappedValue = norm*log(exponent*numberOfColors*rawValue*invMaxValue + 1);
	else
		mappedValue = norm*log(1 - exponent*numberOfColors*rawValue*invMaxValue);
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeSqrtMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double invMaxValue, double norm)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double mappedValue = sqrt(rawValue*invMaxValue)*norm;
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeSquareMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double invMaxValue, double norm)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double v = rawValue*invMaxValue;
	double mappedValue = v*v*norm;
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeSinhMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double v = 2.0*3.0*rawValue*invMaxValue;
	double mappedValue = (exp(v) - 1) / (exp(v)) / 10.0;
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

__kernel void cubeASinhMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, double exponent, double numberOfColors, double norm, double par, double invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	
	// Read indirectly the double value.
	DoubleSample sample;
	sample.uint2Value  = read_imageui(rawCube, RawSampler, coord).xy;
	double rawValue = sample.doubleValue;
	
	// Perform the mapping.
	double v = rawValue*invMaxValue;
	double var2 = v + sqrt(1.0 + v*v);
	
	double mappedValue;
	if(exponent >= 0)
		mappedValue = log(exponent * numberOfColors * var2 * par + 1.0) / 3.0;
	else
		mappedValue = log(1.0 - exponent * numberOfColors * var2 * par) / 3.0;
	
	write_imagef(mappedCube,  coord, (float)mappedValue);
}

