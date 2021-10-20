#include "JNIUtils.h"
#include "ALog.h"
#include <pthread.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define LOG_TAG "JNIUtils"

static JavaVM *java_vm = nullptr;
static pthread_key_t current_env;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static const char *custom_class_names[] = {
        "com/niklaus/player/FFmpegPlayer",
};

static int custom_class_num = 0;
static jclass *loaded_custom_classes = nullptr;

static bool jni_is_custom_class(const char *name) {
    for (int i = 0; i < custom_class_num; i++) {
        if (!strcasecmp(name, custom_class_names[i])) {
            return true;
        }
    }

    return false;
}

static jclass jni_get_custom_class(const char *name) {
    for (int i = 0; i < custom_class_num; i++) {
        if (!strcasecmp(name, custom_class_names[i])) {
            return loaded_custom_classes[i];
        }
    }

    return NULL;
}

void jni_load_custom_class(JNIEnv *env)
{
    custom_class_num = sizeof(custom_class_names) / sizeof(custom_class_names[0]);

    loaded_custom_classes = (jclass *)malloc(sizeof(jclass) * custom_class_num);
    memset(loaded_custom_classes, 0, sizeof(jclass *) * custom_class_num);

    for (int i = 0; i < custom_class_num; i++) {
        jclass clazz = env->FindClass(custom_class_names[i]);
        loaded_custom_classes[i] = (jclass)env->NewGlobalRef(clazz);
        env->DeleteLocalRef(clazz);
    }
}

void jni_unload_custom_class(JNIEnv *env)
{
    for (int i = 0; i < custom_class_num; i++) {
        env->DeleteGlobalRef(loaded_custom_classes[i]);
        loaded_custom_classes[i] = NULL;
    }

    free(loaded_custom_classes);
    loaded_custom_classes= NULL;
    custom_class_num = 0;
}

int jni_set_java_vm(void *vm)
{
    int ret = 0;

    pthread_mutex_lock(&lock);
    if (java_vm == NULL) {
        java_vm = (JavaVM *)vm;
    } else if (java_vm != vm) {
        ret = -1;
        LOGD(LOG_TAG, "A Java virtual machine has already been set\n");
    }
    pthread_mutex_unlock(&lock);

    return ret;
}

void *jni_get_java_vm()
{
    void *vm;

    pthread_mutex_lock(&lock);
    vm = java_vm;
    pthread_mutex_unlock(&lock);

    return vm;
}

static void jni_detach_env(void *data)
{
    if (java_vm) {
        java_vm->DetachCurrentThread();
        pthread_setspecific(current_env, NULL);
    }
}

static void jni_create_pthread_key(void)
{
    pthread_key_create(&current_env, jni_detach_env);
}

JNIEnv *jni_get_env()
{
    int ret = 0;
    JNIEnv *env = NULL;
    pthread_mutex_lock(&lock);
    if (!java_vm) {
        LOGE(LOG_TAG, "No Java virtual machine has been registered\n");
        goto done;
    }
    pthread_once(&once, jni_create_pthread_key);
    if ((env = (JNIEnv *)pthread_getspecific(current_env)) != NULL) {
        goto done;
    }

    ret = java_vm->GetEnv((void **)&env, JNI_VERSION_1_6);

    switch(ret) {
        case JNI_EDETACHED:
            if (java_vm->AttachCurrentThread(&env, NULL) != 0) {
                LOGE(LOG_TAG, "Failed to attach the JNI environment to the current thread\n");
                env = NULL;
            } else {
                pthread_setspecific(current_env, env);
            }
            break;
        case JNI_OK:
            //nativelog(LOG_LEVEL_D, LOG_TAG, "[%s:%d] JNI_OK \n", __FUNCTION__, __LINE__);
            break;
        case JNI_EVERSION:
            LOGE(LOG_TAG, "The specified JNI version is not supported\n");
            break;
        default:
            LOGE(LOG_TAG, "Failed to get the JNI environment attached to this thread\n");
            break;
    }

    done:
    pthread_mutex_unlock(&lock);
    return env;
}

