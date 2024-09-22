#version 460 core

layout(location = 0) in vec3 aPosition;  // Vertex pozition

uniform mat4 u_world;  // World transform matrix

void main()
{
    gl_Position = u_world * vec4(aPosition, 1.0);  // Calculation of the position
}
