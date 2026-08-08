#version 120

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse[lightCount];
uniform vec4 lightSpecular[lightCount];
uniform vec4 matAmbient;
uniform vec4 matDiffuse;
uniform vec4 matSpecular;
uniform float matSpecPower;

varying vec3 N;
varying vec3 L[lightCount];
varying float Att[lightCount];
varying vec3 V;
varying vec2 T;

#ifdef useReflection
uniform samplerCube reflectionMap;
varying vec3 Rw;
const float reflectionAmount = 0.25;
#endif

//F_Point_MaterialTex
void main()
{

    
    vec3 normal = normalize(N);
    vec3 viewDir = normalize(V);
    
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    
    for(int i = 0; i < lightCount; i++)
    {
        float att = max(0.0, 1.0 - dot(L[i], L[i]));
        att *= Att[i];
        
        vec3 lightDir = normalize(L[i]);
        vec3 R = reflect(-lightDir, normal);
        float NdotL = max(dot(normal, lightDir), 0.0);
        float NdotHV = max(dot(R, viewDir), 0.0);
        
        diffuse += att * NdotL * matDiffuse.xyz * lightDiffuse[i].xyz;
        specular += att * pow(NdotHV, matSpecPower) * matSpecular.xyz * lightSpecular[i].xyz;
    }
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;

#ifdef useReflection
    //float F0 = 0.001;
    //float F0 = 0.04;
    float F0 = 0.1;
    //float F0 = 0.4;
    //float F0 = 1.0;
    float NdotV = max(dot(normal, viewDir), 0.0);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
    
    vec3 lit = texture2D(diffuseMap, T).rgb * (ambient + diffuse + specular);
    vec3 refl = textureCube(reflectionMap, normalize(Rw)).rgb;
    //gl_FragColor = vec4(mix(lit, refl, reflectionAmount), 1.0);
    gl_FragColor = vec4(mix(lit, refl, fresnel), 1.0);
#else
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + diffuse + specular), 1.0);
#endif
}
