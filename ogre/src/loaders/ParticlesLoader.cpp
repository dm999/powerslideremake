#include "../pcheader.h"

#include "ParticlesLoader.h"

namespace
{
    struct PSParticle
    {
        int index1;
        int index2;
        int R1,G1,B1;
        int R2,G2,B2;
        bool isVisible;
    };
}//anonymous namespace

void ParticlesLoader::load(GameState& gameState) const
{
    Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) + "/graphst", "partgraph.txt");
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        gameState.getParticles().clear();

        for(size_t q = 0; q < 16; ++q)
        {
            Particle particle;
            PSParticle part;

            char buf[1025];
            fileToLoad->readLine(buf, 1024);
            sscanf(buf, "%d %d %d %d %d %d\n", &part.R1, &part.G1, &part.B1, &part.R2, &part.G2, &part.B2);

            fileToLoad->readLine(buf, 1024);
            sscanf(buf, "%d\n", &part.index1);

            fileToLoad->readLine(buf, 1024);
            sscanf(buf, "%d\n", &part.index2);

            fileToLoad->readLine(buf, 1024);//AGESTEP
            fileToLoad->readLine(buf, 1024);//DRAG
            fileToLoad->readLine(buf, 1024);//GRAVITY
            fileToLoad->readLine(buf, 1024);//C1
            fileToLoad->readLine(buf, 1024);//C2
            fileToLoad->readLine(buf, 1024);//C3
            fileToLoad->readLine(buf, 1024);//RANDMAX
            fileToLoad->readLine(buf, 1024);//WIDTH? or ALPHA
            if(buf[0] == 'W' || buf[0] == 'w')
                fileToLoad->readLine(buf, 1024);//ALPHA
            fileToLoad->readLine(buf, 1024);//SIZE
            float skid[11];
            fileToLoad->readLine(buf, 1024);
            char buf2[256] = {0};
            sscanf(buf, "%s %f %f %f %f %f %f %f %f %f %f %f\n", buf2, &skid[0], &skid[1], &skid[2], &skid[3], &skid[4], &skid[5], &skid[6], &skid[7], &skid[8], &skid[9], &skid[10]);
            fileToLoad->readLine(buf, 1024);
            fileToLoad->readLine(buf, 1024);

            part.isVisible = false;
            for(size_t w = 1; w < 11; ++w)
            {
                if(skid[w] != 0.0f)
                {
                    part.isVisible = true;
                    break;
                }
            }

            particle.rgb1.r = part.R1 / 255.0f;
            particle.rgb1.g = part.G1 / 255.0f;
            particle.rgb1.b = part.B1 / 255.0f;
            particle.rgb1.a = 1.0f;

            particle.rgb2.r = part.R2 / 255.0f;
            particle.rgb2.g = part.G2 / 255.0f;
            particle.rgb2.b = part.B2 / 255.0f;
            particle.rgb2.a = 1.0f;

            particle.index = part.index1;

            particle.isVisible = part.isVisible;

            gameState.getParticles().push_back(particle);
        }

        fileToLoad->close();
    }
    else {assert(false && "No particle file");}
}