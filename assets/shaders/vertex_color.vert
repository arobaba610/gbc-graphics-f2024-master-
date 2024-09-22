#version 460 core

layout(location = 0) in vec3 aPosition;  // Vertex pozition
layout(location = 1) in vec3 aColor;     // Vertex color

out vec3 vertexColor;  // Fragment shader color

void main()
{
    vertexColor = aColor; 
    gl_Position = vec4(aPosition, 1.0);   
}
