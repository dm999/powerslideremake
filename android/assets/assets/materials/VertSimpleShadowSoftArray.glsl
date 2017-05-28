#version 100

uniform mat4 modelviewproj;
uniform mat4 model;
uniform mat4 texturematrix;
uniform mat4 texviewproj;

attribute vec4 position;
attribute vec3 uv0;
attribute vec4 colour;

varying vec3 T;
varying vec3 C;

varying vec4 VPosInLightSpace;

void main()
{

    VPosInLightSpace = texviewproj * (model * position);
    
    T = (texturematrix * vec4(uv0, 1.0)).xyz;
    C = colour.rgb;
    gl_Position = modelviewproj * position;
}

