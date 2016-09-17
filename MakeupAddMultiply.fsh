precision_highp_float

uniform sampler2D u_diffuseTexture;

varying highp vec2 texCoordVarying;

uniform float colorR;
uniform float colorG;
uniform float colorB;
uniform float colorA;

void main()
{
    vec4 destColor = sampleLastFragData();
    vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    
    sourceColor.x *= colorR;
    sourceColor.y *= colorG;
    sourceColor.z *= colorB;
    
    sourceColor.a *= colorA;
    
    vec3 result = destColor.xyz + sourceColor.xyz;
    
    result.rgb *= sourceColor.a;
    
    destColor.rgb *= 1.0 - sourceColor.a;
    
    gl_FragColor.rgb = result.rgb + destColor.rgb;
    gl_FragColor.a = 1.0;
}