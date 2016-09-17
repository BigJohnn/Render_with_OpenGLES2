attribute vec4 position;
attribute vec3 normal;

uniform mat4 MVPmat;
uniform mat4 MVmat;
uniform mat3 normMatrix;

uniform float Time;

varying vec3 ES_normal; 
varying vec4 ES_position;
varying vec3 noiseVector;


void main(void)
{
    
    vec3 translation = vec3(1.0, 1.0, 1.0) * Time / 20.0;
    noiseVector = position.xyz + translation;
    
    ES_normal = normMatrix * normal;
    ES_position = MVmat * position;
    gl_Position = MVPmat * position;
    
}