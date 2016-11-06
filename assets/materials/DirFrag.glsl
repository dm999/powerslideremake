#version 120

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightSpotDirection;
uniform vec4 matAmbient;
uniform vec4 matSpecular;
uniform float matSpecPower;

varying vec3 N;
varying vec3 V;
varying vec2 T;

void main()
{
    vec3 normal = normalize(N);
    vec3 viewDir = normalize(V);
    vec3 R = reflect(lightSpotDirection.xyz, normal);
    float NdotL = max(dot(normal, -lightSpotDirection.xyz), 0.0);
    float NdotHV = max(dot(R, viewDir), 0.0);
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * vec3(1.0, 1.0, 1.0) * lightDiffuse.xyz;
    vec3 specular = pow(NdotHV, matSpecPower) * matSpecular.xyz * lightSpecular.xyz;
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
    //gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + att * (diffuse + specular)), 1.0);
}
