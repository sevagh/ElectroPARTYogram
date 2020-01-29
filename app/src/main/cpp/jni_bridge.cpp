#ifndef MODULE_NAME
#define MODULE_NAME  "jni_bridge"
#endif

#include <jni.h>
#include "AudioEngine.h"
#include "logging_macros.h"

const char *TAG = "jni_bridge:: %s";
static AudioEngine *audioEngine = nullptr;

extern "C" {

JNIEXPORT bool JNICALL
Java_xyz_sevag_animals_1as_1meter_AudioEngine_create(JNIEnv *env, jclass) {

    LOGD(TAG, "create(): ");

    if (audioEngine == nullptr) {
        audioEngine = new AudioEngine();
    }

    return (audioEngine != nullptr);
}

JNIEXPORT void JNICALL
Java_xyz_sevag_animals_1as_1meter_AudioEngine_delete(JNIEnv *env, jclass) {

    LOGD(TAG, "delete(): ");

    delete audioEngine;
    audioEngine = nullptr;

}

JNIEXPORT void JNICALL
Java_xyz_sevag_animals_1as_1meter_AudioEngine_startRecording(JNIEnv *env, jclass) {

    LOGD(TAG, "startRecording(): ");

    if (audioEngine == nullptr) {
        LOGE(TAG, "audioEngine is null, you must call create() method before calling this method");
        return;
    }

    audioEngine->startRecording();

}

JNIEXPORT void JNICALL
Java_xyz_sevag_animals_1as_1meter_AudioEngine_stopRecording(JNIEnv *env, jclass) {

    LOGD(TAG, "stopRecording(): ");

    if (audioEngine == nullptr) {
        LOGE(TAG, "audioEngine is null, you must call create() method before calling this method");
        return;
    }

    audioEngine->stopRecording();
}

jobject NewFloat(JNIEnv* env, float value)
{
    jclass floatClass = env->FindClass("java/lang/Float");
    jmethodID floatConstructor = env->GetMethodID(floatClass, "<init>", "(F)V");
    return env->NewObject(floatClass, floatConstructor, static_cast<jfloat>(value));
}

jobject NewBool(JNIEnv* env, bool value)
{
    jclass boolClass = env->FindClass("java/lang/Boolean");
    jmethodID boolConstructor = env->GetMethodID(boolClass, "<init>", "(Z)V");
    return env->NewObject(boolClass, boolConstructor, static_cast<jboolean>(value));
}

JNIEXPORT jobjectArray JNICALL
Java_xyz_sevag_animals_1as_1meter_AudioEngine_GetDrawParams(JNIEnv *env, jclass) {
    //LOGD(TAG, "getBeatResults(): ");

    auto ret = audioEngine->GetDrawParams();
    jobjectArray retobjarr = (jobjectArray)env->NewObjectArray(2, env->FindClass("java/lang/Object"), NULL);
    env->SetObjectArrayElement(retobjarr, 0, NewBool(env, ret.beat));
    env->SetObjectArrayElement(retobjarr, 1, NewFloat(env, ret.tempo));

    return retobjarr;
}

};
