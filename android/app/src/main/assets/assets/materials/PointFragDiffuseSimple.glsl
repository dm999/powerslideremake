#version 100

precision highp float;

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 matAmbient;

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

void main()
{
    vec3 lightDir = normalize(L);
    vec3 normal = normalize(N);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * lightDiffuse.xyz;
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + diffuse), 1.0);
}
