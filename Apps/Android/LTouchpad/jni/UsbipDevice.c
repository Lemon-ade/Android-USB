#include "UsbipDevice.h"
#include "UsbipMouse.c"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/usbdevice_fs.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define PORT_Device 3700
#define PORT_App 3650
#define MAXLINE 1024
#define MAX_RECV 20
#define MAX_SEND 1024
#define CMD_MAX 7
#define CMD_MAX_RECV 5
#define SIZE sizeof(struct sockaddr_in)
#define SIZE_USBDEVFS sizeof(struct usbdevfs_urb)
#define SIZE_ISOPACKETDESC sizeof(struct usbdevfs_iso_packet_desc)

int sockfd = -1;
struct usbdevfs_urb urb;
unsigned int seqnum = 0;
unsigned int re_seqnum = 0;

struct sockaddr_in sv_addr;
int sockfd_listen=-1;

char message[MAXLINE];

int recv_seqnum(int sockfd, char* cmd, int cmd_size);
int send_urb(struct usbdevfs_urb *surb, unsigned char *buffer);
int send_cmd(int cmd_num);
void set_urb_control();
int send_control() ;


JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_server_1bind
  (JNIEnv *env, jobject obj, jint max_client) {

	if((sockfd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		// listen socket make error
		return -1;
	}
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sv_addr.sin_port = htons(PORT_App);
	if(bind(sockfd_listen, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) == -1)
	{
		// bind failed
		return -2;
	}
	if(listen(sockfd_listen, max_client) == -1)
	{
		// listen error
		return -3;
	}
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_listen_1client
  (JNIEnv *env, jobject obj) {

	int sockfd=-1;
	if((sockfd = accept(sockfd_listen, NULL, NULL)) ==-1)
	{
		// accept error
		return -1;
	}

	return sockfd;
}

JNIEXPORT jstring JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1sock
  (JNIEnv *env, jobject obj, jint sockfd) {
	  char content_recv[MAXLINE];
	  int i;
	  jstring str;

	  memset(content_recv, 0, MAXLINE);
	  i=recv(sockfd, content_recv, MAXLINE, 0);
	  if(i<=0)
		  return NULL;

	  str = (*env)->NewStringUTF(env, content_recv);
	  return str;
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_connect_1usbip
(JNIEnv *env, jobject obj){
	struct sockaddr_in sockin;
	int con = -1;

	if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) == -1) {
		fprintf(stderr, "socket() fail\n");
		return -1;
	}
	memset(&sockin, 0, sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockin.sin_port = htons(PORT_Device);

	con = connect(sockfd,(struct sockaddr*)&sockin, sizeof(sockin));
	if(con==-1) {
		fprintf(stderr, "connect() fail\n");
		return -2;
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1enum
  (JNIEnv *env, jobject obj) {
	char cmd[MAX_RECV];
	int cmd_num;

	memset(cmd, 0, MAX_RECV);
	if(recv_seqnum(sockfd, cmd, MAX_RECV)==-1){
		fprintf(stderr, "recv_seqnum() fail\n");
		return -1;
	}
	sscanf(cmd, "%d", &cmd_num);
	return cmd_num;
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_send_1enum
  (JNIEnv *env, jobject obj, jint desc) {
	if(desc>CMD_MAX)
		return -1;
	else if(desc==CMD_MAX) {
		set_urb_control();
		re_seqnum = desc;
		seqnum = desc;
		return 0;
	}
	else {
		return send_cmd(desc);
	}
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_is_1sendable
  (JNIEnv *evn, jobject obj) {
	  if(seqnum <= re_seqnum+1)
		  return 1;
	  else
		  return 0;
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_recv_1ack
(JNIEnv *env, jobject obj) {
	char cmd[MAX_RECV];

	if(recv_seqnum(sockfd, cmd, MAX_RECV)==-1){
		fprintf(stderr, "recv_seqnum() fail\n");
		return -1;
	}
	sscanf(cmd, "%d", &re_seqnum);
	return re_seqnum;
}

JNIEXPORT jint JNICALL Java_app_android_ltouchpad_UsbipDevice_send_1control
  (JNIEnv *env, jobject obj, jbyteArray jbuffer) {
	jsize n = (*env)->GetArrayLength(env, jbuffer);
	jbyte *buffer = (*env)->GetByteArrayElements(env, jbuffer, 0);
	(*env)->ReleaseByteArrayElements(env, jbuffer, buffer, 0);
	return send_control(buffer);
}

int recv_seqnum(int sockfd, char* cmd, int cmd_size) 
{
	int ret;

	memset(cmd, 0, cmd_size);
	ret = recv(sockfd, cmd, cmd_size, 0);
	if(ret<=0) {
		fprintf(stderr, "recv() error\n");
		return -1;
	}
	return ret;
}

int send_urb(struct usbdevfs_urb *surb, unsigned char *buffer)
{
	int ret, total = 0;
	ret = send(sockfd, surb, SIZE_USBDEVFS, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb) error\n");
		return -1;
	}
	total += ret;
	printf(" < send >\n");
	printf("  buffer[0] : %02x\n", buffer[0]);
	printf("  buffer[1] : %02x\n", buffer[1]);
	printf("  buffer[2] : %02x\n", buffer[2]);
	printf("  buffer[3] : %02x\n", buffer[3]);
	ret = send(sockfd, buffer, surb->buffer_length, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb.buffer) error\n");
		return -1;
	}
	total += ret;
	ret = send(sockfd, surb->iso_frame_desc, SIZE_ISOPACKETDESC, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb.iso_frame_desc[0]) error\n");
		return -1;
	}
	total += ret;
	return total;
}

int send_cmd(int cmd_num)
{
	unsigned char buffer[MAX_SEND];

	switch(cmd_num) {
	case 1 : 
		urb.type=2;
		urb.endpoint=0x80;
		urb.status=0;
		urb.flags=0x00000000;
        buffer[0]=0x80;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x01;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x12;
        buffer[7]=0x00;
        //memcpy(buffer+8, &device_desc, sizeof(device_desc));
        ///*
        buffer[8]=0x12;
                buffer[9]=0x01;
                buffer[10]=0x00;
                buffer[11]=0x02;
                buffer[12]=0x00;
                buffer[13]=0x00;
                buffer[14]=0x00;
                buffer[15]=0x08;
                buffer[16]=0x61;
                buffer[17]=0x04;
                buffer[18]=0x16;
                buffer[19]=0x4d;
                buffer[20]=0x00;
                buffer[21]=0x02;
                buffer[22]=0x00;
                buffer[23]=0x02;
                buffer[24]=0x00;
                buffer[25]=0x01;
                //*/
		urb.buffer_length=26;
		urb.actual_length=18;
		urb.start_frame=0;
		urb.number_of_packets=0;
		urb.error_count=0;
		urb.signr=0;
		urb.usercontext = 0x00;
		urb.iso_frame_desc[0].length = 0x01000680;
		urb.iso_frame_desc[0].actual_length = 0x00120000;
		urb.iso_frame_desc[0].status = 0x02000112;
		break;
	case 2 :
		urb.type=2;
		urb.endpoint=0x80;
		urb.status=0;
		urb.flags=0x00000000;
     	buffer[0]=0x80;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x02;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x09;
        buffer[7]=0x00;
        //memcpy(buffer+8, &config_desc, sizeof(config_desc));
        ///*
        buffer[8]=0x09;
                buffer[9]=0x02;
                buffer[10]=0x22;
                buffer[11]=0x00;
                buffer[12]=0x01;
                buffer[13]=0x01;
                buffer[14]=0x00;
                buffer[15]=0xa0;
                buffer[16]=0x32;
                //*/
		urb.buffer_length=17;
		urb.actual_length=9;
		urb.start_frame=0;
		urb.number_of_packets=0;
		urb.error_count=0;
		urb.signr=0;
		urb.usercontext = 0x00;
		urb.iso_frame_desc[0].length = 0x02000680;
		urb.iso_frame_desc[0].actual_length = 0x00090000;
		urb.iso_frame_desc[0].status = 0x00220209;
		break;
	case 3 : 
		urb.type=2;
		urb.endpoint=0x80;
		urb.status=0;
		urb.flags=0x00000000;
		buffer[0]=0x80;
       buffer[1]=0x06;
       buffer[2]=0x00;
       buffer[3]=0x02;
       buffer[4]=0x00;
       buffer[5]=0x00;
       buffer[6]=0x22;
       buffer[7]=0x00;
       /*
       memcpy(buffer+8, &config_desc, sizeof(config_desc));
       memcpy(buffer+17, &intf_desc, sizeof(intf_desc));
       memcpy(buffer+26, &hid_desc, sizeof(hid_desc));
       memcpy(buffer+35, &endpoint_desc, sizeof(endpoint_desc));
       */
       ///*
       buffer[8]=0x09;
               buffer[9]=0x02;
               buffer[10]=0x22;
               buffer[11]=0x00;
               buffer[12]=0x01;
               buffer[13]=0x01;
               buffer[14]=0x00;
               buffer[15]=0xa0;
               buffer[16]=0x32;
               buffer[17]=0x09;
               buffer[18]=0x04;
               buffer[19]=0x00;
               buffer[20]=0x00;
               buffer[21]=0x01;
               buffer[22]=0x03;
               buffer[23]=0x01;
               buffer[24]=0x02;
               buffer[25]=0x00;
               buffer[26]=0x09;
               buffer[27]=0x21;
               buffer[28]=0x11;
               buffer[29]=0x01;
               buffer[30]=0x00;
               buffer[31]=0x01;
               buffer[32]=0x22;
               buffer[33]=0x34;
               buffer[34]=0x00;
               buffer[35]=0x07;
               buffer[36]=0x05;
               buffer[37]=0x81;
               buffer[38]=0x03;
               buffer[39]=0x04;
               buffer[40]=0x00;
               buffer[41]=0x0a;
               //*/
		urb.buffer_length=42;
		urb.actual_length=34;
		urb.start_frame=0;
		urb.number_of_packets=0;
		urb.error_count=0;
		urb.signr=0;
		urb.usercontext = 0x00;
		urb.iso_frame_desc[0].length = 0x02000680;
		urb.iso_frame_desc[0].actual_length = 0x00220000;
		urb.iso_frame_desc[0].status = 0x00220209;
		break;
	case 4 : 
		urb.type=2;
		urb.endpoint=0x00;
		urb.status=0;
		urb.flags=0x00000000;
		buffer[0]=0x21;
        buffer[1]=0x0a;
        buffer[2]=0x00;
        buffer[3]=0x00;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x00;
        buffer[7]=0x00;
		urb.buffer_length=8;
		urb.actual_length=0;
		urb.start_frame=0;
		urb.number_of_packets=0;
		urb.error_count=0;
		urb.signr=0;
		urb.usercontext = 0x00;
		urb.iso_frame_desc[0].length = 0x00000a21;
		urb.iso_frame_desc[0].actual_length = 0x00000000;
		urb.iso_frame_desc[0].status = 0x0001d881;
		break;
	case 5 : 
		urb.type=2;
		urb.endpoint=0x80;
		urb.status=0;
		urb.flags=0x00000000;
		buffer[0]=0x81;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x22;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x74;
        buffer[7]=0x00;
        buffer[8]=0x05;
        buffer[9]=0x01;
        buffer[10]=0x09;
        buffer[11]=0x02;
        buffer[12]=0xa1;
        buffer[13]=0x01;
        buffer[14]=0x09;
        buffer[15]=0x01;
        buffer[16]=0xa1;
        buffer[17]=0x00;
        buffer[18]=0x05;
        buffer[19]=0x09;
        buffer[20]=0x19;
        buffer[21]=0x01;
        buffer[22]=0x29;
        buffer[23]=0x03;
        buffer[24]=0x15;
        buffer[25]=0x00;
        buffer[26]=0x25;
        buffer[27]=0x01;
        buffer[28]=0x75;
        buffer[29]=0x01;
        buffer[30]=0x95;
        buffer[31]=0x03;
        buffer[32]=0x81;
        buffer[33]=0x02;
        buffer[34]=0x75;
        buffer[35]=0x05;
        buffer[36]=0x95;
        buffer[37]=0x01;
        buffer[38]=0x81;
        buffer[39]=0x01;
        buffer[40]=0x05;
        buffer[41]=0x01;
        buffer[42]=0x09;
        buffer[43]=0x30;
        buffer[44]=0x09;
        buffer[45]=0x31;
        buffer[46]=0x09;
        buffer[47]=0x38;
        buffer[48]=0x15;
        buffer[49]=0x81;
        buffer[50]=0x25;
        buffer[51]=0x7f;
        buffer[52]=0x75;
        buffer[53]=0x08;
        buffer[54]=0x95;
        buffer[55]=0x03;
        buffer[56]=0x81;
        buffer[57]=0x06;
        buffer[58]=0xc0;
        buffer[59]=0xc0;
        buffer[60]=0x00;
        buffer[61]=0x00;
        buffer[62]=0x00;
        buffer[63]=0x00;
        buffer[64]=0x70;
        buffer[65]=0x68;
        buffer[66]=0xa5;
        buffer[67]=0x01;
        buffer[68]=0x00;
        buffer[69]=0x00;
        buffer[70]=0x00;
        buffer[71]=0x00;
        buffer[72]=0x03;
        buffer[73]=0x00;
        buffer[74]=0x00;
        buffer[75]=0x00;
        buffer[76]=0x00;
        buffer[77]=0x00;
        buffer[78]=0x00;
        buffer[79]=0x00;
        buffer[80]=0x2a;
        buffer[81]=0x00;
        buffer[82]=0x00;
        buffer[83]=0x00;
        buffer[84]=0x22;
        buffer[85]=0x00;
        buffer[86]=0x00;
        buffer[87]=0x00;
        buffer[88]=0x02;
        buffer[89]=0x80;
        buffer[90]=0x00;
        buffer[91]=0x00;
        buffer[92]=0x00;
        buffer[93]=0x00;
        buffer[94]=0x00;
        buffer[95]=0x00;
        buffer[96]=0x00;
        buffer[97]=0x00;
        buffer[98]=0x00;
        buffer[99]=0x00;
        buffer[100]=0x00;
        buffer[101]=0x00;
        buffer[102]=0x00;
        buffer[103]=0x00;
        buffer[104]=0x70;
        buffer[105]=0x68;
        buffer[106]=0xa5;
        buffer[107]=0x01;
        buffer[108]=0x00;
        buffer[109]=0x00;
        buffer[110]=0x00;
        buffer[111]=0x00;
        buffer[112]=0x2a;
        buffer[113]=0x00;
        buffer[114]=0x00;
        buffer[115]=0x00;
        buffer[116]=0x22;
        buffer[117]=0x00;
        buffer[118]=0x00;
        buffer[119]=0x00;
        buffer[120]=0x00;
        buffer[121]=0x00;
        buffer[122]=0x00;
        buffer[123]=0x00;
		urb.buffer_length=124;
		urb.actual_length=52;
		urb.start_frame=0;
		urb.number_of_packets=0;
		urb.error_count=0;
		urb.signr=0;
		urb.usercontext = 0x00;
		urb.iso_frame_desc[0].length = 0x22000681;
		urb.iso_frame_desc[0].actual_length = 0x00740000;
		urb.iso_frame_desc[0].status = 0x02090105;
		break;
	default :
		return -1;
	}
	return send_urb(&urb, buffer);
}

void set_urb_control()
{
	int i;
	urb.type=1;
	urb.endpoint=0x81;
	urb.status=0;
	urb.flags=0x00000000;
	urb.buffer_length=4;
	urb.actual_length=4;
	urb.start_frame=0;
	urb.number_of_packets=0;
	urb.error_count=0;
	urb.signr=0;
	urb.usercontext = 0x00;
}

int send_control(unsigned char *buffer)
{
	unsigned int tmp_length = 0x00000000;

	tmp_length = buffer[3];
	tmp_length = tmp_length << 8;
	tmp_length = tmp_length | buffer[2];
	tmp_length = tmp_length << 8;
	tmp_length = tmp_length | buffer[1];
	tmp_length = tmp_length << 8;
	tmp_length = tmp_length | buffer[0];
	urb.iso_frame_desc[0].length = tmp_length;
	printf("tmp_length : 0x%08x\n", tmp_length);

	if(++seqnum%2) {
		urb.iso_frame_desc[0].actual_length = 0x00000000;
		urb.iso_frame_desc[0].status = 0x00000061;
	}
	else {
		urb.iso_frame_desc[0].actual_length = 0x00740000;
		urb.iso_frame_desc[0].status = 0x00000021;
	}

	if(send_urb(&urb, buffer) <0)
		return -1;

	return 0;
}

JNIEXPORT jstring JNICALL Java_app_android_ltouchpad_UsbipDevice_get_1message
  (JNIEnv *env, jobject obj) {
	jstring str;
	str = (*env)->NewStringUTF(env, message);
	return str;
}
