#version 120

uniform sampler2D diffuseMap;

varying vec2 T;
varying vec4 C;


void main()
{
    gl_FragColor = vec4(C.rgb * C.a * texture2D(diffuseMap, T).a, 1.0);
}
