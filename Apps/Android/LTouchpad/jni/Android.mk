LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := usb-device
LOCAL_SRC_FILES := UsbipDevice.c
include $(BUILD_SHARED_LIBRARY)
