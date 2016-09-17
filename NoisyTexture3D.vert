#version 300 es

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float LightIntensity;
out vec3 MCposition;

uniform highp vec4 lightPosition;
uniform float Scale;

void main()
{
	vec3 LightPosition = vec3(lightPosition);
	vec4 ECposition = view * model * position;
	MCposition = vec3(position)*1.0;
	vec3 tnorm = mat3(transpose(inverse(model))) * normal;
	LightIntensity = dot(normalize(LightPosition - vec3(ECposition)),tnorm)*1.5;
	gl_Position = projection * view * model * vec4(position, 1.0f);
}