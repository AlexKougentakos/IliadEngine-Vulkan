#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;

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
	mat4 modelMatrix; //projection * view * model
	mat4 normalMatrix; 
} push;

const float AMBIENT = 0.02;

void main() 
{
	vec4 position = push.modelMatrix * vec4(position, 1.0);
	gl_Position = globalUbo.projectionMatrix * globalUbo.viewMatrix * position;

	//Only works correctly if the model matrix is uniformally scaled (aka {1,1,1} or {2,2,2} but not {1,2,3})
	//vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);

	//Works correctly with non-uniform scaling, but calculating the inverse in a shader is expensive
	//mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
	//vec3 normalWorldSpace = normalize(normalMatrix * normal);

	fragNormalWorld  = normalize(mat3(push.normalMatrix) * normal);
	fragPositionWorld = position.xyz;
	outColor = color;
}