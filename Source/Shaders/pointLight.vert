#version 450

const vec2[6] offsets = vec2[6](
    vec2(-1, -1),
    vec2(-1, 1),
    vec2(1, -1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(1, 1)
);

layout(location = 0) out vec2 fragOffset;

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	
	vec4 ambientColor; //Alpha channel is intensity
	vec4 lightPosition; //Ignore the w component, it's just there for padding
	vec4 lightColor; //Alpha channel is intensity
} globalUbo;

const float LIGHT_RADIUS = 0.03;

void main()
{
	fragOffset = offsets[gl_VertexIndex];

vec3 cameraRightWorldSpace = vec3(globalUbo.viewMatrix[0][0], globalUbo.viewMatrix[1][0], globalUbo.viewMatrix[2][0]);
vec3 cameraUpWorldSpace = vec3(globalUbo.viewMatrix[0][1], globalUbo.viewMatrix[1][1], globalUbo.viewMatrix[2][1]);


	vec3 lightPositionWorldSpace = globalUbo.lightPosition.xyz 
	+ LIGHT_RADIUS * fragOffset.x * cameraRightWorldSpace
	+ LIGHT_RADIUS * fragOffset.y * cameraUpWorldSpace;

	gl_Position = globalUbo.projectionMatrix * globalUbo.viewMatrix* vec4(lightPositionWorldSpace, 1.0);
}