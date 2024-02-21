#version 330 core
in vec3 fragmentColor; // Color from vertex shader
out vec4 color;        // Final color output
void main()
{
   color = vec4(fragmentColor, 1.0);
}