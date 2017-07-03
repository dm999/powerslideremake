#version 100

precision highp float;

uniform sampler2D diffuseMap;

uniform vec4 fogColor;
uniform vec4 fogParams;

varying vec2 T;

void main()
{
    //fog
    //const float LOG2 = 1.442695;
    //const float fogDensity = 0.005;
    float zDist = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = (fogParams.z - zDist) * fogParams.w;
    //float fogFactor = exp2( -fogDensity  * fogDensity  * zDist * zDist * LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0); 
    
    gl_FragColor = vec4(mix(fogColor.rgb, texture2D(diffuseMap, T).rgb, fogFactor ), 1.0);
}
