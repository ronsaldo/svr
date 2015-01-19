// Colored 2D simple vector graphic fragment shader.
#version 120

uniform sampler2D colorTexture;
uniform float inverseGammaFactor;

varying vec2 fTexCoord;

void main()
{
    vec4 color = texture2D(colorTexture, fTexCoord);
    gl_FragData[0] = vec4(pow(color.r, inverseGammaFactor), pow(color.g, inverseGammaFactor), pow(color.b, inverseGammaFactor), color.a);
}

