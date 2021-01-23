#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler;
uniform vec4 lightDirection;

vec3 calcColor()
{
  	vec4 ambient = vec4(0.8,0.8,0.8,1.0);
	vec3 Ks = vec3(0.7,0.7,0.7);
	vec3 Ka = color0;
	vec3 Kd = Ka;
	float shin = 20.0;
	vec3 lightPosition = vec3(2.0,4.0,10.0);
	vec4 lightIntens = vec4(1., 1., 1. ,1.0);
	vec3 eye = normalize(-position0); 
	vec3 color = vec3(0,0,0);
	vec3 N = normalize(normal0);
	vec3 L = normalize(lightPosition.xyz - position0); 

	vec3 IL = lightIntens.xyz * dot(N,L);
	vec3 V = normalize(eye - position0);
	vec3 R = reflect(-L,N);

	float dotDiffuse =  dot(N,L);
	float dotSpec = dot(V,R);

	if(dotSpec > 0)
	{
		color += Ks*(pow(dotSpec,shin))*IL;
	}
	if(dotDiffuse > 0)
	{
		color += Kd*(dotDiffuse)*IL;
	}
	
	color += Ka*ambient.xyz;

	//return normalize(color);
	return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));	
	
}


void main()
{
//gl_FragColor = vec4(1,0,0,1);
	gl_FragColor = vec4((texture(sampler, texCoord0) * vec4(calcColor(),0.5)).xyz,1.0); //you must have gl_FragColor
}