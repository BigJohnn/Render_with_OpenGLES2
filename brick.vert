attribute vec3 position;
attribute vec3 normal;

varying vec3 OS_position;
varying float LightIntensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightPos;
uniform mat3 normMatrix;

const float SpecularContribution = 0.3;
const float DiffuseContribution  = 1.0 - SpecularContribution;

void main(void)
{
	float w = lightPos.w;
    vec3 lightPosition = lightPos.xyz;

	vec4 ES_position = view * model * vec4(position,1.0);
	vec3 tnorm      = normMatrix * normal;
    vec3 lightVec;
    
    if (w == 0.0)
        lightVec = normalize(lightPosition);
    else
       lightVec = normalize(lightPosition - ES_position.xyz); 

	vec3 reflectVec = reflect(-lightVec, tnorm);	
	vec3 viewVec    = normalize(-ES_position.xyz);
	float diffuse   = max(dot(lightVec, tnorm), 0.0);
	float spec      = 0.0;

	if (diffuse > 0.0) {
		spec = dot(reflectVec, viewVec);
		spec = pow(spec, 16.0);
	}
	
	LightIntensity = DiffuseContribution * diffuse;
	 + SpecularContribution * spec;// bug????  error display
    
	OS_position = position;
	
	gl_Position = projection*ES_position;
}
