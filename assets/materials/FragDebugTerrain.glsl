#version 120

uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;
uniform vec4 lightAmbient;
uniform vec4 matAmbient;

uniform vec4 textureSize;

varying vec2 T;
varying vec3 C;

varying vec4 VPosInLightSpace;


void main()
{
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * 16.0, 1.0);
}
