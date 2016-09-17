uniform sampler2D u_diffuseTexture;

varying highp vec2 texCoordVarying;


#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))

#define BlendSoftLightf(base, blend) 	((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))

#define Blend(base, blend, funcf) 		vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))

#define BlendSoftLight(base, blend) 	Blend(base, blend, BlendSoftLightf)
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)
#define BlendSubstractf(base, blend) 	max(base + blend - 1.0, 0.0)
#define BlendHardLight(base, blend) 	BlendOverlay(blend, base)

void main()
{
    lowp vec4 destColor = sampleLastFragData();
    lowp vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    lowp vec3 result = BlendHardLight(destColor.xyz, sourceColor.xyz);
    
    result.rgb *= sourceColor.a;
    
    destColor.rgb *= 1.0 - sourceColor.a;
    
    gl_FragColor.rgb = result.rgb + destColor.rgb;
    gl_FragColor.a = 1.0;
}