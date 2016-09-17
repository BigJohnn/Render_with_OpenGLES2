precision mediump float;

void main()
{             
	gl_FragColor.r = gl_FragCoord.z;
	//gl_FragColor = color;
	//float value = 10.0 - TEXCOORD.z;
 //   float v = floor(value);
 //   float f = value - v;
 //   float vn = v * 0.1;
 //   gl_FragColor = vec4(vn, f, 0.0, 1.0);
}