char *jni_jstring_to_utf_chars(JNIEnv *env, jstring string)
{
    char *ret = NULL;
    const char *utf_chars = NULL;

    jboolean copy = 0;

    if (!string) {
        return NULL;
    }

    utf_chars = env->GetStringUTFChars(string, &copy);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "String.getStringUTFChars() threw an exception\n");
        return NULL;
    }

    ret = strdup(utf_chars);

    env->ReleaseStringUTFChars(string, utf_chars);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "String.releaseStringUTFChars() threw an exception\n");
        return NULL;
    }

    return ret;
}

jstring jni_utf_chars_to_jstring(JNIEnv *env, const char *utf_chars)
{
    jstring ret;

    ret = env->NewStringUTF(utf_chars);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "NewStringUTF() threw an exception\n");
        return NULL;
    }

    return ret;
}

int jni_exception_get_summary(JNIEnv *env, jthrowable exception, char **error)
{
    int ret = 0;

    int bufsize = 1024;
    char *buf = NULL;

    char *name = NULL;
    char *message = NULL;

    jclass class_class = NULL;
    jmethodID get_name_id = NULL;

    jclass exception_class = NULL;
    jmethodID get_message_id = NULL;

    jstring string = NULL;

    buf = (char *)malloc(bufsize * sizeof(char));
    memset(buf, 0, bufsize * sizeof(char));

    exception_class = env->GetObjectClass(exception);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Could not find Throwable class\n");
        ret = -1;
        goto done;
    }

    class_class = env->GetObjectClass(exception_class);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Could not find Throwable class's class\n");
        ret = -1;
        goto done;
    }

    get_name_id = env->GetMethodID(class_class, "getName", "()Ljava/lang/String;");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Could not find method Class.getName()\n");
        ret = -1;
        goto done;
    }

    string = (jstring)env->CallObjectMethod(exception_class, get_name_id);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Class.getName() threw an exception\n");
        ret = -1;
        goto done;
    }

    if (string) {
        name = jni_jstring_to_utf_chars(env, string);
        env->DeleteLocalRef(string);
        string = NULL;
    }

    get_message_id = env->GetMethodID(exception_class, "getMessage", "()Ljava/lang/String;");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Could not find method java/lang/Throwable.getMessage()\n");
        ret = -1;
        goto done;
    }

    string = (jstring)env->CallObjectMethod(exception, get_message_id);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE(LOG_TAG, "Throwable.getMessage() threw an exception\n");
        ret = -1;
        goto done;
    }

    if (string) {
        message = jni_jstring_to_utf_chars(env, string);
        env->DeleteLocalRef(string);
        string = NULL;
    }

    if (name && message) {
        sprintf(buf, "%s: %s", name, message);
    } else if (name && !message) {
        sprintf(buf, "%s occurred", name);
    } else if (!name && message) {
        sprintf(buf, "Exception: %s", message);
    } else {
        LOGD(LOG_TAG, "Could not retrieve exception name and message\n");
        sprintf(buf, "Exception occurred");
    }

    *error = buf;
    done:

    free(name);
    free(message);

    if (class_class) {
        env->DeleteLocalRef(class_class);
    }

    if (exception_class) {
        env->DeleteLocalRef(exception_class);
    }

    if (string) {
        env->DeleteLocalRef(string);
    }

    return ret;
}

int jni_exception_check(JNIEnv *env, int log)
{
    int ret;

    jthrowable exception;

    char *message = NULL;

    if (!env->ExceptionCheck()) {
        return 0;
    }

    if (!log) {
        env->ExceptionClear();
        return -1;
    }

    exception = env->ExceptionOccurred();
    env->ExceptionClear();

    if ((ret = jni_exception_get_summary(env, exception, &message)) < 0) {
        env->DeleteLocalRef(exception);
        return ret;
    }

    env->DeleteLocalRef(exception);

    LOGE(LOG_TAG, "%s\n", message);
    free(message);

    return -1;
}

