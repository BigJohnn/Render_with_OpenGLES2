precision lowp float;
uniform lowp sampler2D shadowMap;

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec4 FragPosLightSpace;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct Direction_Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Direction_Light light;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture2D(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is varying shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
	//ivec2 sz= textureSize(shadowMap, 0);
	vec2 texelSize = vec2(1.0/float(1024), 1.0/float(1024));
    //lowp vec2 texelSize = vec2(1.0, 1.0) / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    // Ambient
    vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
  	
    // Diffuse 
    vec3 norm = normalize(-Normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));  
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    // Calculate shadow
    float shadow = shadows ? ShadowCalculation(FragPosLightSpace) : 0.0;                 
    shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light varying shadowed regions
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(texture2D(material.diffuse, TexCoords));    
    
    gl_FragColor = vec4(lighting, 1.0);
}
