#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform vec4 lightDirection;



void main()
{
	gl_FragColor = vec4(0.f,0.f,0.f,1.f);
}