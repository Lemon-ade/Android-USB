LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := ndk-control
LOCAL_SRC_FILES := NdkCapture.c
include $(BUILD_SHARED_LIBRARY)
