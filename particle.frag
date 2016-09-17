precision mediump float;
uniform vec4 ucolor;
varying float vlifetime;
//uniform sampler2D stexture;

void main()
{
	vec4 texColor;
	//texColor = texture2D(stexture, gl_FragCoord.xy/600.0);//gl_PointCoord);
	gl_FragColor = ucolor;//*texColor;
	//gl_FragColor.a *=vlifetime;
}