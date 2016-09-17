#ifdef GL_ES
    #extension GL_EXT_shader_framebuffer_fetch : require
#endif

#ifndef GL_ES
    #define lowp
    #define highp
    #define mediump
    #define precision_highp_float
    #define precision_mediump_float
    #define precision_lowp_float
#else
    #define precision_highp_float precision highp float;
    #define precision_mediump_float precision mediump float;
    #define precision_lowp_float precision lowp float;
#endif

#ifdef GL_ES
    
    uniform sampler2D SamplerUV;

    lowp vec3 sampleVideoImpl(sampler2D SamplerY, sampler2D SamplerUV, highp vec2 uv) {
        mediump vec3 yuv;
        lowp vec3 rgb;

        yuv.x = texture2D(SamplerY, uv).r;
        yuv.yz = texture2D(SamplerUV, uv).rg - vec2(0.5, 0.5);

        // BT.601, which is the standard for SDTV is provided as a reference
        /*
            rgb = mat3(    1,       1,     1,
            0, -.34413, 1.772,
            1.402, -.71414,     0) * yuv;
        */

        // Using BT.709 which is the standard for HDTV
        rgb = mat3(      1,       1,      1,
            0, -.18732, 1.8556,
            1.57481, -.46813,      0) * yuv;
            return rgb;
    }

    #define sampleVideo(texCoord) sampleVideoImpl(SamplerY, SamplerUV, texCoord)
    #define sampleVideoLuma(texCoord) texture2D(SamplerY, texCoord).r

    #define sampleLastFragData() gl_LastFragData[0]
#else
    uniform sampler2DRect SamplerUV;

    vec3 sampleVideoImpl(sampler2DRect Sampler, vec2 size, vec2 uv) {
        uv.x = size.x * (1.0 - uv.x);
        uv.y = size.y * uv.y;
        return texture2DRect(Sampler, uv).rgb;
    }

    #define sampleVideo(texCoord) sampleVideoImpl(SamplerY, videoSize, texCoord)
    #define sampleVideoLuma(texCoord) dot(sampleVideoImpl(SamplerY, videoSize, texCoord).rgb, vec3(0.299, 0.587, 0.114))

    uniform sampler2D SamplerScreen;
    uniform vec2 SamplerScreenInvSize;

    #define sampleLastFragData() texture2D(SamplerScreen, gl_FragCoord.xy * SamplerScreenInvSize)
#endif

mediump vec2 encodeFloat2( mediump float v ) {
    mediump vec2 enc = vec2(1.0, 255.0) * clamp(v, 0.0, 1.0);
    enc = fract(enc);
    enc -= enc.yy * vec2(1.0/255.0, 0.0);
    return enc;
}

mediump float decodeFloat2( mediump vec2 v ) {
    return v.x + v.y/255.0;
}

precision mediump float;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_lightTexture;

varying highp vec2 texCoordVarying;
//uniform float offsetX;
//uniform float offsetY;
//uniform float influence;

float offsetX = 0.5;
float offsetY = 0.5;
float influence = 0.7;

void main()
{
    lowp vec4 destColor = texture2D(u_diffuseTexture, gl_FragCoord.xy * texCoordVarying);
    lowp vec4 sourceColor = texture2D(u_diffuseTexture, texCoordVarying);
    
    lowp vec4 lightColor = texture2D(u_lightTexture, texCoordVarying.xy + vec2(offsetX, offsetY));
    
    lightColor.xyz *= influence;
    sourceColor.xyz = sourceColor.xyz + sourceColor.xyz*lightColor.xyz;
    
    lowp vec3 result = destColor.xyz + sourceColor.xyz;
    
    result.rgb *= sourceColor.a;
    
    destColor.rgb *= 1.0 - sourceColor.a;
    
    gl_FragColor.rgb = result.rgb + destColor.rgb;
    gl_FragColor.a = 1.0;
}