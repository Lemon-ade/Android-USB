/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class app_android_ltouchpad_UsbipDevice */

#ifndef _Included_app_android_ltouchpad_UsbipDevice
#define _Included_app_android_ltouchpad_UsbipDevice
#ifdef __cplusplus
extern "C" {
#endif
#undef app_android_ltouchpad_UsbipDevice_MAX_CLIENT
#define app_android_ltouchpad_UsbipDevice_MAX_CLIENT 5L
#undef app_android_ltouchpad_UsbipDevice_ERRCODE_USBIPD
#define app_android_ltouchpad_UsbipDevice_ERRCODE_USBIPD 1L
#undef app_android_ltouchpad_UsbipDevice_ERRCODE_BINDD
#define app_android_ltouchpad_UsbipDevice_ERRCODE_BINDD 2L
#undef app_android_ltouchpad_UsbipDevice_ERRCODE_USBIPCONNECT
#define app_android_ltouchpad_UsbipDevice_ERRCODE_USBIPCONNECT 3L
#undef app_android_ltouchpad_UsbipDevice_ERRCODE_DISCONNECT
#define app_android_ltouchpad_UsbipDevice_ERRCODE_DISCONNECT 4L
/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    server_bind
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_server_1bind
  (JNIEnv *, jobject, jint);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    listen_client
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_listen_1client
  (JNIEnv *, jobject);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    recv_sock
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1sock
  (JNIEnv *, jobject, jint);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    connect_usbip
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_connect_1usbip
  (JNIEnv *, jobject);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    recv_enum
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1enum
  (JNIEnv *, jobject);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    send_enum
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_send_1enum
  (JNIEnv *, jobject, jint);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    is_sendable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_is_1sendable
  (JNIEnv *, jobject);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    recv_ack
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1ack
  (JNIEnv *, jobject);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    send_control
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_send_1control
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     app_android_ltouchpad_UsbipDevice
 * Method:    get_message
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_app_android_ltouchpad_UsbipDevice_get_1message
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
