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
attribute vec2 uv0;
attribute vec4 colour;

varying vec3 L[lightCount];
varying float Att[lightCount];
varying vec2 T;
varying vec3 C;

varying vec4 VPosInLightSpace;

void main()
{
    vec4 P = modelview * position;
    
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
    
    T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    C = colour.rgb;
    gl_Position = modelviewproj * position;
}

