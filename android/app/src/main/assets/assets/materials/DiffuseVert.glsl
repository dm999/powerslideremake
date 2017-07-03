#version 100

uniform mat4 modelviewproj;
uniform mat4 texturematrix;

attribute vec4 position;
attribute vec2 uv0;

varying vec2 T;

void main()
{
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    gl_Position = modelviewproj * position;
}

