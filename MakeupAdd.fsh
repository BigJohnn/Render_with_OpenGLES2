precision mediump float;

uniform sampler2D u_diffuseTexture;

uniform lowp float alpha;

varying highp vec2 texCoordVarying;

void main()
{
    lowp vec4 destColor = sampleLastFragData();
    lowp vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    lowp vec3 result = destColor.xyz + sourceColor.xyz;

    sourceColor.a *= alpha;
    result.rgb *= sourceColor.a;
    
    destColor.rgb *= 1.0 - sourceColor.a;
    
    gl_FragColor.rgb = result.rgb + destColor.rgb;
    gl_FragColor.a = 1.0;
}
