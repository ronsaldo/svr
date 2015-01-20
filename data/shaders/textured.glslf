// Colored 2D simple vector graphic fragment shader.
#version 120

uniform sampler2D colorTexture;

varying vec2 fTexCoord;
varying vec4 fColor;

void main()
{
    gl_FragData[0] = texture2D(colorTexture, fTexCoord)*fColor;
}

