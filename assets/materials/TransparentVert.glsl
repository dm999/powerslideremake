#version 120

uniform mat4 modelviewproj;

attribute vec4 position;

void main()
{
    gl_Position = modelviewproj * position;
}

