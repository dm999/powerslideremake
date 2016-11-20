
#ifndef RANDOMIZER
#define RANDOMIZER

#include "../includes/CommonIncludes.h"

class Randomizer
{
public:
        static Randomizer& GetInstance()
        {
            static Randomizer rnd;
            return rnd;
        }

        void InitSeed(unsigned long v)
        {
            m_rng.seed(v);
        }

        unsigned int GetRandomInt(unsigned int min, unsigned int max)
        {
            assert(min <= max);
            UNIFORM_INT<unsigned int> rndShift(min, max);
#if defined(_MSC_VER) && _MSC_VER < 1600 || ((defined(__GNUC__) && __GNUC__ == 4) && (defined(__GNUC_MINOR__) && __GNUC_MINOR__ < 7))
            UNIFORM_INT<unsigned int> rndInit(0, 0xFFFFFF);
            RANDOM_ENGINE rng(rndInit(m_rng));
            CommonIncludes::variate_generator<RANDOM_ENGINE, UNIFORM_INT<unsigned int> > genShift(rng, rndShift);
            return genShift();
#else
            return rndShift(m_rng);
#endif
        }

        int GetRandomIntGeometric(double successProbability)
        {
            assert(0 < successProbability && successProbability <= 1);
            GEOMETRIC_DIST<int> rndShift(successProbability);
#if defined(_MSC_VER) && _MSC_VER < 1600 || ((defined(__GNUC__) && __GNUC__ == 4) && (defined(__GNUC_MINOR__) && __GNUC_MINOR__ < 7))
            UNIFORM_INT<unsigned int> rndInit(0, 0xFFFFFF);
            RANDOM_ENGINE rng(rndInit(m_rng));
            CommonIncludes::variate_generator<RANDOM_ENGINE, GEOMETRIC_DIST<int> > genShift(rng, rndShift);
            return genShift();
#else
            return rndShift(m_rng);
#endif
        }

        int GetRandomSign()
        {
            return 1 - 2 * Randomizer::GetInstance().GetRandomInt(0, 1);
        }

        float GetRandomFloat(float min, float max)
        {
            assert(min <= max);
            UNIFORM_REAL<float> rndShift(min, max);
#if defined(_MSC_VER) && _MSC_VER < 1600 || ((defined(__GNUC__) && __GNUC__ == 4) && (defined(__GNUC_MINOR__) && __GNUC_MINOR__ < 7))
            UNIFORM_INT<unsigned int> rndInit(0, 0xFFFFFF);
            RANDOM_ENGINE rng(rndInit(m_rng));
            CommonIncludes::variate_generator<RANDOM_ENGINE, UNIFORM_REAL<float> > genShift(rng, rndShift);
            return genShift();
#else
            return rndShift(m_rng);
#endif
        }

private:
        RANDOM_ENGINE m_rng;

        Randomizer()
        {
#ifdef __MINGW32__
            m_rng.seed(static_cast<unsigned int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
#else
            CommonIncludes::random_device rd;
            m_rng.seed(rd());
#endif
        }

        ~Randomizer()
        {
        }

        Randomizer(const Randomizer&);
        Randomizer& operator=(const Randomizer&);
    };

#endif