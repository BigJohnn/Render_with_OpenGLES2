precision mediump float;
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix

uniform sampler2D background;
uniform vec4 GlassParams;

varying vec4 TexCoord;
varying vec3 LightVec;
varying vec3 ViewVec;
varying vec3 Normal;

void main()
{
    vec2 UV             = TexCoord.xy;
    vec2 ScreenUV       = TexCoord.zw * 1.0;
    vec3 norm           = normalize( Normal );
    vec3 Light          = normalize( LightVec );
    vec3 View           = normalize( ViewVec );
    vec3 Half           = normalize( Light + View );
    float NV            = max( 0.0, dot( norm, View ) );
    vec3 BendVec        = reflect( -View, norm );
    vec3 GlassColor     = GlassParams.xyz;
    float Distort       = GlassParams.w;

    vec4 BGColor    = texture2D( background, ScreenUV - BendVec.xy * Distort );
    vec4 EnvColor   = texture2D( background, ScreenUV + BendVec.xy * Distort );

    // Fresnel
    float F = saturate( pow( ( 1.0 - NV ), 2.0 ) );

    // Specular
    float Specular  = pow( saturate( dot( Half, norm ) ), 100.0 ) * 4.0;

    // refraction + env + spec
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    gl_FragColor.rgb += BGColor.rgb * GlassColor * ( 1.0 - F );
    gl_FragColor.rgb += ( EnvColor.rgb + GlassColor * 0.5 ) * F;
    gl_FragColor.rgb += vec3( Specular * 0.7 );
}