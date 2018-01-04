#version 100

precision highp float;

varying vec3 C;

void main()
{
    gl_FragColor = vec4(C.rgb, 1.0);
}
