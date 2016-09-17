varying highp vec2 texCoordVarying;


#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define BlendDarkenf(base, blend) 		min(blend, base)

#define BlendSoftLightf(base, blend) 	((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))
#define BlendColorDodgef(base, blend) 	((blend == 1.0) ? blend : min(base / (1.0 - blend), 1.0))
#define BlendColorBurnf(base, blend) 	((blend == 0.0) ? blend : max((1.0 - ((1.0 - base) / blend)), 0.0))

#define BlendVividLightf(base, blend) 	((blend < 0.5) ? BlendColorBurnf(base, (2.0 * blend)) : BlendColorDodgef(base, (2.0 * (blend - 0.5))))

#define Blend(base, blend, funcf) 		vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))

#define BlendSoftLight(base, blend) 	Blend(base, blend, BlendSoftLightf)
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)
#define BlendHardLight(base, blend) 	BlendOverlay(blend, base)
#define BlendVividLight(base, blend) 	Blend(base, blend, BlendVividLightf)
#define BlendDarken				BlendDarkenf


uniform lowp float time;

void main()
{
    lowp vec4 destColor = sampleLastFragData();
    
    highp float noise = fract(sin(dot(texCoordVarying.xy + vec2(time, time),vec2(12.9898,78.233))) * 43758.5453) * 0.22;
    
    gl_FragColor.rgb = destColor.rgb + noise;
    gl_FragColor.a = 1.0;
}