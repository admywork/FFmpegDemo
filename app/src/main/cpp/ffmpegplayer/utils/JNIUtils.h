#ifndef MG_JNI_UTILS_H
#define MG_JNI_UTILS_H

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C"
{
#endif

enum JniFieldType {
    JNI_CLASS,
    JNI_FIELD,
    JNI_STATIC_FIELD,
    JNI_METHOD,
    JNI_STATIC_METHOD
};

struct JniField {
    const char *name;
    const char *method;
    const char *signature;
    enum JniFieldType type;
    int offset;
    int mandatory;
};

void jni_load_custom_class(JNIEnv *env);
void jni_unload_custom_class(JNIEnv *env);

int jni_set_java_vm(void *vm);
void *jni_get_java_vm();

JNIEnv *jni_get_env();
char *jni_jstring_to_utf_chars(JNIEnv *env, jstring string);
jstring jni_utf_chars_to_jstring(JNIEnv *env, const char *utf_chars);
int jni_exception_get_summary(JNIEnv *env, jthrowable exception, char **error);
int jni_exception_check(JNIEnv *env, int log);

int jni_init_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global);
int jni_reset_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global);

#define JNI_GET_ENV_OR_RETURN(env, ret) do {                       \
    (env) = jni_get_env();                                         \
    if (!(env)) {                                                  \
        return ret;                                                \
    }                                                              \
} while (0)

#define JNI_GET_ENV_OR_RETURN_VOID(env) do {                       \
    (env) = jni_get_env();                                         \
    if (!(env)) {                                                  \
        return;                                                    \
    }                                                              \
} while (0)

#ifdef __cplusplus
}
#endif

#endif //MG_JNI_UTILS_H
