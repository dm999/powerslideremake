#version 120

varying vec2 depth;

void main()
{
    float normalizedDistance = depth.x / depth.y;
    normalizedDistance += 0.0008;
    gl_FragColor = vec4(normalizedDistance, normalizedDistance, normalizedDistance, 1.0);
}