// OpenCL single precision cube mapping kernels
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__constant const sampler_t RawSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

__kernel void cubeLinearRangeMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float scale, float offset)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float mappedValue = rawValue * scale + offset;
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeLinearMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float  invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float mappedValue = rawValue*invMaxValue;
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeLogMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float exponent, float numberOfColors, float norm, float invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float mappedValue;
	if(exponent >= 0)
		mappedValue = norm*log(exponent*numberOfColors*rawValue*invMaxValue + 1);
	else
		mappedValue = norm*log(1 - exponent*numberOfColors*rawValue*invMaxValue);
		
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeSqrtMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float  invMaxValue, float norm)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float mappedValue = sqrt(rawValue*invMaxValue)*norm;
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeSquareMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float  invMaxValue, float norm)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float v = rawValue*invMaxValue;
	float mappedValue = v*v*norm;
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeSinhMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float  invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	// Perform the mapping.
	float v = 2.0*3.0*rawValue*invMaxValue;
	float mappedValue = (exp(v) - 1) / (exp(v)) / 10.0;
	
	write_imagef(mappedCube,  coord, mappedValue);
}

__kernel void cubeASinhMapping(__read_only image3d_t rawCube, __write_only image3d_t mappedCube, float exponent, float numberOfColors, float norm, float par, float invMaxValue)
{
	int4 coord = (int4) (get_global_id(0), get_global_id(1), get_global_id(2), 0);
	float rawValue = read_imagef(rawCube, RawSampler, coord).x;
	
	float v = rawValue*invMaxValue;
	float var2 = v + sqrt(1.0 + v*v);
	
	float mappedValue;
	if(exponent >= 0)
		mappedValue = log(exponent * numberOfColors * var2 * par + 1.0) / 3.0;
	else
		mappedValue = log(1.0 - exponent * numberOfColors * var2 * par) / 3.0;
		
	write_imagef(mappedCube,  coord, mappedValue);
}
