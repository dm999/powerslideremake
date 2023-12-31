#ifndef COMMONINCLUDES_H
#define COMMONINCLUDES_H

//http://stackoverflow.com/questions/70013/how-to-detect-if-im-compiling-code-with-visual-studio-2008
#ifdef _MSC_VER 
    #if (_MSC_VER > 1500)   // vc2010
        #include <memory>
        #include <random>
        #include <thread>
    #endif
    #if (_MSC_VER == 1500)  // vc2008
        #include <memory>
        #include <random>
        #include <windows.h>
    #endif
#else
    #include <memory>
    #include <random>
    #include <chrono>
    #include <thread>
#endif

namespace CommonIncludes
{
#ifdef _MSC_VER 
    #if (_MSC_VER > 1500)
        using std::shared_ptr;
        using std::random_device;
        using std::mt19937;
        #define UNIFORM_INT std::uniform_int_distribution
        #define UNIFORM_REAL std::uniform_real_distribution
        #define GEOMETRIC_DIST std::geometric_distribution
        #define RANDOM_ENGINE std::default_random_engine
        #define SLEEP(milliseconds) std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds))
    #endif
    #if (_MSC_VER <= 1500)
    #error newer version needed
    #endif
#else
    using std::shared_ptr;
    using std::random_device;
    using std::mt19937;
    #define UNIFORM_INT std::uniform_int_distribution
    #define UNIFORM_REAL std::uniform_real_distribution
    #define GEOMETRIC_DIST std::geometric_distribution
    #define RANDOM_ENGINE std::default_random_engine
    #define SLEEP(milliseconds) std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds))
#endif
}

#define TEMP_RESOURCE_GROUP_NAME "tempRes"

#endif