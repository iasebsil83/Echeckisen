#ifndef __ECHECKISEN_H__
#define __ECHECKISEN_H__



//to activate graphic mode :
//	uncomment the line below
	#define GRAPHICS_ON
//	do the same in Echeckisen.c and in makefile before compilation



// ======================= SAVE ===========================

// ---- data management ----
#ifndef GRAPHICS_ON
int charToInt(char c);
#endif
void delChar(char* x, int a, int b);
int saveGame(game* g);
int loadGame(game* g);



// ====================== DISPLAY =========================

// ---- graphics ----
#ifdef GRAPHICS_ON
void setSprites();
img getSprite(square p);
void graphic_display(game* g);
#endif

// ---- console ----
void printfString(square p);
void console_display(game* g);

// ---- both ----
void refreshScreens(game* g);



// ====================== CONTROL ==========================

// ---- initialisation ----
void setBasicValues(game* g);
void globalInitialization(game* g);

// ---- IHM ----
void keyboard_special(game* g, char key);
void keyboard_normal(game* g, char key);

// ---- time ----
void timedExecution(game* g);



// ====================== MAIN CODE ========================

// ---- console only ----
#ifndef GRAPHICS_ON
void* console_timeLoop(void* g);
void console_gestionEvenements(char cmd, game* g);
void lanceBoucleEvenements();
#endif



#endif
