attribute vec3 position;
attribute vec3 normal;

varying vec3 OS_position;
varying vec3 ES_normal;
varying vec4 ES_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normMatrix;

void main()
{
	OS_position = position;
    
	ES_position = view * model * vec4(position, 1.0);
    ES_normal = normMatrix * normal;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
