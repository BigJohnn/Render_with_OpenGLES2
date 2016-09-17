attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec3 Position;
varying vec3 Normal;
varying vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float Time;
uniform mat3 normMatrix;

//Õñ·ù
#define AMPLITUDE 1.2

void main()
{
	Position = vec3(view * model * vec4(position, 1.0));
    Normal = normMatrix * normal;
    TexCoords = texCoords;
	vec4 p = vec4(position, 1.0);
	p.y += sin(p.x+Time)*AMPLITUDE-0.2;
	p.z += sin(p.x-Time)*AMPLITUDE;
	gl_Position = projection * view * model * p;
}
