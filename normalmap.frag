precision mediump float;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
    float     shininess;
};  

struct Light {
    vec4 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
varying highp vec3 Normal;
varying mediump vec2 TexCoords;
varying vec3 Position;

uniform Material material;
uniform Light light;
uniform mat3 normMatrix;


void main()
{
	mediump vec2 repetitions = vec2(10.0, 5.0);
	highp float theta = acos(Normal.y);
	highp float phi = atan(Normal.z,Normal.x);

	highp vec2 deltaNormal = (texture2D(material.normal, TexCoords).xy-0.5);
	theta = theta+deltaNormal.y;
	phi = phi - deltaNormal.x;

	mediump vec3 norm = vec3(cos(phi) * sin(theta), cos(theta), sin(phi)* sin(theta));
	norm = normMatrix * norm;
 
    vec3 lightDir = normalize(light.pos.xyz);
	// Ambient
    mediump  vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
    // Diffuse
    mediump float diff = max(dot(lightDir, norm), 0.0);
    mediump vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));
    // Specular
    mediump vec3 viewDir = normalize(viewPos);
   
    mediump vec3 halfwayDir = normalize(lightDir + viewDir);
    mediump float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    mediump vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    
    gl_FragColor = vec4(ambient + diffuse, 1.0);
} 