#version 120

uniform sampler2D diffuseMap;
uniform vec4 lightAmbient;
uniform vec4 matAmbient;

varying vec2 T;
varying vec3 C;

void main()
{

    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;

    //gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + C.rgb), 1.0);
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * C.rgb, 1.0);
}
