//-----------------------------------------------------------------------------
// ParaEngineActivity.h
// Authors: LanZhihong, big
// CreateDate: 2019.12.30
// ModifyDate: 2022.1.12
//-----------------------------------------------------------------------------

#include "ParaEngineActivity.h"
#include "AppDelegate.h"
#include <jni.h>

namespace ParaEngine {
    const std::string ParaEngineActivity::classname = "com/tatfook/paracraft/ParaEngineActivity";

    std::string ParaEngineActivity::getLauncherIntentData()
    {
        return JniHelper::callStaticStringMethod(classname, "getLauncherIntentData");
    }

    void ParaEngineActivity::setScreenOrientation(int type)
    {
        JniHelper::callStaticVoidMethod(classname, "setScreenOrientation", type);
    }
}

extern "C" {
    JNIEXPORT jintArray JNICALL Java_com_tatfook_paracraft_ParaEngineActivity_getGLContextAttrs(JNIEnv* env,
                                                                                                jclass clazz)
    {
        ParaEngine::GLContextAttrs attrs = {8, 8, 8, 8, 24, 8, 0 };

        int temp[] =  { attrs.redBits,
                        attrs.greenBits,
                        attrs.blueBits,
                        attrs.alphaBits,
                        attrs.depthBits,
                        attrs.stencilBits,
                        attrs.multisamplingCount };

        jintArray glContextAttrsJava = env->NewIntArray(7);
        env->SetIntArrayRegion(glContextAttrsJava, 0, 7, temp);

        return glContextAttrsJava;
    }
}