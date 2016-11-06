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

void ParticlesLoader::load(const PFLoader& pfLoader, GameState& gameState) const
{
    FILE * fileToLoad = pfLoader.getFile("data/tracks/" + gameState.getTrackName() + "/graphst", "partgraph.txt");
    if(fileToLoad)
    {
        gameState.getParticles().clear();

        for(size_t q = 0; q < 16; ++q)
        {
            Particle particle;
            PSParticle part;

            fscanf(fileToLoad, "%d %d %d %d %d %d\n", &part.R1, &part.G1, &part.B1, &part.R2, &part.G2, &part.B2);
            fscanf(fileToLoad, "%d\n", &part.index1);
            fscanf(fileToLoad, "%d\n", &part.index2);
            char buf[256] = {0};
            fgets(buf, 256, fileToLoad);//AGESTEP
            fgets(buf, 256, fileToLoad);//DRAG
            fgets(buf, 256, fileToLoad);//GRAVITY
            fgets(buf, 256, fileToLoad);//C1
            fgets(buf, 256, fileToLoad);//C2
            fgets(buf, 256, fileToLoad);//C3
            fgets(buf, 256, fileToLoad);//RANDMAX
            fgets(buf, 256, fileToLoad);//WIDTH? or ALPHA
            if(buf[0] == 'W' || buf[0] == 'w')
                fgets(buf, 256, fileToLoad);//ALPHA
            fgets(buf, 256, fileToLoad);//SIZE
            float skid[11];
            fscanf(fileToLoad, "%s %f %f %f %f %f %f %f %f %f %f %f\n", buf, &skid[0], &skid[1], &skid[2], &skid[3], &skid[4], &skid[5], &skid[6], &skid[7], &skid[8], &skid[9], &skid[10]);
            fgets(buf, 256, fileToLoad);
            fgets(buf, 256, fileToLoad);

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

        fclose(fileToLoad);
    }
    else {assert(false && "No particle file");}
}