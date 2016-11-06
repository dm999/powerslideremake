#version 120

uniform mat4 modelviewproj;
uniform mat4 texturematrix;

attribute vec4 position;
attribute vec2 uv0;
attribute vec4 colour;


varying vec2 T;
varying vec4 C;


void main()
{
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    C = colour.rgba;
    gl_Position = modelviewproj * position;
}

