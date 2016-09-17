precision_mediump_float

uniform sampler2D u_diffuseTexture;

varying highp vec2 texCoordVarying;

uniform lowp float alpha;

void main()
{
    lowp vec4 destColor = sampleLastFragData();
    lowp vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    sourceColor.a *= alpha;
    
    gl_FragColor.rgb = mix(destColor.rgb, sourceColor.rgb, sourceColor.a);
    gl_FragColor.a = 1.0;
}