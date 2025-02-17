#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec2 texCoords;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;
out vec3 lookat;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	
	texCoord0 = texCoords;
	color0 = color;
	normal0 = (Model * vec4(normal, 0.0)).xyz;
	position0 = vec3(Model * vec4(position, 1.0));
	gl_Position = Projection *View * Model* vec4(position, 1.0); //you must have gl_Position
	lookat = position0;//vec3(View * vec4(position, 1.0));
}
