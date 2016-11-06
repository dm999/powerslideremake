#include <stdio.h>
#include <stddef.h>


#include "OpenALState.h"
#include "OpenALSource.h"
#include "LinearController.h"
#include "DMChrono.h"





int main()
{
    OpenALState state;

    state.init();

    const float maxTime = 10.0f;

    DMChrono chrono;

    LinearController<float> pitch;
    pitch.addPoint(0.0f, 0.25f);

    LinearController<float> gain;
    gain.addPoint(maxTime  * 0.3f, 1.0f);
    gain.addPoint(maxTime  * 0.6f, 0.0f);


    LinearController<float> pitch2;
    pitch2.addPoint(0.0f, 0.15f);

    LinearController<float> gain2;
    gain2.addPoint(maxTime * 0.3f, 1.0f);
    gain2.addPoint(maxTime * 0.6f, 0.0f);


    LinearController<float> pitch3;
    pitch3.addPoint(maxTime * 0.4f, 0.5f);
    pitch3.addPoint(maxTime, 1.0f);

    LinearController<float> gain3;
    gain3.addPoint(maxTime * 0.4f, 0.0f);
    gain3.addPoint(maxTime * 0.42f, 1.0f);


    OpenALSource wavData("low.wav", 1.4f);
    OpenALSource wavData2("mid.wav", 1.2f);
    OpenALSource wavData3("high.wav", 1.0f);

    wavData.setLooping(AL_TRUE);
    wavData2.setLooping(AL_TRUE);
    wavData3.setLooping(AL_TRUE);

    wavData.setGain(gain.getVal(0.0f));
    wavData2.setGain(gain2.getVal(0.0f));
    wavData3.setGain(gain3.getVal(0.0f));

    wavData.setPitch(pitch.getVal(0.0f));
    wavData2.setPitch(pitch2.getVal(0.0f));
    wavData3.setPitch(pitch3.getVal(0.0f));

    wavData.startPlaying();
    wavData2.startPlaying();
    wavData3.startPlaying();

    chrono.Start();

     while(1)
     {
         float timeR = static_cast<float>(chrono.GetTimeDiffFromFirst());

         printf("%.2f\n", timeR);

         wavData.setPitch(pitch.getVal(timeR));
         wavData.setGain(gain.getVal(timeR));

         wavData2.setPitch(pitch2.getVal(timeR));
         wavData2.setGain(gain2.getVal(timeR));

         wavData3.setPitch(pitch3.getVal(timeR));
         wavData3.setGain(gain3.getVal(timeR));

         if(timeR > maxTime)
         {
             wavData.stopPlaying();
             wavData2.stopPlaying();
             wavData3.stopPlaying();
             break;
         }
     }

     wavData.clearALData();
     wavData2.clearALData();
     wavData3.clearALData();


    state.deInit();

    return 0;
}