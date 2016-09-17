attribute vec3 position;
attribute vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normMatrix;
uniform float Time;

varying vec4 materialDiffuseColor;
varying vec4 ES_position;
varying vec3 ES_normal;

const float M_PI = 3.14159;

void main(void)
{
	float r = length(position.xyz);
	float theta = asin(position.y/r);
	theta = theta/M_PI+0.5;

	float phi = atan(position.z,position.x);
	phi = phi/M_PI/2.0+0.5;

	float colorAngle;
	//colorAngle = phi;
    //colorAngle = theta;
    //colorAngle = (theta + 2.0 * phi);
    colorAngle = (theta + 2.0 * phi) + Time / 10.0;
    
    float number = floor(colorAngle);
    if (number > 0.0)
        colorAngle = colorAngle - number;
    
    float H = colorAngle * 360.0;
    float S = 1.0;
    float V = 1.0;
    
    float h = floor(H / 60.0);
    float f = H / 60.0 - h;
    
    float p = V * (1.0 - S);
    float q = V * (1.0 - S * f);
    float t = V * (1.0 - S * (1.0 - f));
    
    vec4 color;
    
    if (h <= 0.0)
        color = vec4(V, t, p, 1.0);
    else if (h == 1.0)
        color = vec4(q, V, p, 1.0);
    else if (h == 2.0)
        color = vec4(p, V, t, 1.0);    
    else if (h == 3.0)
        color = vec4(p, q, V, 1.0);    
    else if (h == 4.0)
        color = vec4(t, p, V, 1.0);    
    else if (h == 5.0)
        color = vec4(V, p, q, 1.0); 
    else if (h == 6.0)
        color = vec4(V, t, p, 1.0); 


    materialDiffuseColor = color;
	mat4 model_view_mat = view*model;
    ES_normal = normMatrix * normal;
    ES_position = model_view_mat * vec4(position,1.0);
    gl_Position = projection * ES_position;
}
