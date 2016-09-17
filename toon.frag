precision highp float;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct Light {
    vec4 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

varying vec3 ES_normal;
varying vec4 ES_position;
varying vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
  	
    // Diffuse 
    highp vec3 norm = normalize(ES_normal);
	
	highp vec3 lightDir;
	if(light.pos.w == 0.0)
		lightDir = normalize(vec3(light.pos));
	else
		lightDir = normalize(vec3(light.pos - ES_position));

    highp float diff = max(dot(norm, lightDir), 0.0);
	if(diff<0.1)
		diff=0.0;
	else if(diff<0.3)
		diff = 0.3;
	else if(diff<0.6)
		diff=0.6;
	else if(diff<0.8)
		diff=0.8;
	else diff = 1.0;

    highp vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));  
    
    // Specular
    highp vec3 viewDir = normalize(viewPos - ES_position.xyz);
	highp vec3 halfPlane = normalize(lightDir + viewDir);

    highp vec3 reflectDir = reflect(-lightDir, norm);  
    highp float spec = pow(max(dot(norm, halfPlane), 0.0), material.shininess);
	spec = step(0.5,spec);
    highp vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);  
} 