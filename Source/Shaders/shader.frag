#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPositionWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	
	vec4 ambientColor; //Alpha channel is intensity
	vec4 lightPosition; //Ignore the w component, it's just there for padding
	vec4 lightColor; //Alpha channel is intensity
} globalUbo;


layout(push_constant) uniform Push 
{
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main() 
{
	vec3 lightDirection = globalUbo.lightPosition.xyz - fragPositionWorld;

	//Do this BEFORE normalizing the light direction
	float attenuation = 1.0 / dot(lightDirection, lightDirection); // Getting the dot product of a vector with itself is a quick/efficient way to get the length squared


	vec3 lightColor = globalUbo.lightColor.xyz * globalUbo.lightColor.w * attenuation;
	vec3 ambientLight = globalUbo.ambientColor.xyz * globalUbo.ambientColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(lightDirection)), 0);

	outColor = vec4((diffuseLight + ambientLight) * lightColor, 1.0);
}