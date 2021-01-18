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
int *BVRA2ARVB(int largeur, int hauteur, const unsigned char *donnees){
	const unsigned char *ptrDonnees;
	unsigned char *pixels = (unsigned char*)malloc(largeur*hauteur*sizeof(int));
	unsigned char *ptrPixel;
	int index;

	ptrPixel = pixels;
	ptrDonnees = donnees;
	for(index = largeur*hauteur; index != 0; --index){ //On parcourt tous les pixels de l'image
		ptrPixel[0] = ptrDonnees[3];
		ptrPixel[1] = ptrDonnees[2];
		ptrPixel[2] = ptrDonnees[1];
		ptrPixel[3] = ptrDonnees[0];
		ptrDonnees += 4; /* On passe a la premiere composante du pixel suivant */
		ptrPixel += 4; /* On passe au pixel suivant */
	}
	return (int*)pixels;
}

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
		for(int i=0; i < 4*image.width; i++){
			intData[a] = (unsigned char)(
				100*charToInt(data[ (int)(h*12*image.width+3*i  ) ]) +
				 10*charToInt(data[ (int)(h*12*image.width+3*i+1) ]) +
					charToInt(data[ (int)(h*12*image.width+3*i+2) ])
			);
			a++;
		}
	}
	image.data = BVRA2ARVB(image.width, image.height, intData);
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
