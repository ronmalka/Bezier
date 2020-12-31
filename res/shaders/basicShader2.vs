#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 MVP;
uniform mat4 Normal;

void main()
{
    TexCoords = aPos;
    gl_Position = MVP * Normal * vec4(aPos, 1.0);
} 