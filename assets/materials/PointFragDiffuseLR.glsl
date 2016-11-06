#version 120

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightAttenuation;
uniform vec4 matAmbient;
uniform vec4 matSpecular;
uniform float matSpecPower;

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

//F_Point_MaterialTex
void main()
{
    float att = max(0.0, 1.0 - dot(L, L));

    vec3 lightDir = normalize(L);
    vec3 normal = normalize(N);
    vec3 viewDir = normalize(V);
    vec3 R = reflect(-lightDir, normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotHV = max(dot(R, viewDir), 0.0);
    
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * lightDiffuse.xyz;
    vec3 specular = pow(NdotHV, matSpecPower) * matSpecular.xyz * lightSpecular.xyz;
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + att * (diffuse + specular)), 1.0);
}
