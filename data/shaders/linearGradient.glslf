// Colored 2D simple vector graphic fragment shader.
#version 120

uniform sampler1D gradientTexture;

uniform vec2 gradientStart;
uniform vec2 gradientDirection;
uniform float gradientLength;

varying vec2 fTexCoord;

void main()
{
    float gradientCoord = dot(fTexCoord - gradientStart, gradientDirection)/gradientLength;
    gl_FragData[0] = texture1D(gradientTexture, gradientCoord);
}

