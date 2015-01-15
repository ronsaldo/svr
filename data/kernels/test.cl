// Test kernel
__kernel void test(__write_only image2d_t colorBuffer)
{
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    int2 size = (int2) (get_global_size(0), get_global_size(1));
    float2 uvCoord = (float2) (coord.x / (size.x - 1.0f) , coord.y / (size.y - 1.0f));

    float4 color = (float4) (0.0, uvCoord*0.5 + 0.5, 1.0);
    write_imagef(colorBuffer, coord, color);
}

