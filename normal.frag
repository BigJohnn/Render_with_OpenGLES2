precision mediump float;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
    float     shininess;
};  

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

varying vec3 FragPos;
varying vec2 TexCoords;
varying vec3 TangentLightPos;
varying vec3 TangentViewPos;
varying vec3 TangentFragPos;
  
uniform Material material;
uniform Light light;

void main()
{
// Obtain normal from normal map in range [0,1]
    vec3 norm = texture2D(material.normal, TexCoords).rgb;
    // Transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space

	// Ambient
    vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
    // Diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));
    // Specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}