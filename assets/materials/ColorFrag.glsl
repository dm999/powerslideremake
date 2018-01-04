#version 120

varying vec3 C;

void main()
{
    gl_FragColor = vec4(C.rgb, 1.0);
}
