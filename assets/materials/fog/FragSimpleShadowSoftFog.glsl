#version 120

uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;

uniform vec4 fogColor;
uniform vec4 fogParams;

uniform vec4 lightAmbient;
uniform vec4 matAmbient;

uniform vec4 textureSize;

varying vec2 T;
varying vec3 C;

varying vec4 VPosInLightSpace;
varying float shadowFade;

vec2 dxdy(sampler2D tex, vec2 coords, float xOffset, float yOffset)
{
    vec2 res;
    float original = texture2D(tex, coords).x;
    
    float dy = original - texture2D(tex, coords + vec2(0.0, yOffset)).x;
    float dx = original - texture2D(tex, coords + vec2(xOffset, 0.0)).x;
    res.x = abs(dx + dy);
    
    dy = original - texture2D(tex, coords - vec2(0.0, yOffset)).x;
    dx = original - texture2D(tex, coords - vec2(xOffset, 0.0)).x;
    res.y = abs(dx + dy);
    return res;
}

void main()
{

    float shadow = 0.0;
    float shadowColor;
    const float shadowDarkness = 0.3;
    
    vec3 lightZ  = vec3(0.0, 0.0, 0.0);
    if(VPosInLightSpace.w > 0.0)
    {
        lightZ = VPosInLightSpace.xyz / VPosInLightSpace.w;
            
        float xPixelOffset = 1.0 / textureSize.x;
        float yPixelOffset = 1.0 / textureSize.y;
        vec2 dxdy_val = dxdy(shadowMap, lightZ.xy, 10.0 * xPixelOffset, 10.0 * yPixelOffset);

        float centerdepth;

        if(dxdy_val.x > 0.000001 || dxdy_val.y > 0.000001)
        {
            const float shadowDelta = 1.0 / 16.0;
            for (float y = -1.5 ; y <=1.5 ; y+=1.0) 
            for (float x = -1.5 ; x <=1.5 ; x+=1.0) 
            {
                centerdepth = texture2D(shadowMap, lightZ.xy + vec2( x * xPixelOffset, y * yPixelOffset )).r;
                if(centerdepth <= lightZ.z) shadow += shadowDelta;
            }
        }
        else
        {
            centerdepth = texture2D(shadowMap, lightZ.xy).r;
            if(centerdepth <= lightZ.z) shadow = 1.0;
        }

    }
    
    shadowColor = 1.0 - shadowDarkness * shadow * (1.0 - shadowFade);

    vec3 ambient = matAmbient.xyz * lightAmbient.xyz;

    //fog
    //const float LOG2 = 1.442695;
    //const float fogDensity = 0.005;
    float zDist = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = (fogParams.z - zDist) * fogParams.w;
    //float fogFactor = exp2( -fogDensity  * fogDensity  * zDist * zDist * LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0); 

    vec3 sceneColor = texture2D(diffuseMap, T).rgb * (ambient + C.rgb) * shadowColor;
    gl_FragColor = vec4(mix(fogColor.rgb, sceneColor, fogFactor ), 1.0);

}
