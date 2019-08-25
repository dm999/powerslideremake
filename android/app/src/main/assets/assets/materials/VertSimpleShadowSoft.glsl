#version 100

uniform mat4 modelviewproj;
uniform mat4 model;
uniform mat4 texturematrix;
uniform mat4 texviewproj;
uniform float shadowFadeRange;

attribute vec4 position;
attribute vec2 uv0;
attribute vec4 colour;

varying vec2 T;
varying vec3 C;

varying vec4 VPosInLightSpace;
varying float shadowFade;

void main()
{

    VPosInLightSpace = texviewproj * (model * position);
    
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    C = colour.rgb;
    vec4 pos = modelviewproj * position;
    shadowFade = clamp(pos.z * shadowFadeRange, 0.0, 1.0);
    gl_Position = pos;
}

