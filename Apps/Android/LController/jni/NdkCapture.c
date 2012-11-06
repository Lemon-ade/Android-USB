#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include "pixelflinger.h"
#include "NdkCapture.h"
#define SOCKADDR_SIZE sizeof(struct sockaddr_in)
#define PORT 4001
#define MAXLINE 1024

struct sockaddr_in sv_addr;
struct sockaddr_in cl_addr;
int sockfd_connect;
int sockfd_listen;
int port_num;
char errmsg[MAXLINE];

uint8_t *rgb24=NULL;
uint8_t *rgb24_rotate=NULL;
uint8_t *rot_des;

typedef struct {
     long filesize;
     char reserved[2];
     long headersize;
     long infoSize;
     long width;
     long depth;
     short biPlanes;
     short bits;
     long biCompression;
     long biSizeImage;
     long biXPelsPerMeter;
     long biYPelsPerMeter;
     long biClrUsed;
     long biClrImportant;
} BMPHEAD;

//copyright text
char cprght[255]="Copyright(C)2009 Motisan Radu , All rights reserved.\n radu.motisan@gmail.com";
//surface pointer
static GGLSurface gr_framebuffer[2];
//handler
static int gr_fb_fd = -1;
//v screen info
static struct fb_var_screeninfo vi;
//f screen info
struct fb_fix_screeninfo fi;

static int get_framebuffer(GGLSurface *fb, char* errmsg)
{
    int fd;
    void *bits;

    fd = open("/dev/graphics/fb0", O_RDWR);
    if(fd < 0) {
        //perror("cannot open fb0");
	sprintf(errmsg, "cannot open fb0\n");
        return -1;
    }

    if(ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
        //perror("failed to get fb0 info");
	sprintf(errmsg, "failed to get fb0 info\n");
        return -1;
    }

    if(ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
        //perror("failed to get fb0 info");
	sprintf(errmsg, "failed to get fb0 info\n");
        return -1;
    }

    //dumpinfo(&fi, &vi);

    bits = mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(bits == MAP_FAILED) {
        //perror("failed to mmap framebuffer");
	sprintf(errmsg, "failed to mmap framebuffer\n");
        return -1;
    }
	
    fb->version = sizeof(*fb);
    fb->width = vi.xres;
    fb->height = vi.yres;
    fb->stride = fi.line_length / (vi.bits_per_pixel >> 3);
    fb->data = bits;
    fb->format = GGL_PIXEL_FORMAT_RGB_565;

    fb++;

    fb->version = sizeof(*fb);
    fb->width = vi.xres;
    fb->height = vi.yres;
    fb->stride = fi.line_length / (vi.bits_per_pixel >> 3);
    fb->data = (void*) (((unsigned) bits) + vi.yres * vi.xres * 2);
    fb->format = GGL_PIXEL_FORMAT_RGB_565;

    return fd;
}

int TCPConnect_send_msg(BMPHEAD bh, uint8_t* rgb24) {
	int i;

	//printf("send...");
	i=send(sockfd_connect, &bh, sizeof(bh), 0);
	if(i<0) return -1;
	i=send(sockfd_connect, rgb24, bh.depth * bh.width * bh.bits / 8, 0);
	if(i<0) return -1;
	return i;
}

