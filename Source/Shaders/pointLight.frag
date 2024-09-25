#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	
	vec4 ambientColor; //Alpha channel is intensity
	vec4 lightPosition; //Ignore the w component, it's just there for padding
	vec4 lightColor; //Alpha channel is intensity
} globalUbo;

void main()
{
	float _discard = length(fragOffset);

	if (_discard > 1.0)
	{
		discard;
	}

	outColor = vec4(globalUbo.lightColor.xyz, 1.0);
}