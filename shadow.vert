attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat3 normMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = normMatrix * normal;
    TexCoords = texCoords;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}