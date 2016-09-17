attribute vec3 position;
attribute vec3 normal;

varying vec3 MS_position;
varying float LightIntensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightPos;
uniform float materialSpecularExponent;
uniform mat3 normMatrix;
const float SpecularContribution = 0.3;
const float diffusecontribution  = 1.0 - SpecularContribution;

void main()
{
    // compute the vertex position in eye coordinates
    vec3  ecPosition           = vec3(view*model * vec4(position,1.0));
    
    // compute the transformed normal
    vec3  tnorm                = normalize(normMatrix * normal);
    
    // compute a vector from the model to the light position
    vec3  lightVec             = normalize(lightPos.xyz - ecPosition);
    
    // compute the reflection vector
    vec3  reflectVec           = reflect(-lightVec, tnorm);
    
    // compute a unit vector in direction of viewing position
    vec3  viewVec              = normalize(-ecPosition);
    
    // calculate amount of diffuse light based on normal and light angle
    float diffuse              = max(dot(lightVec, tnorm), 0.0);
    float spec                 = 0.0;
    
    // if there is diffuse lighting, calculate specular
    if(diffuse > 0.0)
       {
          spec = max(dot(reflectVec, viewVec), 0.0);
          spec = pow(spec, materialSpecularExponent);
       }
    
    // add up the light sources, since this is a varying (global) it will pass to frag shader     
    LightIntensity  = diffusecontribution * diffuse * 1.5 +
                          SpecularContribution * spec;
    
    // the varying variable MCPosition will be used by the fragment shader to determine where
    //    in model space the current pixel is                      
    MS_position      = position;
    
    // send vertex information
    gl_Position = projection * view * model * vec4(position, 1.0);
}
