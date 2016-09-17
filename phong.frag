precision highp float;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
    float     shininess;
};  

struct Direction_Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

varying vec3 Position;
varying vec3 Normal;  
varying vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Direction_Light light;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - Position);
    vec3 lightDir = normalize(light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));  
    
    // Specular
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess)*0.1;
    vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
            
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);  
} 