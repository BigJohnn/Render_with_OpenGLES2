varying highp vec3 ES_normal;
varying highp vec4 ES_position;

uniform highp vec4 materialSpecularColor;
uniform highp float materialSpecularExponent;
uniform highp vec4 lightPos;

uniform highp samplerCube texture0;

void main(void)
{
    highp vec3 materialSpecularColor = materialSpecularColor.xyz;
    
    highp vec3 lightDirection; 
    
    if (lightPos.w == 0.0)
        lightDirection = normalize(vec3(lightPos));
    else
        lightDirection = normalize(vec3(lightPos - ES_position));
    
    highp vec3 normal = normalize(ES_normal);
    highp vec3 viewDirection = vec3(0.0, 0.0, 1.0);
    highp vec3 halfPlane = normalize(lightDirection + viewDirection);
    
    highp float diffuseFactor = dot(normal, lightDirection); 
    highp float specularFactor = dot(normal, halfPlane); 
    
	if(diffuseFactor<0.0) diffuseFactor*=-0.2;
	if(specularFactor<0.0) specularFactor*=-0.2;
    highp vec3 reflection = ES_position.xyz - 2.0 * dot(ES_position.xyz, normal) * normal;
    highp vec3 materialDiffuseColor = textureCube(texture0, reflection).xyz;
    
    specularFactor = pow(specularFactor, materialSpecularExponent);
    highp vec3 color = diffuseFactor * materialDiffuseColor + specularFactor * materialSpecularColor;
    
    gl_FragColor = vec4(color, 1); 
    
}