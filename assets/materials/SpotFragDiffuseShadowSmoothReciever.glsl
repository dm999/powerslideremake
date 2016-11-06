#version 120

uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;
uniform vec4 lightAmbient;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightSpotDirection;
uniform vec4 lightAttenuation;
uniform vec4 lightSpot;
uniform vec4 matAmbient;
uniform vec4 matSpecular;
uniform float matSpecPower;
uniform vec4 textureSize;

varying vec3 N;
varying vec3 L;
varying vec3 V;
varying vec2 T;

varying vec4 VPosInLightSpace;

vec2 dxdy(sampler2D tex, vec2 coords, float xOffset, float yOffset)
{
    vec2 res;
    float dx = texture2D(tex, coords).x;
    res.y = dx;
    float dy = dx - texture2D(tex, coords + vec2(0.0, yOffset)).x;
    dx -= texture2D(tex, coords + vec2(xOffset, 0.0)).x;
    res.x = abs(dx + dy);
    return res;
}

//F_Point_MaterialTex
void main()
{

    vec3 lightZ  = vec3(0.0, 0.0, 0.0);
    if(VPosInLightSpace.w != 0.0)
    {
        lightZ = VPosInLightSpace.xyz / VPosInLightSpace.w;
    }
    float shadow = 0.0;
    float shadowColor;
    const float shadowDarkness = 0.9;
    const float shadowDelta = 1.0 / 9.0;
    float xPixelOffset = 1.0 / textureSize.x;
    float yPixelOffset = 1.0 / textureSize.y;
    float dx0 = -2.5 * xPixelOffset;
    float dy0 = -2.5 * yPixelOffset;
    float dx1 = 2.5 * xPixelOffset;
    float dy1 = 2.5 * yPixelOffset;
    vec2 dxdy_val = dxdy(shadowMap, lightZ.xy, xPixelOffset, yPixelOffset);

    float centerdepth;

    if(dxdy_val.x > 0.0001)
    {
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx0, dy0 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( 0.0, dy0 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx1, dy0 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx0, 0.0 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx1, 0.0 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx0, dy1 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( 0.0, dy1 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = texture2D(shadowMap, lightZ.xy + vec2( dx1, dy1 )).r;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
        centerdepth = dxdy_val.y;
        if(centerdepth <= lightZ.z) shadow += shadowDelta;
    }
    else
    {
        centerdepth = dxdy_val.y;
        if(centerdepth <= lightZ.z) shadow += 1.0;
    }
    
    shadowColor = 1.0 - shadowDarkness * shadow;
    
    float dist = length(L);
    vec3 lightDir = normalize(L);
    vec3 normal = normalize(N);
    vec3 viewDir = normalize(V);
    vec3 R = reflect(-lightDir, normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotHV = max(dot(R, viewDir), 0.0);
    float spotDot = dot(lightSpotDirection.xyz, -lightDir);
    float spotEffect = smoothstep(lightSpot.y, lightSpot.x, spotDot);
    spotEffect = pow(spotEffect, lightSpot.z);
    
    float att = 0.0;
    
    float attFactor = lightAttenuation.y + lightAttenuation.z * dist + lightAttenuation.w * dist * dist;
        if(lightAttenuation.x > dist && attFactor != 0.0)
            att = 1.0 / attFactor;

    att *= spotEffect;
    
    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;
    vec3 diffuse = NdotL * lightDiffuse.xyz;
    vec3 specular = pow(NdotHV, matSpecPower) * matSpecular.xyz * lightSpecular.xyz;
    gl_FragColor = vec4(texture2D(diffuseMap, T).rgb * (ambient + att * (diffuse + specular) * shadowColor), 1.0);
}
