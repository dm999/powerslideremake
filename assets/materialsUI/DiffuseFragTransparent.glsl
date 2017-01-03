#version 120

uniform sampler2D diffuseMap;

varying vec2 T;

void main()
{
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgba);
}
