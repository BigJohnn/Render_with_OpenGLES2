precision mediump float;

varying highp float LightIntensity;
varying highp vec3 MS_position;

//Create uniform variables so dots can be spaced and scaled by user
//uniform vec3 Spacing;
//uniform float DotSize;

const highp vec3 Spacing = vec3(0.8, 0.8, 0.64);
const highp float DotSize = 0.3;

//Create colors as uniform variables so they can be easily changed

uniform highp vec4 materialDiffuseColor0;
uniform highp vec4 materialDiffuseColor1;

// uniform vec3 ModelColor, PolkaDotColor;

void main(void)
{

    highp vec3 ModelColor = materialDiffuseColor0.xyz;
    highp vec3 PolkaDotColor = materialDiffuseColor1.xyz;

   highp float insidesphere, sphereradius, scaledpointlength;
   highp vec3 scaledpoint, finalcolor;

   // Scale the coordinate system
   // The following line of code is not yet implemented in current drivers:
   // mcpos = mod(Spacing, MCposition);
   // We will use a workaround found below for now
   scaledpoint       = MS_position - (Spacing * floor(MS_position/Spacing));

   // Bring the scaledpoint vector into the center of the scaled coordinate system
   scaledpoint       = scaledpoint - Spacing/2.0;

   // Find the length of the scaledpoint vector and compare it to the dotsize
   scaledpointlength = length(scaledpoint);
   insidesphere      = step(scaledpointlength,DotSize);
   
   // Determine final output color before lighting
   finalcolor        = vec3(mix(ModelColor, PolkaDotColor, insidesphere));

   // Output final color and factor in lighting
   gl_FragColor      = clamp(vec4(finalcolor * LightIntensity, 1.0), 0.0, 1.0);

}
