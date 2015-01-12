// Colored 2D simple vector graphic fragment shader.
#version 120

uniform sampler2D colorTexture;

varying vec2 fTexCoord;

void main()
{
    gl_FragData[0] = texture2D(colorTexture, fTexCoord);
}

