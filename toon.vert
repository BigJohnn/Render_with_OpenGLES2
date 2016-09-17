attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec3 ES_normal;
varying vec4 ES_position;
varying vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normMatrix;

void main(void)
{
	ES_normal = normMatrix * normal;
	ES_position = view*model*vec4(position,1.0);
	gl_Position = projection*ES_position;
	TexCoords = texCoords;
}
