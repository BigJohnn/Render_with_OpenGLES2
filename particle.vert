uniform float Time;
uniform vec3 centerPosition;

attribute float lifetime;
attribute vec3 startPos;
attribute vec3 endPos;
varying float vlifetime;

void main()
{
	if(Time<=lifetime+10.0)
	{
		gl_Position.xyz = startPos + Time*endPos;
		gl_Position.xyz+=centerPosition;
		gl_Position.w = 1.0;
	}
	else
	{
		gl_Position = vec4(0, 0, 0, 0);
	}
	vlifetime = 1.0-(Time/lifetime);
	vlifetime = clamp(vlifetime, 0.0, 1.0);
	gl_PointSize = (vlifetime * vlifetime) *40.0;
	gl_PointSize = Time*40.0;
}