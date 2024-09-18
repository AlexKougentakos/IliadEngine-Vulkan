#version 450

layout (location = 0) in vec2 position;
void main()
{
	//The centre is at 0,0
	gl_Position = vec4(position,0.0, 1.0);
}