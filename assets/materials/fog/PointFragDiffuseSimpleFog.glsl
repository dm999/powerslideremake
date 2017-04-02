#version 120

uniform sampler2D diffuseMap;

uniform vec4 fogColor;
uniform vec4 fogParams;

uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 matAmbient;

varying vec3 N;
varying vec3 L;
varying vec2 T;

void main()
{
    vec3 lightDir = normalize(L);
    vec3 normal = normalize(N);
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * lightDiffuse.xyz;
    
    //fog
    //const float LOG2 = 1.442695;
    //const float fogDensity = 0.005;
    float zDist = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = (fogParams.z - zDist) * fogParams.w;
    //float fogFactor = exp2( -fogDensity  * fogDensity  * zDist * zDist * LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0); 
   
    vec3 sceneColor = texture2D(diffuseMap, T).rgb * (ambient + diffuse);
    gl_FragColor = vec4(mix(fogColor.rgb, sceneColor, fogFactor ), 1.0);
}
