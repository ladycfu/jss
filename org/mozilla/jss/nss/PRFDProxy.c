#include <nspr.h>
#include <jni.h>

#include "java_ids.h"
#include "jssutil.h"
#include "PRFDProxy.h"

jobject
JSS_PR_wrapPRFDProxy(JNIEnv *env, PRFileDesc **fd)
{
    jbyteArray pointer = NULL;
    jclass proxyClass;
    jmethodID constructor;
    jobject fdObj = NULL;

    PR_ASSERT(env != NULL && fd != NULL && *fd != NULL);

    /* convert pointer to byte array */
    pointer = JSS_ptrToByteArray(env, *fd);

    /* Lookup the class and constructor */
    proxyClass = (*env)->FindClass(env, PRFD_PROXY_CLASS_NAME);
    if(proxyClass == NULL) {
        ASSERT_OUTOFMEM(env);
        goto finish;
    }
    constructor = (*env)->GetMethodID(env, proxyClass,
                            PLAIN_CONSTRUCTOR,
                            PRFD_PROXY_CONSTRUCTOR_SIG);
    if(constructor == NULL) {
        ASSERT_OUTOFMEM(env);
        goto finish;
    }

    /* call the constructor */
    fdObj = (*env)->NewObject(env, proxyClass, constructor, pointer);

finish:
    if (fdObj == NULL && *fd != NULL) {
        /* didn't work, so free resources */
        PR_Close(*fd);
    }

    *fd = NULL;

    PR_ASSERT(fdObj || (*env)->ExceptionOccurred(env));
    return fdObj;
}

PRStatus
JSS_PR_getPRFileDesc(JNIEnv *env, jobject prfd_proxy, PRFileDesc **fd)
{
    return JSS_getPtrFromProxy(env, prfd_proxy, (void**)fd);
}
