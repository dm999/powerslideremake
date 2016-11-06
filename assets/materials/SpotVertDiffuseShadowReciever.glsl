#version 120

uniform mat4 modelviewproj;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 normalmatrix;
uniform mat4 texturematrix;
uniform vec4 lightPosition;
uniform mat4 texviewproj;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 uv0;

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

varying vec4 VPosInLightSpace;

//V_Point_MaterialTex
void main()
{
    vec4 P = modelview * position;
    N = vec3(normalmatrix * vec4(normal, 0.0));
    L = lightPosition.xyz - P.xyz;
    V = -P.xyz;
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    VPosInLightSpace = texviewproj * (model * position);
    gl_Position = modelviewproj * position;
}

