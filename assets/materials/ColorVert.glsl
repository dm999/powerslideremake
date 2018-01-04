#version 120

uniform mat4 modelviewproj;

attribute vec4 position;
attribute vec4 colour;

varying vec3 C;

void main()
{
    C = colour.rgb;
    gl_Position = modelviewproj * position;
}

