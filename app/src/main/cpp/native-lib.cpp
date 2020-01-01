#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_xyz_sevag_animals_1as_1meter_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
