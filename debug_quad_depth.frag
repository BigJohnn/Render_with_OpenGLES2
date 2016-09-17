precision mediump float;

varying vec2 TexCoords;

//layout(location = 0) out float depthValue;
//uniform highp sampler2DShadow depthMap;
uniform sampler2D depthMap;

void main()
{            
    float depthValue = texture2D(depthMap, TexCoords).r;
	gl_FragColor = vec4(vec3(depthValue), 1.0);
	//color.r = textureProj(depthMap, vec4(TexCoords,0.0,1.0));
}

