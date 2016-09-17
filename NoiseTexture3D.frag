#version 300 es
precision highp float;

in float LightIntensity;
in vec3 MCposition;

out vec4 FragmentColor;

uniform highp sampler3D Noise;
uniform float time;

const vec3 Color1 = vec3(1.0,0.5,0.0);
const vec3 Color2 = vec3(0.65,0.0,0.0);
const float NoiseScale = 2.0;

void main()
{
	vec3 translation = vec3(0.0,-time*0.1,0.0);
	vec3 newPosition = MCposition + translation;
	vec4 noisevec = texture(Noise, NoiseScale * newPosition);
	float intensity = abs(noisevec.x - 0.25) +
					abs(noisevec.y - 0.125) +
					abs(noisevec.z - 0.0625) +
					abs(noisevec.w - 0.03125);
	intensity = clamp(intensity*6.0,0.0,1.0);
	vec3 color = mix(Color1, Color2, intensity) * LightIntensity;
	color = clamp(color,0.0,1.0);
	FragmentColor = vec4(color,1.0);
}