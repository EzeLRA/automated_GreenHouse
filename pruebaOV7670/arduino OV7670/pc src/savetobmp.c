/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	savetobmp.c				                                        */
/*		Functionality  :    bmp file write            			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/********************************************************************************************/
/*                          Defines	                                                        */
/********************************************************************************************/
#define QVGA_W    320
#define QVGA_H    240
#define QQVGA_W   160
#define QQVGA_H   120
#define VGA_W     640
#define VGA_H     480

/********************************************************************************************/
/*                          Extern variables                                                */
/********************************************************************************************/
extern int ts_stream_start;
/********************************************************************************************/
/*                          Global Vars	                                                    */
/********************************************************************************************/
const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

int cnt=0;
/********************************************************************************************/
/*                          Function Prototypes                                             */
/********************************************************************************************/
void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);
static void yuvtorgb(int Y, int U, int V, unsigned char *rgb);
/********************************************************************************************/
/*                          Function Definition                                             */
/********************************************************************************************/
void convertyuvtoBMP()
{
    int height = QQVGA_H;
    int width = QQVGA_W;
    unsigned char image[height][width][BYTES_PER_PIXEL];
    //char* imageFileName = (char*) "bitmapImage.bmp";
    char imageFileName[128];
    FILE *testfptr,*fptr;
    int i, j;
    char tempnamebuffer[128];
    
    //unsigned char buffertemp[VGA_W*VGA_H];//for monochrome
    //unsigned char buffertemp[QVGA_W*QVGA_H];//for monochrome
    //unsigned char buffertemp[QQVGA_W*QQVGA_H];//for monochrome
    unsigned char buffertemp[QQVGA_W*QQVGA_H*2];//for colour
    
    
    unsigned char yvalue,vvalue,uvalue,nextyvalue;
    
    int rgbval;
    
    sprintf(imageFileName,"bitmapImage%d.bmp",cnt);
    //cnt++;
    
    testfptr=fopen("img1","r");
    //fread(buffertemp,1,QQVGA_W*QQVGA_H,testfptr);
    fread(buffertemp,1,QQVGA_W*QQVGA_H*2,testfptr);//for colour
    fclose(testfptr);
    
    for ( i = 0; i < height; i++)
    for ( j = 0; j < width; )
    {
        /*
        //monochrome
        //yvalue=buffertemp[i*640+j];//for vga monochrome
        //yvalue=buffertemp[i*320+j];//for qvga monochrome
        yvalue=buffertemp[i*160+j];//for qqvga monochrome

        image[i][j][0]=yvalue;
        image[i][j][1]=yvalue;
        image[i][j][2]=yvalue;
        j++;

        */ 
        
        //qqvga colour yuv422 to rgb
        uvalue=buffertemp[i*320+j*2+1];
        yvalue=buffertemp[i*320+j*2];
        nextyvalue=buffertemp[i*320+j*2+2];
        vvalue=buffertemp[i*320+j*2+3];

        yuvtorgb(yvalue,uvalue,vvalue,image[i][j]);
        j++;
        yuvtorgb(nextyvalue,uvalue,vvalue,image[i][j]);
        j++;



        //qqvga colour rgb565 to rgb888                    
        /*rgbval= buffertemp[i*320+j*2+1]|(buffertemp[i*320+j*2]<<8);                    
                        
        image[i][j][0] = ((((rgbval >> 11) & 0x1F) * 527) + 23) >> 6;
        image[i][j][1] = ((((rgbval >> 5) & 0x3F) * 259) + 33) >> 6;
        image[i][j][2] = (((rgbval & 0x1F) * 527) + 23) >> 6;

        j++;*/
    }


    printf("haai %d %d\n",i,j);
    
    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    system("rm -f img1");
    printf("Image generated!!\n");
 
    if((fptr=fopen("tmp.jpg","r"))==NULL)
    {
        sprintf(tempnamebuffer,"convert -quality 90 %s tmp.jpg",imageFileName);
        system(tempnamebuffer);
    }
    else
    {
        fclose(fptr);
    }
}

/* convert a YUV set to a rgb set - thanks to MartinS and   http://www.efg2.com/lab/Graphics/Colors/YUV.htm */
static void yuvtorgb(int Y, int U, int V, unsigned char *rgb)
{
	int r, g, b;
	static short L1[256], L2[256], L3[256], L4[256], L5[256];
	static int initialised;

	if (!initialised) {
		int i;
		initialised=1;
		for (i=0;i<256;i++) {
			L1[i] = 1.164*(i-16);
			L2[i] = 1.596*(i-128);
			L3[i] = -0.813*(i-128);
			L4[i] = 2.018*(i-128);
			L5[i] = -0.391*(i-128);
		}
	}
#if 0
	r = 1.164*(Y-16) + 1.596*(V-128);
	g = 1.164*(Y-16) - 0.813*(U-128) - 0.391*(V-128);
	b = 1.164*(Y-16) + 2.018*(U-128);
#endif

	r = L1[Y] + L2[V];
	g = L1[Y] + L3[U] + L5[V];
	b = L1[Y] + L4[U];

	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) 
    {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = 
    {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = 
    {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}

 
