// Colored 2D simple vector graphic fragment shader.
#version 120

varying vec4 fColor;

void main()
{
    gl_FragData[0] = fColor;
}

