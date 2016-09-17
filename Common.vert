#version 300 es
// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix