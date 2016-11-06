#version 120

uniform mat4 modelviewproj;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 normalmatrix;
uniform mat4 texturematrix;
uniform vec4 lightPosition[lightCount];
uniform vec4 lightAttenuation[lightCount]; //lightAttenuation.x as lightRadius;
uniform mat4 texviewproj;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 uv0;

varying vec3 N;
varying vec3 L[lightCount];
varying float Att[lightCount];
varying vec3 V;
varying vec2 T;

varying vec4 VPosInLightSpace;

void main()
{
    vec4 P = modelview * position;
    N = vec3(normalmatrix * vec4(normal, 0.0));
    
    for(int i = 0; i < lightCount; i++)
    {
        float lightRadiusIn = lightAttenuation[i].y;
        float lightRadiusOut = lightAttenuation[i].x;
        
        vec3 tempL = (lightPosition[i].xyz - P.xyz);
        
        float dist = length(tempL);
        Att[i] = clamp((lightRadiusOut - dist) / (lightRadiusOut - lightRadiusIn), 0.0, 1.0);
        
        L[i] = tempL / lightRadiusOut;
    }
    
    VPosInLightSpace = texviewproj * (model * position);
    
    V = -P.xyz;
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    gl_Position = modelviewproj * position;
}

