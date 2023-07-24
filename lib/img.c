// ---------------- IMPORTATIONS ----------------
#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <glut.h>
#endif






// ---------------- DEFINITIONS ----------------
struct IMG{
	int width,height;
	int* data;
};
typedef struct IMG img;






// ---------------- UTILITIES ----------------
int charToInt(char c){
	switch(c){
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
	}
	return 0;
}

//read .rgba file and convert it to an ARVB
img readRGBA2ARVB(char name[100], img image){
	char data[(int)(3*4*image.width*image.height)];
	FILE* file;
	file = fopen(name,"r");
	int d = 0;
	char c = '_';
	if(file != NULL){
		while((c=fgetc(file)) != EOF)
			data[d++] = c;
	}
	fclose(file);

	unsigned char* intData = (unsigned char*)malloc(4*image.width*image.height*sizeof(unsigned char));
	int a=0;
	for(int h=image.height-1; h >= 0; h--){
		for(int i = 4*image.width - 1; i >= 0; i--){
			intData[a] = (unsigned char)(
				100*charToInt(data[ (int)(h*12*image.width+3*i  ) ]) +
				 10*charToInt(data[ (int)(h*12*image.width+3*i+1) ]) +
					charToInt(data[ (int)(h*12*image.width+3*i+2) ])
			);
			a++;
		}
	}
	image.data = (int*)intData;
	return image;
}

void ecrisImageIMG(int x, int y, img image){ //(x,y) will be the left bottom corner of the image
	if(image.data == NULL)
		return;
	glRasterPos2i(x,y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glDrawPixels(image.width, image.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image.data);
}
