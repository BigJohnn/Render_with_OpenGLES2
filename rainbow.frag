precision mediump float;

varying mediump vec4 materialDiffuseColor;
varying highp vec4 ES_position;
varying highp vec3 ES_normal;

uniform highp vec4 materialSpecularColor;
uniform highp float materialSpecularExponent;
uniform highp vec3 lightPos;
uniform highp vec3 viewPos;

void main()
{
	highp vec3 lightDirection;//全称表示非uniform变量

	lightDirection = normalize(vec3(lightPos-ES_position.xyz));

	highp vec3 normal = normalize(ES_normal);
	highp vec3 viewDir = viewPos;
	highp vec3 halfPlane = normalize(lightDirection+viewDir);
	
	highp float diffuseFactor = dot(normal, lightDirection); 
    highp float specularFactor =dot(normal, halfPlane); 
    
	if(diffuseFactor<0.0) diffuseFactor*=-0.2;
	if(specularFactor<0.0) specularFactor*=-0.2;

    specularFactor = pow(specularFactor, materialSpecularExponent);
                 
    gl_FragColor =  diffuseFactor * materialDiffuseColor + specularFactor * materialSpecularColor;
}