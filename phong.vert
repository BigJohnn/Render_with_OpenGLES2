attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec3 Position;
varying vec3 Normal;
varying vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	Position = vec3(model * vec4(position, 1.0));
    Normal = normMatrix * normal;
    TexCoords = texCoords;
}
