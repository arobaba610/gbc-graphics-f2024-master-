#version 460 core

uniform vec3 u_color;      // user color
uniform float u_intensity; // color intensity

out vec4 FragColor;

void main()
{
    FragColor = vec4(u_color * u_intensity, 1.0);  // Using the user color and intensity to get output 
}