JNIEXPORT jint JNICALL Java_app_android_lcontroller_CaptureScreen_TCPConnect_1listen_1server
  (JNIEnv *env, jobject obj, jint port) {
	port_num = port;
	if((sockfd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		sprintf(errmsg, "listen socket make error");
		return -1;
	}
    
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sv_addr.sin_port = htons(port);
    
	if(bind(sockfd_listen, (struct sockaddr *)&sv_addr, SOCKADDR_SIZE) == -1)
	{
		sprintf(errmsg, "bind failed");
		return -1;
	}

	if(listen(sockfd_listen, 1) == -1)
	{
		sprintf(errmsg, "listen error");
		return -1;
	}

	return 0;
}

JNIEXPORT jstring JNICALL Java_app_android_lcontroller_CaptureScreen_TCPConnect_1accept_1client
  (JNIEnv *env, jobject obj) {
	jstring clientIP_jstr;
	char clientIP[MAXLINE];
	int addrlen = sizeof(cl_addr);

	if((sockfd_connect = accept(sockfd_listen, (struct sockaddr *)&cl_addr, &addrlen)) ==-1)
    	{
		sprintf(errmsg, "accept error");
		return -1;
	}

	memset(clientIP, 0x00, MAXLINE);
	strcpy(clientIP, inet_ntoa(cl_addr.sin_addr));
	clientIP_jstr = (*env)->NewStringUTF(env, clientIP);
	return clientIP_jstr;
}

JNIEXPORT jint JNICALL Java_app_android_lcontroller_CaptureScreen_TCPConnect_1recv_1msg
  (JNIEnv *env, jobject obj) {
	char content_recv[MAXLINE];
	int i;

	memset(content_recv, 0x00, MAXLINE);
	i=recv(sockfd_connect,content_recv,MAXLINE,0);
	//printf("recv : %s\n", content_recv);

	return i;
}

JNIEXPORT jint JNICALL Java_app_android_lcontroller_CaptureScreen_TCPConnect_1send_1screen
  (JNIEnv *env, jobject obj) {
	int line,x,ret;
	char tmp[MAXLINE];
	
	//get screen capture
	gr_fb_fd = get_framebuffer(gr_framebuffer, tmp);
	if (gr_fb_fd <= 0) {
		sprintf(errmsg, "ERROR : can't get screen capture - %s", tmp);
		return -1;
	}

	int w = vi.xres, h = vi.yres, depth = vi.bits_per_pixel;

	if(rgb24==NULL)
		rgb24 = (uint8_t *)malloc(w * h * 3);
	if(rgb24_rotate==NULL)
		rgb24_rotate = (uint8_t *)malloc(w * h * 3);
	if(!rgb24 || !rgb24_rotate) {
		sprintf(errmsg, "memory allocate error\n");
		return -1;
	}

	if(sizeof(rgb24) != w*h*3) {
		realloc(rgb24, w*h*3);
		realloc(rgb24_rotate, w*h*3);

		if(!rgb24_rotate || !rgb24) {
			sprintf(errmsg, "memory allocation error\n");
			return -1;
		}
	}

	if (depth == 16)
	{
		int i = 0;
		for (;i<w*h;i++)
		{
			uint16_t pixel16 = ((uint16_t *)gr_framebuffer[0].data)[i];
			// RRRRRGGGGGGBBBBBB -> RRRRRRRRGGGGGGGGBBBBBBBB
			// in rgb24 color max is 2^8 per channel (*255/32 *255/64 *255/32)
			rgb24[3*i+2]   = (255*(pixel16 & 0x001F))/ 32; 		//Blue
			rgb24[3*i+1]   = (255*((pixel16 & 0x07E0) >> 5))/64;	//Green
			rgb24[3*i]     = (255*((pixel16 & 0xF800) >> 11))/32; 	//Red
		}
	}
	else if (depth == 24) //exactly what we need
	{
		memcpy(rgb24, (uint8_t*)gr_framebuffer[0].data, sizeof(rgb24)); 
		// rgb24 = (uint8_t *) gr_framebuffer[0].data;
	}
	else if (depth == 32) //skip transparency channel
	{
		int i=0;
		for (;i<w*h;i++)
		{
			uint32_t pixel32 = ((uint32_t *)gr_framebuffer[0].data)[i];
			// in rgb24 color max is 2^8 per channel 
			rgb24[3*i+0]   =  pixel32 & 0x000000FF; 		//Blue
			rgb24[3*i+1]   = (pixel32 & 0x0000FF00) >> 8;	//Green
			rgb24[3*i+2]   = (pixel32 & 0x00FF0000) >> 16; 	//Red
		}
	} 
	else
	{
		//free
		close(gr_fb_fd);
		return -1;
	};

	//save RGB 24 Bitmap
	int bytes_per_pixel = 3;
	BMPHEAD bh;
	memset ((char *)&bh,0,sizeof(BMPHEAD)); // sets everything to 0 
	//bh.filesize  =   calculated size of your file (see below)
	//bh.reserved  = two zero bytes
	bh.headersize  = 54L;			// for 24 bit images
	bh.infoSize  =  0x28L;		// for 24 bit images
	bh.width     = w;			// width of image in pixels
	bh.depth     = h;			// height of image in pixels
	bh.biPlanes  =  1;			// for 24 bit images
	bh.bits      = 8 * bytes_per_pixel;	// for 24 bit images
	bh.biCompression = 0L;		// no compression
	int bytesPerLine;
	bytesPerLine = w * bytes_per_pixel;  	// for 24 bit images
	//round up to a dword boundary 
	if (bytesPerLine & 0x0003) 
	{
		  bytesPerLine |= 0x0003;
		  ++bytesPerLine;
	}
	bh.filesize = bh.headersize + (long)bytesPerLine * bh.depth;

	for (line = h-1; line >= 0; line-- )
	{
		for( x =0 ; x < w; x++ )
		{
			rot_des = rgb24_rotate + (x*h*bytes_per_pixel) + (h-1-line)*bytes_per_pixel;
		   	*(rot_des) = *(rgb24 + (x+line*w)*bytes_per_pixel);
		   	*(rot_des+1) = *(rgb24 + (x+line*w)*bytes_per_pixel+1);
		   	*(rot_des+2) = *(rgb24 + (x+line*w)*bytes_per_pixel+2);
		}
	}
	bh.depth = w;
	bh.width = h;
//*/

	ret = TCPConnect_send_msg(bh, rgb24_rotate);
	if(ret<1) {
		sprintf(errmsg, "disconnect client\n");
		return -1;
	}

	close(gr_fb_fd);
	return 0;
}

JNIEXPORT jint JNICALL Java_app_android_lcontroller_CaptureScreen_TCPConnect_1close
  (JNIEnv *env, jobject obj) {
	
}

JNIEXPORT jstring JNICALL Java_app_android_lcontroller_CaptureScreen_getErrorMsg
  (JNIEnv *env, jobject obj) {
	jstring errmsg_jstr;
	errmsg_jstr = (*env)->NewStringUTF(env, errmsg);
	return errmsg_jstr;
}
