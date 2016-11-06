#version 120

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

void main()
{
    vec3 normal = normalize(N);
    gl_FragColor = vec4(normal, 1.0);
}
