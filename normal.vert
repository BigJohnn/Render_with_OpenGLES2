attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoords;
attribute vec3 tangent;
attribute vec3 bitangent;

varying vec3 FragPos;
varying vec2 TexCoords;
varying vec3 TangentLightPos;
varying vec3 TangentViewPos;
varying vec3 TangentFragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normMatrix;

uniform vec4 lightPos;
uniform vec3 viewPos;

//highp mat4 transpose(in highp mat4 inMatrix) {
//    highp vec4 i0 = inMatrix[0];
//    highp vec4 i1 = inMatrix[1];
//    highp vec4 i2 = inMatrix[2];
//    highp vec4 i3 = inMatrix[3];

//    highp mat4 outMatrix = mat4(
//                 vec4(i0.x, i1.x, i2.x, i3.x),
//                 vec4(i0.y, i1.y, i2.y, i3.y),
//                 vec4(i0.z, i1.z, i2.z, i3.z),
//                 vec4(i0.w, i1.w, i2.w, i3.w)
//                 );

//    return outMatrix;
//}

highp mat3 transpose(in highp mat3 inMatrix) {
    highp vec3 i0 = inMatrix[0];
    highp vec3 i1 = inMatrix[1];
    highp vec3 i2 = inMatrix[2];

    highp mat3 outMatrix = mat3(
                 vec3(i0.x, i1.x, i2.x),
                 vec3(i0.y, i1.y, i2.y),
                 vec3(i0.z, i1.z, i2.z)
                 );

    return outMatrix;
}

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0));   
    TexCoords = texCoords;

	//vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	//vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	//// re-orthogonalize T with respect to N
	//T = normalize(T - dot(T, N) * N);
	//// then retrieve perpendicular vector B with the cross product of T and N
	//vec3 B = cross(T, N);

	//mat3 TBN = mat3(T, B, N);
    vec3 T = normalize(normMatrix * tangent);
    vec3 B = normalize(normMatrix * bitangent);
    vec3 N = normalize(normMatrix * normal);    
    
    mat3 TBN = transpose(mat3(T, B, N));  
    TangentLightPos = TBN * lightPos.xyz;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
}