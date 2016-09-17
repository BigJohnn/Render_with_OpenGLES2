precision_highp_float

uniform sampler2D u_diffuseTexture;

varying highp vec2 texCoordVarying;


#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))

#define BlendSoftLightf(base, blend) 	((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))

#define Blend(base, blend, funcf) 		vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))

#define BlendSoftLight(base, blend) 	Blend(base, blend, BlendSoftLightf)
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)
#define BlendSubstractf(base, blend) 	max(base + blend - 1.0, 0.0)
#define BlendHardLight(base, blend) 	BlendOverlay(blend, base)
#define BlendScreenf(base, blend) 		(1.0 - ((1.0 - base) * (1.0 - blend)))
#define BlendScreen(base, blend) 		Blend(base, blend, BlendScreenf)
#define BlendColorBurnf(base, blend) 	((blend == 0.0) ? blend : max((1.0 - ((1.0 - base) / blend)), 0.0))
#define BlendColorBurn(base, blend) 	Blend(base, blend, BlendColorBurnf)
#define BlendMultiply(base, blend) 		(base * blend)

vec3 czm_saturation(vec3 rgb, float adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

void main()
{
    vec4 destColor = sampleLastFragData();
    vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    
    vec4 destColorSaturate = vec4(czm_saturation(destColor.xyz, 0.0), 1.0);

//    destColorSaturate.a *= 0.4;
//    sourceColor.a *= 0.4;
    
    vec3 result = BlendMultiply(destColorSaturate.xyz, sourceColor.xyz);
    
    vec4 resultHardLight = vec4(BlendHardLight(destColor.xyz, result.xyz),sourceColor.a);
    
    //    czm_saturation(rgb, 0)
    
    resultHardLight = destColorSaturate;
    
    resultHardLight.rgb *= resultHardLight.a;
    
    destColor.rgb *= 1.0 - resultHardLight.a;
    
    gl_FragColor.rgb = resultHardLight.rgb + destColor.rgb;
    gl_FragColor.a = 1.0;
}


