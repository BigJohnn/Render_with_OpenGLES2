precision highp float;

varying vec4 ES_position;
varying vec3 ES_normal;  
varying vec3 OS_position;

uniform highp vec4 materialSpecularColor;
uniform highp float materialSpecularExponent;
uniform sampler2D texture0;
uniform vec4 lightPos;
uniform highp float Time;

const highp float M_PI = 3.14159;
const highp float M_2PI = 6.28318;

void main()
{
	highp float theta = asin(OS_position.y / length(OS_position.xyz));
    highp float phi = atan(OS_position.z , OS_position.x);
    mediump vec2 textureCoordinate = vec2(theta / M_PI, phi / M_2PI);
    
    highp vec2 position = (textureCoordinate - 0.5) * 2.0;
    highp float length = length(position);
    highp vec2 direction = position / length;
        
    highp vec2 st = textureCoordinate.xy + direction * cos(length * 20.0 - Time * 10.0) * 0.005;
    highp vec3 materialDiffuseColor = texture2D(texture0, st).xyz;
    
    highp vec3 materialSpecularColor = materialSpecularColor.xyz;
    
    highp vec3 lightDirection;; 
    
    lightDirection = normalize(vec3(lightPos));
    highp vec3 normal = normalize(ES_normal);
    highp vec3 viewDirection = vec3(0.0, 0.0, 1.0);
    highp vec3 halfPlane = normalize(lightDirection + viewDirection);
    
    highp float diffuseFactor = max(0.1, 1.5 * dot(normal, lightDirection)); 
    highp float specularFactor = max(0.0, dot(normal, halfPlane)); 
    
    specularFactor = pow(specularFactor, materialSpecularExponent);
        
    highp vec3 Color = diffuseFactor * materialDiffuseColor - specularFactor * materialSpecularColor; //正常思路是 + specularFactor * materialSpecularColor，这里显示特殊效果;
    
    gl_FragColor = vec4(Color, 1.0);  
} 