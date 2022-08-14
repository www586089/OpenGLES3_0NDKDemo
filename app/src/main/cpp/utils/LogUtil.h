//
// Created by Thinkpad on 2022/7/9.
//

#ifndef STUDIOTEST_LOGUTIL_H
#define STUDIOTEST_LOGUTIL_H

#include <android/log.h>
#define LOG_TAG "zfangJNI"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARIN, LOG_TAG, __VA_ARGS__)

#endif //STUDIOTEST_LOGUTIL_H
