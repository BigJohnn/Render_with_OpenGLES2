//By hph 2016.6.18
precision highp float;
attribute vec3 position;
attribute vec3 normal;

uniform mat4 MVPmat;
uniform mat4 MVmat;
uniform mat3 normMatrix;

varying vec3 ES_normal; 
varying vec4 ES_position;


void main(void)
{
    ES_normal = normMatrix * normal;
    ES_position = MVmat * vec4(position, 1.0);
    gl_Position = MVPmat * vec4(position, 1.0);  
}