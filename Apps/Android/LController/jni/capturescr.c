
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

#define SOCKADDR_SIZE sizeof(struct sockaddr_in)
#define PORT 4001
#define MAXLINE 1024
struct sockaddr_in sv_addr;
struct sockaddr_in cl_addr;
int sockfd_connect;
int sockfd_listen;
int port_num;
char errmsg[MAXLINE];

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

static void dumpinfo(struct fb_fix_screeninfo *fi,
                     struct fb_var_screeninfo *vi);

static int get_framebuffer(GGLSurface *fb)
{
    int fd;
    void *bits;

    fd = open("/dev/graphics/fb0", O_RDWR);
    if(fd < 0) {
        perror("cannot open fb0");
        return -1;
    }

    if(ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
        perror("failed to get fb0 info");
        return -1;
    }

    if(ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
        perror("failed to get fb0 info");
        return -1;
    }

    //dumpinfo(&fi, &vi);

    bits = mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(bits == MAP_FAILED) {
        perror("failed to mmap framebuffer");
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


static void dumpinfo(struct fb_fix_screeninfo *fi, struct fb_var_screeninfo *vi)
{
    fprintf(stderr,"vi.xres = %d\n", vi->xres);
    fprintf(stderr,"vi.yres = %d\n", vi->yres);
    fprintf(stderr,"vi.xresv = %d\n", vi->xres_virtual);
    fprintf(stderr,"vi.yresv = %d\n", vi->yres_virtual);
    fprintf(stderr,"vi.xoff = %d\n", vi->xoffset);
    fprintf(stderr,"vi.yoff = %d\n", vi->yoffset);
    fprintf(stderr, "vi.bits_per_pixel = %d\n", vi->bits_per_pixel);

    fprintf(stderr, "fi.line_length = %d\n", fi->line_length);

}

int TCPConnect_listen_server(int port) {
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

int TCPConnect_accept_client(char* clientIP, int size) {
	//jstring clientIP_jstr;
	int addrlen = sizeof(cl_addr);

	if((sockfd_connect = accept(sockfd_listen, (struct sockaddr *)&cl_addr, &addrlen)) ==-1)
    	{
		sprintf(errmsg, "accept error");
		return -1;
	}

	memset(clientIP, 0x00, size);
	strcpy(clientIP, inet_ntoa(cl_addr.sin_addr));
	return 0;
	//clientIP_jstr = (*env)->NewStringUTF(env, clientIP);
	//return clientIP_jstr;
}

int TCPConnect_recv_msg() {
	char content_recv[MAXLINE];
	int i;

	memset(content_recv, 0x00, MAXLINE);
	i=recv(sockfd_connect,content_recv,MAXLINE,0);
	//printf("recv : %s\n", content_recv);

	return i;
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

int main(int argc, char **argv) 
{
  int ret;
  char clientIP[MAXLINE];
  int line,x;

  //convert pixel data
  uint8_t *rgb24=NULL;
  uint8_t *rgb24_rotate=NULL;
  uint8_t *rot_des;

  printf("server opening...\n");
  //open server
  ret = TCPConnect_listen_server(PORT);
  if(ret<0) {
	fprintf(stderr, "ERROR : %s\n", errmsg);
	return -1;
  }

  //connect client
  while(1) {
	printf("accepting client...\n");
  	ret = TCPConnect_accept_client(clientIP, MAXLINE);
  	if(ret<0) {
		fprintf(stderr, "ERROR : %s\n", errmsg);
		continue;
	}
	printf("connect %s\n", clientIP);

	while(TCPConnect_recv_msg()>0) {
	  	//get screen capture
		gr_fb_fd = get_framebuffer(gr_framebuffer);
		if (gr_fb_fd <= 0) {
			fprintf(stderr, "ERROR : can't get screen capture\n");
			continue;
		}

		int w = vi.xres, h = vi.yres, depth = vi.bits_per_pixel;

		if(rgb24==NULL)
			rgb24 = (uint8_t *)malloc(w * h * 3);
		if(rgb24_rotate==NULL)
			rgb24_rotate = (uint8_t *)malloc(w * h * 3);
		if(!rgb24 || !rgb24_rotate) {
		  	fprintf(stderr, "memory allocate error\n");
			exit(1);
		}

		if(sizeof(rgb24) != w*h*3) {
			realloc(rgb24, w*h*3);
			realloc(rgb24_rotate, w*h*3);

			if(!rgb24_rotate || !rgb24) {
				fprintf(stderr, "memory allocation error\n");
				exit(1);
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
			exit(2);
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

/*
		int w_ro = h/2, h_ro = w/2;
		for (line = h-1; line >= 0; line-=2 ) // line--
		{
			for( x =0 ; x < w; x+=2 ) // x++
		  	{
				rot_des = rgb24_rotate + (x*w_ro*bytes_per_pixel/2) + (w_ro-1-line/2)*bytes_per_pixel;
		   		*(rot_des) = *(rgb24 + (x+line*w)*bytes_per_pixel);
		   		*(rot_des+1) = *(rgb24 + (x+line*w)*bytes_per_pixel+1);
		   		*(rot_des+2) = *(rgb24 + (x+line*w)*bytes_per_pixel+2);
			}
		}
		bh.depth = h_ro;
		bh.width = w_ro;
*/
///* original
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
			fprintf(stderr, "disconnect client\n");
			break;
		}

		close(gr_fb_fd);
	}
	printf("connect end\n");
  }

  free(rgb24);
  free(rgb24_rotate);
  return 0;
}
