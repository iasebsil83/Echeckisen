//	THIS PROGRAM ALLOWS YOU TO DISABLE THE GRAPHIC INTERFACE
//
//	IF YOU DISABLE THIS INTERFACE, ANOTHER TERMINAL WILL BE
//	LAUNCHED FOR YOU AS INPUT
//
//	IF YOU ACTIVATE THE GRAPHIC INTERFACE, YOU WILL BE ABLE
//	TO HAVE NORMAL INPUT WITH KEYBOARD (FOCUSING THE WINDOW)

//to activate graphic mode :
//	uncomment the line below
	#define GRAPHICS_ON
//	do the same in makefile before compilation

//keys for console-only mode
//	- O : go up
//	- L : go down
//	- K : go left
//	- M : go right
//      - A : Action

//console-only mode does not have DRAW key (not pertinent)






//standard
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifndef GRAPHICS_ON
	#include <pthread.h>
#else
	//graphics
	#include "src/S2DE.h"
	#include "src/img.c"
#endif
//game
#include "src/files.h"
#include "Utilities.h"






/*~~~~~~~~~~~~~~~~~~~~~~~~~~~ Echeckisen [0.1.5] ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Developped using S2DE :

        Project developped for ISEN Toulon, an Yncrea Mediterranee engineering school
    and then adapted with S2DE graphic library.

    08/01/2020 > [0.1.0] :
        - Defining player and square structures and enums
        - Graphic and console display of board and squares
        - Turn counters for each player and total
        - Time spent for each player and total
        - Timer for the current turn (automatic turn switch)
        - Selection system : MOVE / POSSIBILITY options
        - Possibility calculations for each piece
        BUGS : Images are incorrectly displayed

    09/01/2020 > [0.1.1] :
        - Added a few sprites
        - Fixed image display bugs (Echeckisen.c and GfxLib.c)
        - Dead squares system
        - Functions existsDead(), addDead() and rmvDead() implemented
        - Pawn transformation (inside rmvDead() and moveSquare())
        - Player attribute "checked" and function checkForChecked() (without content)
        - Save option key (input and display without save system)
        BUGS : Pawns can kill in front of them
        NOTES : MOVE and POSSIBILITIES modes have the same selection sprite
        This can be confusing if piece don't have any possibility to move

    09/01/2020 > [0.1.2] :
        - Pawns can't kill in front of them
        - Pawns can kill in front diagonals if enemy on it
        - Adding sprite for selection in POSSIBILITIES mode
        - Basic functions for position set for pawns and knight
        - Adding sprite to see selected piece in POSSIBILITIES mode
        - Player attribute "checked" and function checkForChecked() (content)
        - Detection of mat added in checkForChecked()
        - Win screens (BLACK and WHITE)
        - Draw key (leads to another win screen saying "DRAW")
        - Restart option (press R) and function resetGame()
        - Game information (last action) displayed in console
        - Set game information when piece moved
        - Set game information when pawn promoted
        - Set game information when piece move, eating an opponent
        - Function getTypeChar() added for better comfort
        BUGS : Turn timer not updating when moving a piece

    10/01/2020 > [0.1.3] :
        - Fixed previous turn timer bug
        - Save system implemented (in save/save.txt)

    12/01/2020 > [0.1.4] :
        - Graphic interface activator at start of Echeckisen.c, Echeckisen.h and makefile
        - Separation in 2 files (+headers) : Echeckisen.c/.h Utilities.c/.h

    18/01/2021 > [0.1.5] :
        - Adapted with S2DE.

    Contacts : frederic.terrasson@isen.yncrea.fr
               axel.meloni@isen.yncrea.fr
               sebastien.silvano@isen.yncrea.fr

    ISEN Toulon : www.isen.fr
******************************************************************************************

    LICENCE :

    Echeckisen
    Copyright (C) 2020  Sebastien SILVANO & Axel MELONI & Frederic TERRASSON
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.

    If not, see <https://www.gnu.org/licenses/>.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/






// ==================== DECLARATIONS ======================

// ---- game ----
const int maxl = 700;
const int maxh = 700;
const int GAME_INFO_LEN = 50;

// ---- players ----
const int MAX_NAME_LENGTH = 50;

// ---- time management ----
const int MAX_GAME_TIME = 3600;
const int MAX_SECONDS_PER_TURN = 180;

// ---- selection ----
const int SELECTION_OFFSET = 3;

// ---- save system ----
const int MAX_SAVECNT = 1;

#ifdef GRAPHICS_ON
// ---- graphics ----
const int BOARD_BORDER = 62;
const int POSS_BORDER = 1;
const int SQUARE_SIZE = 72;

// ---- S2DE ----
extern int S2DE_keyState;
extern short S2DE_key;

// ---- sprites ----
static img img_void;
static img img_board;
static img img_poss;
static img img_moveSel;
static img img_piecePoss;
static img img_possSel;
static img img_replay;
static img img_draw;
static img img_w_win; static img img_b_win;
//pieces
static img img_w_rook,   img_b_rook;
static img img_w_bishop, img_b_bishop;
static img img_w_knight, img_b_knight;
static img img_w_queen,  img_b_queen;
static img img_w_king,   img_b_king;
static img img_w_pawn,   img_b_pawn;
#else
static int threadOK = 0;
#endif






// ======================= SAVE ===========================

// ---- data management ----
#ifndef GRAPHICS_ON
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
#endif
int saveGame(game* g){
	printf("Saving game...\n");

	//save data
	size_t saveLength = 168;
	char* saveString = malloc(saveLength);
	unsigned int index=0;

	//distributes data
	//add board
	for(int i=0; i < 8; i++){
		for(int k=0; k < 8; k++){
			saveString[index++] = (char)(g->board[i][k].owner) + '0';
			saveString[index++] = (char)(g->board[i][k].type ) + '0';
		}
	}

	//add white dead then add black dead
	for(int i=0; i < 7; i++){
		saveString[index++] = (char)(g->p_white.dead[i].owner) + '0';
		saveString[index++] = (char)(g->p_white.dead[i].type ) + '0';
	}
	for(int i=0; i < 7; i++){
		saveString[index++] = (char)(g->p_black.dead[i].owner) + '0';
		saveString[index++] = (char)(g->p_black.dead[i].type ) + '0';
	}

	//add castlings and kingMoved
	saveString[index++] = (char)(g->p_white.castling ) + '0';
	saveString[index++] = (char)(g->p_black.castling ) + '0';
	saveString[index++] = (char)(g->p_white.kingMoved) + '0';
	saveString[index++] = (char)(g->p_black.kingMoved) + '0';

	//add turn player
	//white
	saveString[index++] = (char)(
		(
			g->p_white.turnCnt - 10000*(int)(g->p_white.turnCnt/10000)
		)/1000
	) + '0';
	saveString[index++] = (char)(
		(
			g->p_white.turnCnt - 1000*(int)(g->p_white.turnCnt/1000)
		)/100
	) + '0';
	saveString[index++] = (char)(
		(
			g->p_white.turnCnt - 100*(int)(g->p_white.turnCnt/100)
		)/10
	) + '0';
	saveString[index++] = (char)(
		(
			g->p_white.turnCnt - 10*(int)(g->p_white.turnCnt/10)
		)
	) + '0';

	//black
	saveString[index++] = (char)(
		(
			g->p_black.turnCnt - 10000*(int)(g->p_black.turnCnt/10000)
		)/1000
	) + '0';
	saveString[index++] = (char)(
		(
			g->p_black.turnCnt - 1000*(int)(g->p_black.turnCnt/1000)
		)/100
	) + '0';
	saveString[index++] = (char)(
		(
			g->p_black.turnCnt - 100*(int)(g->p_black.turnCnt/100)
		)/10
	) + '0';
	saveString[index  ] = (char)(
		(
			g->p_black.turnCnt - 10*(int)(g->p_black.turnCnt/10)
		)
	) + '0';

	//save
	file_write("save/save.txt", &saveString, &saveLength);
	free(saveString);
	printf("Game saved.\n");
	usleep(3000000);

	return 1;
}

int loadGame(game* g){
	printf("Loading save...\n");

	//save file
	size_t saveLength = 0;
	char* loadString = NULL;

	//read save
	file_read("save/save.txt", &loadString, &saveLength);
	if(saveLength != 168){
		printf("Incorrect data length in save file.\n");
		if(loadString != NULL)
			free(loadString);
		usleep(4000000);
		return 0;
	}
	unsigned int index = 0;

	//distribute data on game variables
	//add board
	for(int i=0; i < 8; i++){
		for(int k=0; k < 8; k++){
			g->board[i][k].owner = charToInt(loadString[index++]);
			g->board[i][k].type  = charToInt(loadString[index++]);
		}
	}

	//add white dead then add black dead
	for(int i=0; i < 7; i++){
		g->p_white.dead[i].owner = charToInt(loadString[index++]);
		g->p_white.dead[i].type  = charToInt(loadString[index++]);
	}
	for(int i=0; i < 7; i++){
		g->p_black.dead[i].owner = charToInt(loadString[index++]);
		g->p_black.dead[i].type  = charToInt(loadString[index++]);
	}

	//add castlings and kingMoved
	g->p_white.castling  = charToInt(loadString[index++]);
	g->p_black.castling  = charToInt(loadString[index++]);
	g->p_white.kingMoved = charToInt(loadString[index++]);
	g->p_black.kingMoved = charToInt(loadString[index++]);

	//add turn player
	g->p_white.turnCnt =
		charToInt(loadString[index  ])*1000 +
		charToInt(loadString[index+1])*100 +
		charToInt(loadString[index+2])*10 +
		charToInt(loadString[index+3])
	;
	index += 4;
	g->p_black.turnCnt =
		charToInt(loadString[index  ])*1000 +
		charToInt(loadString[index+1])*100 +
		charToInt(loadString[index+2])*10 +
		charToInt(loadString[index+3])
	;
	printf("save loaded.\n");
	usleep(3000000);

	free(loadString);
	return 1;
}






// ====================== DISPLAY =========================

#ifdef GRAPHICS_ON
// ---- graphics ----
void setSprites(){
	//game
	img_void.width = 0; img_void.height = 0;
	img_void.data = NULL;
	img_board.width = 700; img_board.height = 700;
	img_board.data = NULL;
	img_board = readRGBA2ARVB("sprites/board[700x700].rgba", img_board);
	img_poss.width = SQUARE_SIZE; img_poss.height = SQUARE_SIZE;
	img_poss.data = NULL;
	img_poss = readRGBA2ARVB("sprites/poss[72x72].rgba", img_poss);
	img_moveSel.width = 78; img_moveSel.height = 78;
	img_moveSel.data = NULL;
	img_moveSel = readRGBA2ARVB("sprites/moveSel[78x78].rgba", img_moveSel);
	img_possSel.width = 78; img_possSel.height = 78;
	img_possSel.data = NULL;
	img_possSel = readRGBA2ARVB("sprites/possSel[78x78].rgba", img_possSel);
	img_piecePoss.width = 72; img_piecePoss.height = 72;
	img_piecePoss.data = NULL;
	img_piecePoss = readRGBA2ARVB("sprites/piecePoss[72x72].rgba", img_piecePoss);
	img_replay.width = 700; img_replay.height = 700;
	img_replay.data = NULL;
	img_replay = readRGBA2ARVB("sprites/replay[700x700].rgba", img_replay);
	img_draw.width = 700; img_draw.height = 700;
	img_draw.data = NULL;
	img_draw = readRGBA2ARVB("sprites/draw[700x700].rgba", img_draw);
	img_w_win.width = 700; img_w_win.height = 700;
	img_w_win.data = NULL;
	img_w_win = readRGBA2ARVB("sprites/W_win[700x700].rgba", img_w_win);
	img_b_win.width = 700; img_b_win.height = 700;
	img_b_win.data = NULL;
	img_b_win = readRGBA2ARVB("sprites/B_win[700x700].rgba", img_b_win);
	//pieces
	//white
	img_w_rook.width = SQUARE_SIZE; img_w_rook.height = SQUARE_SIZE;
	img_w_rook.data = NULL;
	img_w_rook = readRGBA2ARVB("sprites/pieces/W_rook[72x72].rgba", img_w_rook);
	img_w_bishop.width = SQUARE_SIZE; img_w_bishop.height = SQUARE_SIZE;
	img_w_bishop.data = NULL;
	img_w_bishop = readRGBA2ARVB("sprites/pieces/W_bishop[72x72].rgba", img_w_bishop);
	img_w_knight.width = SQUARE_SIZE; img_w_knight.height = SQUARE_SIZE;
	img_w_knight.data = NULL;
	img_w_knight = readRGBA2ARVB("sprites/pieces/W_knight[72x72].rgba", img_w_knight);
	img_w_queen.width = SQUARE_SIZE; img_w_queen.height = SQUARE_SIZE;
	img_w_queen.data = NULL;
	img_w_queen = readRGBA2ARVB("sprites/pieces/W_queen[72x72].rgba", img_w_queen);
	img_w_king.width = SQUARE_SIZE; img_w_king.height = SQUARE_SIZE;
	img_w_king.data = NULL;
	img_w_king = readRGBA2ARVB("sprites/pieces/W_king[72x72].rgba", img_w_king);
	img_w_pawn.width = SQUARE_SIZE; img_w_pawn.height = SQUARE_SIZE;
	img_w_pawn.data = NULL;
	img_w_pawn = readRGBA2ARVB("sprites/pieces/W_pawn[72x72].rgba", img_w_pawn);
	//black
	img_b_rook.width = SQUARE_SIZE; img_b_rook.height = SQUARE_SIZE;
	img_b_rook.data = NULL;
	img_b_rook = readRGBA2ARVB("sprites/pieces/B_rook[72x72].rgba", img_b_rook);
	img_b_bishop.width = SQUARE_SIZE; img_b_bishop.height = SQUARE_SIZE;
	img_b_bishop.data = NULL;
	img_b_bishop = readRGBA2ARVB("sprites/pieces/B_bishop[72x72].rgba", img_b_bishop);
	img_b_knight.width = SQUARE_SIZE; img_b_knight.height = SQUARE_SIZE;
	img_b_knight.data = NULL;
	img_b_knight = readRGBA2ARVB("sprites/pieces/B_knight[72x72].rgba", img_b_knight);
	img_b_queen.width = SQUARE_SIZE; img_b_queen.height = SQUARE_SIZE;
	img_b_queen.data = NULL;
	img_b_queen = readRGBA2ARVB("sprites/pieces/B_queen[72x72].rgba", img_b_queen);
	img_b_king.width = SQUARE_SIZE; img_b_king.height = SQUARE_SIZE;
	img_b_king.data = NULL;
	img_b_king = readRGBA2ARVB("sprites/pieces/B_king[72x72].rgba", img_b_king);
	img_b_pawn.width = SQUARE_SIZE; img_b_pawn.height = SQUARE_SIZE;
	img_b_pawn.data = NULL;
	img_b_pawn = readRGBA2ARVB("sprites/pieces/B_pawn[72x72].rgba", img_b_pawn);
}

img getSprite(square p){
	switch(p.owner){
		case VOID:
			return img_void;
		break;
		case WHITE:
			switch(p.type){
				case PAWN:
					return img_w_pawn;
				case ROOK:
					return img_w_rook;
				case KNIGHT:
					return img_w_knight;
				case BISHOP:
					return img_w_bishop;
				case QUEEN:
					return img_w_queen;
				case KING:
					return img_w_king;
			}
		break;
		case BLACK:
			switch(p.type){
				case PAWN:
					return img_b_pawn;
				case ROOK:
					return img_b_rook;
				case KNIGHT:
					return img_b_knight;
				case BISHOP:
					return img_b_bishop;
				case QUEEN:
					return img_b_queen;
				case KING:
					return img_b_king;
			}
		break;
	}
	return img_void;
}

void graphic_display(game* g){
	//board
	S2DE_imageRGBA(
		0,0,
		img_board.width, img_board.height,
		img_board.data
	);
	//pieces
	for(int y=0; y < 8; y++){
		for(int x=0; x < 8; x++){
			img i = getSprite(g->board[y][x]);
			S2DE_imageRGBA(
				BOARD_BORDER + x*SQUARE_SIZE,
				maxh - BOARD_BORDER - (y+1)*SQUARE_SIZE,
				i.width, i.height,
				i.data
			);
		}
	}
	if(g->win_mode){
		if(g->winner == WHITE){
			S2DE_imageRGBA(
				0,0,
				img_w_win.width, img_w_win.height,
				img_w_win.data
			);
		}else if(g->winner == BLACK){
			S2DE_imageRGBA(
				0,0,
				img_b_win.width, img_b_win.height,
				img_b_win.data
			);
		}else{
			S2DE_imageRGBA(
				0,0,
				img_draw.width, img_draw.height,
				img_draw.data
			);
		}
		S2DE_imageRGBA(
			0,0,
			img_replay.width, img_replay.height,
			img_replay.data
		);
	}else{
		//selection
		if(g->mode == MOVE){
			//selection square
			S2DE_imageRGBA(
				BOARD_BORDER + g->moveSelX*SQUARE_SIZE - SELECTION_OFFSET,
				maxh - BOARD_BORDER - (g->moveSelY+1)*SQUARE_SIZE - SELECTION_OFFSET,
				img_moveSel.width, img_moveSel.height,
				img_moveSel.data
			);
		}else{
			//possibilities
			for(int y=0; y < 8; y++){
				for(int x=0; x < 8; x++){
					if(g->board[y][x].able){
						S2DE_imageRGBA(
							BOARD_BORDER + x*SQUARE_SIZE - POSS_BORDER,
							maxh - BOARD_BORDER - (y+1)*SQUARE_SIZE - POSS_BORDER,
							img_poss.width, img_poss.height,
							img_poss.data
						);
					}
				}
			}
			//selected piece square
			//selection square
			S2DE_imageRGBA(
				BOARD_BORDER + g->moveSelX*SQUARE_SIZE,
				maxh - BOARD_BORDER - (g->moveSelY+1)*SQUARE_SIZE - POSS_BORDER,
				img_piecePoss.width, img_piecePoss.height,
				img_piecePoss.data
			);
			//selection square
			S2DE_imageRGBA(
				BOARD_BORDER + g->possSelX*SQUARE_SIZE - SELECTION_OFFSET,
				maxh - BOARD_BORDER - (g->possSelY+1)*SQUARE_SIZE - SELECTION_OFFSET,
				img_possSel.width, img_possSel.height,
				img_possSel.data
			);
		}
	}
}
#endif



// ---- console ----
void printfString(square p){
	if(p.owner == VOID){
		printf("__  ");
		return;
	}
	//owner
	char owner;
	if(p.owner == WHITE)
		owner = 'w';
	else
		owner = 'b';
	printf("%c%c  ",owner,getTypeChar(p.type));
}

void console_display(game* g){
	int temp = 0;
	temp+=system("clear");
	printf("############# ECHECKISEN ###########\n");
	printf("#  A   B   C   D   E   F   G   H   #\n");
	//board
	for(int y=0; y < 8; y++){
		printf("#%i ",y);
		//squares
		for(int x=0; x < 8; x++)
			printfString(g->board[y][x]);
		printf("#\n#  ");
		//indicators
		for(int x=0; x < 8; x++){
			//selection
			if(g->mode == MOVE && x == g->moveSelX && y == g->moveSelY)
				printf("~~  ");
			else if(g->mode == POSSIBILITIES && x == g->possSelX && y == g->possSelY)
				printf("/\\  ");
			//possibilities
			else if(g->board[y][x].able)
				printf("--  ");
			else
				printf("    ");
		}
		printf("#\n");
	}
	//informations
	//white
	int white_name_len = strlen(g->p_white.name);
	printf("####################################\n");
	if(white_name_len > 24)
		printf("# WHITE : <Name too long>          #\n");
	else{
		printf("# WHITE : %s",g->p_white.name);
		for(int a=0; a < 24-white_name_len; a++)
			printf(" ");
		printf(" #\n");
	}
	if(g->p_white.checked)
		printf("# /!\\ WARNING Checked !            #\n");
	else
		printf("#                                  #\n");
	printf("# time spent : %s   turn : %s #\n#  ",
		getTimeStr(g->p_white.timeSpent), getNbrStr(g->p_white.turnCnt)
	);
	//dead squares
	for(int a=0; a < 7; a++)
		printfString(g->p_white.dead[a]);
	//black
	int black_name_len = strlen(g->p_black.name);
	printf("    #\n####################################\n");
	if(black_name_len > 24)
		printf("# BLACK : <Name too long>          #\n");
	else{
		printf("# BLACK : %s",g->p_black.name);
		for(int a=0; a < 24-black_name_len; a++)
			printf(" ");
		printf(" #\n");
	}
	if(g->p_black.checked)
		printf("# /!\\ WARNING Checked !            #\n");
	else
		printf("#                                  #\n");
	printf("# time spent : %s   turn : %s #\n#  ",
		getTimeStr(g->p_black.timeSpent), getNbrStr(g->p_black.turnCnt)
	);
	//dead squares
	for(int a=0; a < 7; a++)
		printfString(g->p_black.dead[a]);
	printf("    #\n####################################\n");
	//game
	char owner;
	if(g->turn == WHITE)
		owner = 'w';
	else
		owner = 'b';
	printf("#               GAME               #\n");
	printf("#                                  #\n");
	printf("#                                  #\n");
	printf("# turn : %c    next turn in : %s #\n",
		owner, getTimeStr(MAX_SECONDS_PER_TURN - g->currentTurn_seconds)
	);
	printf("# time : %s     turn nbr : %s #\n",
		getTimeStr(g->game_seconds), getNbrStr(g->turnCnt)
	);
	printf("####################################\n");
	printf("%s\n", g->game_info);
}



// ---- both ----
void refreshScreens(game* g){
	console_display(g);
	#ifdef GRAPHICS_ON
	S2DE_refresh();
	#endif
}






// ====================== CONTROL ==========================

// ---- initialisation ----
void setBasicValues(game* g){
	int temp=0;
	//init board
	for(int y=0; y < 8; y++){
		for(int x=0; x < 8; x++){
			g->board[y][x].owner = VOID;
			g->board[y][x].type = PAWN;
			g->board[y][x].x = x;
			g->board[y][x].y = y;
			g->board[y][x].able = 0;
		}
	}
	//white
	g->board[0][0] = setBoard(0,0,WHITE,ROOK  );
	g->board[0][1] = setBoard(0,1,WHITE,KNIGHT);
	g->board[0][2] = setBoard(0,2,WHITE,BISHOP);
	g->board[0][3] = setBoard(0,3,WHITE,KING  );
	g->board[0][4] = setBoard(0,4,WHITE,QUEEN );
	g->board[0][5] = setBoard(0,5,WHITE,BISHOP);
	g->board[0][6] = setBoard(0,6,WHITE,KNIGHT);
	g->board[0][7] = setBoard(0,7,WHITE,ROOK  );
	//black
	g->board[7][0] = setBoard(7,0,BLACK,ROOK  );
	g->board[7][1] = setBoard(7,1,BLACK,KNIGHT);
	g->board[7][2] = setBoard(7,2,BLACK,BISHOP);
	g->board[7][3] = setBoard(7,3,BLACK,KING  );
	g->board[7][4] = setBoard(7,4,BLACK,QUEEN );
	g->board[7][5] = setBoard(7,5,BLACK,BISHOP);
	g->board[7][6] = setBoard(7,6,BLACK,KNIGHT);
	g->board[7][7] = setBoard(7,7,BLACK,ROOK  );
	//pawns
	for(int x=0; x < 8; x++){
		g->board[1][x] = setBoard(1,x,WHITE,PAWN);
		g->board[6][x] = setBoard(6,x,BLACK,PAWN);
	}
	//init players
	//white
	printf("Give a name for player WHITE > \n");
	g->p_white.name = malloc(MAX_NAME_LENGTH);
	temp+=scanf("%s",g->p_white.name);
	g->p_white.timeSpent = 0;
	g->p_white.turnCnt = 0;
	g->p_white.checked = 0;
	g->p_white.castling = 0;
	g->p_white.kingMoved = 0;
	for(int a=0; a < 7; a++){
		g->p_white.dead[a].owner = VOID;
		g->p_white.dead[a].type = PAWN;
		g->p_white.dead[a].x = 0;
		g->p_white.dead[a].y = 0;
		g->p_white.dead[a].able = 0;
	}
	//black
	printf("Give a name for player BLACK > \n");
	g->p_black.name = malloc(MAX_NAME_LENGTH);
	temp+=scanf("%s",g->p_black.name);
	g->p_black.timeSpent = 0;
	g->p_black.turnCnt = 0;
	g->p_black.checked = 0;
	g->p_black.castling = 0;
	g->p_black.kingMoved = 0;
	for(int a=0; a < 7; a++){
		g->p_black.dead[a].owner = VOID;
		g->p_black.dead[a].type = PAWN;
		g->p_black.dead[a].x = 0;
		g->p_black.dead[a].y = 0;
		g->p_black.dead[a].able = 0;
	}
	//game
	g->win_mode = 0;
	g->winner = VOID;
	g->mode = MOVE;
	g->turn = WHITE;
	g->turnCnt = 0;
	g->moveSelX = 3; g->moveSelY = 0;
	g->possSelX = 0; g->possSelY = 0;
	g->saveCnt = 0;
	g->saveState = 0;
	g->game_info[0] = '\0';
}

void globalInitialization(game* g){
	#ifdef GRAPHICS_ON
	setSprites();
	#endif
	g->game_info = malloc(GAME_INFO_LEN);
	setBasicValues(g);
	g->saveState = loadGame(g);
	if(g->saveState)
		printf("Save file loaded !\n\n");
	else
		printf("Unable to load save file.\n");
	//init timer
	g->currentTurn_seconds = 0;
	#ifdef GRAPHICS_ON
	S2DE_setTimer(1075); //<=> 1 fps
	#endif
}



// ---- IHM ----
void keyboard_special(game* g, char key){
	if(!g->win_mode){
		switch(key){
			case 'v': //go down
				if(g->mode == MOVE){
					if(g->moveSelY < 7)
						g->moveSelY++;
				}else{
					if(g->possSelY < 7)
						g->possSelY++;
				}
				refreshScreens(g);
			break;
			case '^': //go up
				if(g->mode == MOVE){
					if(g->moveSelY > 0)
						g->moveSelY--;
				}else{
					if(g->possSelY > 0)
						g->possSelY--;
				}
				refreshScreens(g);
			break;
			case '<': //go left
				if(g->mode == MOVE){
					if(g->moveSelX > 0)
						g->moveSelX--;
				}else{
					if(g->possSelX > 0)
						g->possSelX--;
				}
				refreshScreens(g);
			break;
			case '>': //go right
				if(g->mode == MOVE){
					if(g->moveSelX < 7)
						g->moveSelX++;
				}else{
					if(g->possSelX < 7)
						g->possSelX++;
				}
				refreshScreens(g);
			break;
		}
	}
}

void keyboard_normal(game* g, char key){
	switch(key){
		//save
		case 'S':
		case 's':
			if(!g->win_mode && g->saveCnt == 0){
				g->saveCnt = MAX_SAVECNT;
				g->saveState = saveGame(g);
			}
		break;
		//interract
		case ' ':
			if(!g->win_mode){
				if(g->mode == MOVE){
					//move mode
					if(g->board[g->moveSelY][g->moveSelX].owner == g->turn){ //if on one of our squares
						setPossOnBoard(g,g->moveSelX,g->moveSelY);
						g->mode = POSSIBILITIES;
						g->possSelX = g->moveSelX;
						g->possSelY = g->moveSelY;
					}
				}else{
					//possibilities mode
					if(g->possSelX == g->moveSelX && g->possSelY == g->moveSelY){ //if on the current square
						g->mode = MOVE;
						resetPossOnBoard(g);
						return;
					}else if(g->board[g->possSelY][g->possSelX].able){ //if on an "allowed for move" square
						//if detected an invalid "allowed for move" square
						if(g->board[g->possSelY][g->possSelX].owner == g->turn){
							printf("ERROR : Control.c : keyboard() : Something went wrong in setPossOnBoard() concerning square at (%i,%i)\n",
								g->board[g->possSelY][g->possSelX].x, g->board[g->possSelY][g->possSelX].y
							);
							exit(EXIT_FAILURE);
						}
						//if on enemy square
						if(g->board[g->possSelY][g->possSelX].owner != VOID){
							//kill the king => fin de jeu
							if(g->board[g->possSelY][g->possSelX].type == KING){
								g->win_mode = 1;
								if(g->turn == WHITE)
									g->winner = WHITE;
								else
									g->winner = BLACK;
								return;
							}
						}
						//castling detection
						if(g->board[g->moveSelY][g->moveSelX].type == KING){
							if(
								(g->board[g->moveSelY][g->moveSelX].owner == WHITE && !g->p_white.kingMoved) ||
								(g->board[g->moveSelY][g->moveSelX].owner == BLACK && !g->p_black.kingMoved)
							){
								if(g->moveSelX+2 == g->possSelX){
									if(g->board[g->moveSelY][g->moveSelX].owner == WHITE){
										moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
									   	moveSquare(g,7,0,4,0);
									   	g->p_white.castling = 1;
									}else{
										moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
									   	moveSquare(g,7,7,4,7);
									   	g->p_black.castling = 1;
									}
								}else if(g->moveSelX == g->possSelX+2){
									if(g->board[g->moveSelY][g->moveSelX].owner == WHITE){
										moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
										moveSquare(g,0,0,2,0);
									   	g->p_white.castling = 1;
									}else{
										moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
										moveSquare(g,0,7,2,7);
									   	g->p_black.castling = 1;
									}
								}else
									moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
								finishTurn(g);
								refreshScreens(g);
								return;
							}
						}
						//in all cases
						moveSquare(g,g->moveSelX,g->moveSelY, g->possSelX,g->possSelY);
						finishTurn(g);
						refreshScreens(g);
					}
				}
			}
		break;
		//replay
		case 'R':
		case 'r':
			//replay
			setBasicValues(g);
			g->win_mode = 0;
		break;
		//draw
		case 'D':
		case 'd':
			g->win_mode = 1;
			g->winner = VOID;
		break;
	}
}



// ---- time ----
void timedExecution(game* g){
	//delay to show save state
	if(g->saveCnt > 0)
		g->saveCnt--;
	g->game_seconds++;
	//player counters
	if(g->turn == WHITE)
		g->p_white.timeSpent++;
	else
		g->p_black.timeSpent++;
	//if game is too long
	if(g->game_seconds >= MAX_GAME_TIME){
		//draw
		g->win_mode = 1;
		g->winner = VOID;
		return;
	}
	//time left
	g->currentTurn_seconds++;
	if(g->currentTurn_seconds >= MAX_SECONDS_PER_TURN)
		finishTurn(g);
	refreshScreens(g);
}






// ====================== MAIN CODE ========================

// ---- events ----
#ifdef GRAPHICS_ON
static game graphic_game;
void startMainLoop(){
	globalInitialization(&graphic_game);
	S2DE_start();
}
void S2DE_event(int event){
	switch(event){
		case S2DE_TIMER:
			timedExecution(&graphic_game);
		break;
		case S2DE_DISPLAY:
			graphic_display(&graphic_game);
		break;
		case S2DE_KEYBOARD:
			if(S2DE_keyState == S2DE_KEY_RELEASED) //skip "key released" events
				break;
			switch(S2DE_key){
				case S2DE_KEY_UP:
					keyboard_special(&graphic_game,'^');
				break;
				case S2DE_KEY_DOWN:
					keyboard_special(&graphic_game,'v');
				break;
				case S2DE_KEY_LEFT:
					keyboard_special(&graphic_game,'<');
				break;
				case S2DE_KEY_RIGHT:
					keyboard_special(&graphic_game,'>');
				break;
				case S2DE_KEY_D:
				case S2DE_KEY_d:
					keyboard_normal(&graphic_game,'d');
				break;
				case S2DE_KEY_R:
				case S2DE_KEY_r:
					keyboard_normal(&graphic_game,'r');
				break;
				case S2DE_KEY_S:
				case S2DE_KEY_s:
					keyboard_normal(&graphic_game,'s');
				break;
				case S2DE_KEY_SPACE:
					keyboard_normal(&graphic_game,' ');
				break;
			}
		break;
	}
}
#else
static game console_game;
void* console_timeLoop(void* g){
	while(1){
		if(threadOK)
			timedExecution((game*)g);
		usleep(1000000);
	}
	return NULL;
}

void console_gestionEvenements(char cmd, game* g){
	switch(cmd){
		case 'R':
		case 'r':
			keyboard_normal(g,'r');
		break;
		case 'S':
		case 's':
			keyboard_normal(g,'s');
		break;
		case 'A':
		case 'a':
			keyboard_normal(g,' ');
		break;
		case 'O':
		case 'o':
			keyboard_special(g,'^');
		break;
		case 'L':
		case 'l':
			keyboard_special(g,'v');
		break;
		case 'K':
		case 'k':
			keyboard_special(g,'<');
		break;
		case 'M':
		case 'm':
			keyboard_special(g,'>');
		break;
	}
}

void startMainLoop(){
	//time management
	threadOK = 0;
	pthread_t timeLooper;
	pthread_create(&timeLooper,NULL,console_timeLoop,(void*)&console_game);
	//init
	globalInitialization(&console_game);
	refreshScreens(&console_game);
	//event loop
	int temp=0;
	char cmd;
	while(1){
		threadOK = 1;
		cmd = '_';
		temp+=scanf(" %c",&cmd); //get user input
		threadOK = 0;
		usleep(1000); //to be sure that timedExecution has finished executing
		console_gestionEvenements(cmd,&console_game);
		refreshScreens(&console_game);
	}
}
#endif



// ---- launch game ----
int main(int argc, char **argv){
	#ifdef GRAPHICS_ON
	S2DE_init(argc, argv, "Echeckisen [0.1.4]", maxl, maxh);
	#endif
	startMainLoop();
	return EXIT_SUCCESS;
}
