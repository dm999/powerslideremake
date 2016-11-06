#version 120

uniform mat4 modelviewproj;
uniform mat4 modelview;
uniform mat4 normalmatrix;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 uv0;

varying vec3 N;
varying vec3 V;
varying vec2 T;

uniform float time;
uniform float waveFreq;
uniform float waveAmp;

// wave functions
struct Wave {
  float freq;  // 2*PI / wavelength
  float amp;   // amplitude
  float phase; // speed * 2*PI / wavelength
  vec2 dir;
};

//dir
void main()
{
#define NWAVES 2
Wave wave[NWAVES];
wave[0] = Wave( waveFreq, waveAmp, 0.5, vec2(-1, 0) );
wave[1] = Wave( 3.0 * waveFreq, 0.33 * waveAmp, 1.7, vec2(-0.7, 0.7) );




    vec4 P = modelview * position;
    vec4 PP = position;
    
    // sum waves
    float ddx = 0.0, ddy = 0.0;
    float deriv;
    float angle;

    // wave synthesis using two sine waves at different frequencies and phase shift
    for(int i = 0; i<NWAVES; ++i)
    {
        angle = dot(wave[i].dir, PP.xz) * wave[i].freq + time * wave[i].phase;
        PP.y += wave[i].amp * sin( angle );
        // calculate derivate of wave function
        deriv = wave[i].freq * wave[i].amp * cos(angle);
        ddx -= deriv * wave[i].dir.x;
        ddy -= deriv * wave[i].dir.y;
    }
    
    N = vec3(normalmatrix * vec4(normal, 0.0));
    V = -P.xyz;
    //T = (texturematrix * vec4(uv0, 0.0, 1.0)).xy;
    T = uv0;
    
    gl_Position = modelviewproj * PP;
}

