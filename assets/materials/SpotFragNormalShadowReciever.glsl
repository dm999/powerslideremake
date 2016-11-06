#version 120

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightSpotDirection;
uniform vec4 lightAttenuation;
uniform vec4 lightSpot;
uniform vec4 matAmbient;
uniform vec4 matSpecular;
uniform float matSpecPower;

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

varying mat3 TBN;

varying vec4 VPosInLightSpace;

//F_Spot_NormalMap
void main()
{

    vec3 lightZ  = vec3(0.0, 0.0, 0.0);
    if(VPosInLightSpace.w != 0.0)
    {
        lightZ = VPosInLightSpace.xyz / VPosInLightSpace.w;
    }
    float centerdepth = texture2D(shadowMap, lightZ.xy).x;
    float shadow = 1.0;
    if(centerdepth <= lightZ.z)
        shadow = 0.2;
    
    vec3 lightSpotDir = TBN * lightSpotDirection.xyz;
    float dist = length(L);
    vec3 lightDir = normalize(L);
    vec3 normal = normalize(texture2D(normalMap, T).rgb * 2.0 - 1.0);
    vec3 viewDir = normalize(V);
    vec3 R = reflect(-lightDir, normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotHV = max(dot(R, viewDir), 0.0);
    float spotDot = dot(lightSpotDir, -lightDir);
    float spotEffect = smoothstep(lightSpot.y, lightSpot.x, spotDot);
    spotEffect = pow(spotEffect, lightSpot.z);
    
    float att = 0.0;
    
    float attFactor = lightAttenuation.y + lightAttenuation.z * dist + lightAttenuation.w * dist * dist;
        if(lightAttenuation.x > dist && attFactor != 0.0)
            att = 1.0 / attFactor;

    att *= spotEffect;
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * lightDiffuse.xyz;
    vec3 specular = pow(NdotHV, matSpecPower) * matSpecular.xyz * lightSpecular.xyz;
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + att * (diffuse + specular) * shadow), 1.0);
}
