// Colored 2D simple vector graphic vertex shader.
#version 120

uniform mat4 projectionMatrix;

attribute vec2 vPosition;
attribute vec4 vColor;

varying vec4 fColor;

void main()
{
    // Pass through the vertex color.
    fColor = vColor;

    // Transform the position with the projection matrix
    gl_Position = projectionMatrix * vec4(vPosition, 0.0, 1.0);
}