int jni_init_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global)
{
    int i, ret = 0;
    jclass last_clazz = NULL;

    for (i = 0; jfields_mapping[i].name; i++) {
        int mandatory = jfields_mapping[i].mandatory;
        enum JniFieldType type = jfields_mapping[i].type;

        if (type == JNI_CLASS) {
            jclass clazz;

            last_clazz = NULL;

            if (jni_is_custom_class(jfields_mapping[i].name)) {
                clazz = jni_get_custom_class(jfields_mapping[i].name);
            } else {
                clazz = env->FindClass(jfields_mapping[i].name);
            }

            if ((ret = jni_exception_check(env, mandatory)) < 0 && mandatory) {
                goto done;
            }

            last_clazz = *(jclass*)((uint8_t*)jfields + jfields_mapping[i].offset) =
                    (global && !jni_is_custom_class(jfields_mapping[i].name)) ? (jclass)env->NewGlobalRef(clazz) : clazz;

            if (global && !jni_is_custom_class(jfields_mapping[i].name)) {
                env->DeleteLocalRef(clazz);
            }

        } else {

            if (!last_clazz) {
                ret = -1;
                break;
            }

            switch(type) {
                case JNI_FIELD: {
                    jfieldID field_id = env->GetFieldID(last_clazz, jfields_mapping[i].method, jfields_mapping[i].signature);
                    if ((ret = jni_exception_check(env, mandatory)) < 0 && mandatory) {
                        goto done;
                    }

                    *(jfieldID*)((uint8_t*)jfields + jfields_mapping[i].offset) = field_id;
                    break;
                }
                case JNI_STATIC_FIELD: {
                    jfieldID field_id = env->GetStaticFieldID(last_clazz, jfields_mapping[i].method, jfields_mapping[i].signature);
                    if ((ret = jni_exception_check(env, mandatory)) < 0 && mandatory) {
                        goto done;
                    }

                    *(jfieldID*)((uint8_t*)jfields + jfields_mapping[i].offset) = field_id;
                    break;
                }
                case JNI_METHOD: {
                    jmethodID method_id = env->GetMethodID(last_clazz, jfields_mapping[i].method, jfields_mapping[i].signature);
                    if ((ret = jni_exception_check(env, mandatory)) < 0 && mandatory) {
                        goto done;
                    }

                    *(jmethodID*)((uint8_t*)jfields + jfields_mapping[i].offset) = method_id;
                    break;
                }
                case JNI_STATIC_METHOD: {
                    jmethodID method_id = env->GetStaticMethodID(last_clazz, jfields_mapping[i].method, jfields_mapping[i].signature);
                    if ((ret = jni_exception_check(env, mandatory)) < 0 && mandatory) {
                        goto done;
                    }

                    *(jmethodID*)((uint8_t*)jfields + jfields_mapping[i].offset) = method_id;
                    break;
                }
                default:
                    LOGE(LOG_TAG, "Unknown JNI field type\n");
                    ret = -1;
                    goto done;
            }

            ret = 0;
        }
    }

    done:
    if (ret < 0) {
        /* reset jfields in case of failure so it does not leak references */
        jni_reset_jfields(env, jfields, jfields_mapping, global);
    }

    return ret;
}

int jni_reset_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global)
{
    int i;

    for (i = 0; jfields_mapping[i].name; i++) {
        enum JniFieldType type = jfields_mapping[i].type;

        switch(type) {
            case JNI_CLASS: {
                jclass clazz = *(jclass*)((uint8_t*)jfields + jfields_mapping[i].offset);
                if (!clazz)
                    continue;

                if (!jni_is_custom_class(jfields_mapping[i].name)) {
                    if (global) {
                        env->DeleteGlobalRef(clazz);
                    } else {
                        env->DeleteLocalRef(clazz);
                    }
                }

                *(jclass*)((uint8_t*)jfields + jfields_mapping[i].offset) = NULL;
                break;
            }
            case JNI_FIELD: {
                *(jfieldID*)((uint8_t*)jfields + jfields_mapping[i].offset) = NULL;
                break;
            }
            case JNI_STATIC_FIELD: {
                *(jfieldID*)((uint8_t*)jfields + jfields_mapping[i].offset) = NULL;
                break;
            }
            case JNI_METHOD: {
                *(jmethodID*)((uint8_t*)jfields + jfields_mapping[i].offset) = NULL;
                break;
            }
            case JNI_STATIC_METHOD: {
                *(jmethodID*)((uint8_t*)jfields + jfields_mapping[i].offset) = NULL;
                break;
            }
            default:
                LOGE(LOG_TAG, "Unknown JNI field type\n");
        }
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
