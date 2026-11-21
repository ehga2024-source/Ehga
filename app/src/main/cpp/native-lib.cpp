#include <jni.h>
#include <string>
#include "arranger_engine.h"

namespace {
ArrangerEngine& engine() {
    static ArrangerEngine instance;
    return instance;
}
} // namespace

extern "C" JNIEXPORT jboolean JNICALL
Java_com_ehg_arranger_NativeBridge_initAudio(JNIEnv*, jobject, jint sampleRate) {
    return engine().initAudio(sampleRate) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_ehg_arranger_NativeBridge_loadSoundFont(JNIEnv* env, jobject, jstring jPath) {
    if (jPath == nullptr) return JNI_FALSE;
    const char* chars = env->GetStringUTFChars(jPath, nullptr);
    std::string path(chars ? chars : "");
    if (chars) env->ReleaseStringUTFChars(jPath, chars);
    return engine().loadSoundFontFile(path) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_ehg_arranger_NativeBridge_loadStyle(JNIEnv* env, jobject, jstring jPath) {
    if (jPath == nullptr) return JNI_FALSE;
    const char* chars = env->GetStringUTFChars(jPath, nullptr);
    std::string path(chars ? chars : "");
    if (chars) env->ReleaseStringUTFChars(jPath, chars);
    return engine().loadStyleFile(path) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_ehg_arranger_NativeBridge_getPresetCount(JNIEnv*, jobject) {
    return engine().getPresetCount();
}

extern "C" JNIEXPORT void JNICALL
Java_com_ehg_arranger_NativeBridge_noteOn(JNIEnv*, jobject, jint presetIdx, jint note, jfloat vel) {
    engine().noteOn(presetIdx, note, vel);
}

extern "C" JNIEXPORT void JNICALL
Java_com_ehg_arranger_NativeBridge_noteOff(JNIEnv*, jobject, jint presetIdx, jint note) {
    engine().noteOff(presetIdx, note);
}
