attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec3 OS_normal;
varying vec2 TexCoords;
varying vec3 Position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	Position = vec3(model * vec4(position,1.0));
	OS_normal = normal;
	gl_Position = projection*view*model*vec4(position,1.0);
	TexCoords = texCoords;
}
