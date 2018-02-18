#version 100

precision highp float;

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 matAmbient;

uniform vec4 fogColor;
uniform vec4 fogParams;

varying vec2 T;
varying vec3 C;

void main()
{

    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    
    float zDist = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = (fogParams.z - zDist) * fogParams.w;
    fogFactor = clamp(fogFactor, 0.0, 1.0); 

    vec3 sceneColor = texture2D(diffuseMap, T).rgb * (ambient + C.rgb);

    gl_FragColor = vec4(mix(fogColor.rgb, sceneColor, fogFactor ), 1.0);
}
