precision mediump float;

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

varying highp vec3 OS_normal;
varying mediump vec2 TexCoords;
varying vec3 Position;
uniform mat3 normMatrix;
uniform mediump vec4 materialSpecularColor;
uniform mediump float materialSpecularExponent;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    mediump vec2 repetitions = vec2(10.0, 5.0);

    mediump vec3 diffuse = texture2D(texture0, TexCoords*repetitions).xyz;
    mediump vec3 specular = materialSpecularColor.xyz;
        
    highp float theta = acos(OS_normal.y); 
    highp float phi = atan(OS_normal.z, OS_normal.x);

    highp vec2 deltaNormal = (texture2D(texture1, TexCoords * repetitions).xy - 0.5) * 20.0;
    theta = theta + deltaNormal.y;
    phi = phi - deltaNormal.x;
  
    mediump vec3 normal = vec3(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));
	//mediump vec3 normal = OS_normal;
    normal = normMatrix * normal;

    mediump vec3 lightDirection = normalize(vec3(light.pos));    
	mediump vec3 viewDir = normalize(viewPos-Position);
    mediump vec3 halfPlane = normalize(lightDirection + viewDir);
    
    mediump float diffuseFactor = max(0.0, dot(normal, lightDirection)); 
    mediump float specularFactor = max(0.0, dot(normal, halfPlane)); 
    
    specularFactor = pow(specularFactor, materialSpecularExponent);
    mediump vec3 color = diffuseFactor * diffuse + specularFactor * specular;
   
    gl_FragColor = vec4(color, 1.0);  
} 