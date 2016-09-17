#version 300 es
precision mediump float;

struct Light
{
	vec3 color;
};

uniform Light light;

out vec4 color;

void main()
{
	vec3 Color = light.color;
	//color = vec4(Color, 1.0f);
	color = vec4(1.0f);
}