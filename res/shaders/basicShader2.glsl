#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;
in vec3 lookat; // direction vector representing a 3D texture coordinate

uniform vec4 lightColor;
uniform samplerCube skybox;
uniform vec4 lightDirection;

void main()
{             
    gl_FragColor = texture(skybox, lookat);
}  

