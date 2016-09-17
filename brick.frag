precision highp float;

uniform highp vec4 materialDiffuseColor0;
uniform highp vec4 materialDiffuseColor1;

varying highp vec3 OS_position;
varying float LightIntensity;

void main()
{    
	vec2 BrickSize = vec2(0.1, 0.05);
	vec2 BrickPct = vec2(0.9, 0.9);
	vec3 BrickColor =  materialDiffuseColor0.xyz;
	vec3 MortarColor = materialDiffuseColor1.xyz;
    
    float theta = asin(OS_position.y / length(OS_position.xyz));
    float phi = atan(OS_position.z , OS_position.x);
    
	highp vec2 MCposition  = vec2(phi, theta);
	
    vec3 color;
	vec2 position, useBrick;

	position = MCposition / BrickSize;
	
	if (fract(position.y * 0.5) > 0.5)
		position.x += 0.5;

	position = fract(position);

	useBrick = step(position, BrickPct);

	color    = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);
	color   *= LightIntensity * 1.5;
	gl_FragColor = vec4(color, 1.0);
}