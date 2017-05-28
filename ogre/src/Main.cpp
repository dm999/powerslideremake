#include "pcheader.h"

#include "BaseApp.h"


//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Ogre+Application
//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Simple+Win32+Program
//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=StaticLinking
//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Tutorial+6&structure=Tutorials
//http://www.ogre3d.org/tikiwiki/Depth+Shadow+Mapping
//http://www.ogre3d.org/docs/manual/manual_23.html
//http://www.ogre3d.org/docs/manual/manual_14.html

#if defined(__ANDROID__)
    #include <jni.h>
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 

    BaseApp * base = NULL;
    bool gInit = false;

    extern "C" 
    {
        JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) 
        {
            base = new BaseApp();
            base->gVM = vm;
            return JNI_VERSION_1_4;
        }

        JNIEXPORT void JNICALL 	Java_org_ogre3d_android_OgreActivityJNI_create(JNIEnv * env, jobject obj, jobject assetManager)
        {
            try
            {
                if(!gInit)
                {
                    base->androidCreate(env, obj, assetManager);
                    gInit = true;
                }
            }
            catch( Ogre::Exception& e )
            {
                std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
                LOGE("An exception has occured: %s", e.getFullDescription().c_str());
            }
            catch(const std::exception& e)
            {
                std::cerr << "An exception has occured: " << e.what() << std::endl;
                LOGE("An exception has occured: %s", e.what());
            }
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_destroy(JNIEnv * env, jobject obj)
        {
            if(gInit)
            {
                base->androidDestroy();

                if(base)
                {
                    delete base;
                    base = NULL;
                    gInit = false;
                }
            }
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_initWindow(JNIEnv * env, jobject obj,  jobject surface)
        {
            if(surface)
            {
                try
                {
                    base->androidInitWindow(env, obj, surface);
                }
                catch( Ogre::Exception& e )
                {
                    std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
                    LOGE("An exception has occured: %s", e.getFullDescription().c_str());
                }
                catch(const std::exception& e)
                {
                    std::cerr << "An exception has occured: " << e.what() << std::endl;
                    LOGE("An exception has occured: %s", e.what());
                }
            }
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_termWindow(JNIEnv * env, jobject obj)
        {
            try
            {
                base->androidTerminateWindow();
            }
            catch( Ogre::Exception& e )
            {
                std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
            }
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_renderOneFrame(JNIEnv * env, jobject obj)
        {
            base->androidRenderOneFrame(env);
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_pause(JNIEnv * env, jobject obj)
        {
            base->androidPause(env);
        }

        //http://www.ogre3d.org/forums/viewtopic.php?f=21&t=80571
        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_handleActionDown(JNIEnv* env, jobject obj, jint pID, jfloat pX, jfloat pY)
        {
            base->touchDown(pID, pX, pY);
        }

        JNIEXPORT void JNICALL Java_org_ogre3d_android_OgreActivityJNI_handleActionUp(JNIEnv* env, jobject obj, jint pID, jfloat pX, jfloat pY)
        {
            base->touchUp(pID, pX, pY);
        }
    }
#else

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
{
    
    BaseApp base;

    try
    {
        base.go();
    }catch( Ogre::Exception& e )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}
#endif





