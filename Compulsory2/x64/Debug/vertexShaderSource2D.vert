#version 330 core
layout (location = 0) in vec2 vertexPosition; // Position
layout (location = 1) in vec3 vertexColor;    // Color
out vec3 fragmentColor;
void main()
{
   gl_Position = vec4(vertexPosition, 0.0, 1.0);
   fragmentColor = vertexColor; // Pass color to fragment shader
}