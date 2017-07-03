#version 100

uniform mat4 modelviewproj;
uniform vec4 texelOffsets;

attribute vec4 position;

varying vec2 depth;

void main()
{
    vec4 P = modelviewproj * position;
    P.xy += texelOffsets.zw * P.w;
    depth.x = P.z;
    depth.y = P.w;
    gl_Position = P;
}