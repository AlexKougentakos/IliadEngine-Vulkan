#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionViewMatrix;
	vec3 lightDirection;
} globalUbo;

layout(push_constant) uniform Push 
{
	mat4 modelMatrix; //projection * view * model
	mat4 normalMatrix; 
} push;

const float AMBIENT = 0.02;

void main() 
{
	gl_Position = globalUbo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

	//Only works correctly if the model matrix is uniformally scaled (aka {1,1,1} or {2,2,2} but not {1,2,3})
	//vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);

	//Works correctly with non-uniform scaling, but calculating the inverse in a shader is expensive
	//mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
	//vec3 normalWorldSpace = normalize(normalMatrix * normal);

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

	float lightIntensity = AMBIENT + max(dot(normalWorldSpace, globalUbo.lightDirection), 0);

	outColor = lightIntensity * color;
}