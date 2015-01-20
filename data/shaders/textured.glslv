// Textured 2D simple vector graphic vertex shader.
#version 120

uniform mat4 projectionMatrix;

attribute vec2 vPosition;
attribute vec2 vTexCoord;
attribute vec4 vColor;

varying vec2 fTexCoord;
varying vec4 fColor;

void main()
{
    // Pass through the texture coordinate and the color.
    fTexCoord = vTexCoord;
    fColor = vColor;
    
    // Transform the position with the projection matrix
    gl_Position = projectionMatrix * vec4(vPosition, 0.0, 1.0);
}

