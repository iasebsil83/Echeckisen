//standard
#include <stdlib.h>
#include <stdio.h>
//game
#include "Utilities.h"






// ===================== UTILITIES ========================

// ---- useful ----
square setBoard(int y, int x, enum square_owner o, enum square_type t){
	square s;
	s.owner = o; s.type = t;
	s.x = x;     s.y = y;
	s.able = 0;
	return s;
}

square getKing(game* g, enum square_owner o){
	for(int y=0; y < 8; y++){
		for(int x=0; x < 8; x++){
			if(g->board[y][x].owner == o && g->board[y][x].type == KING)
				return g->board[y][x];
		}
	}
	if(o == WHITE)
		printf("ERROR : Utilities.c : getKing() : Unable to find white king\n");
	else if(o == BLACK)
		printf("ERROR : Utilities.c : getKing() : Unable to find black king\n");
	exit(EXIT_FAILURE);
}

void resetSelection(game* g){
	square k = getKing(g,g->turn);
	g->mode = MOVE;
	g->moveSelX = k.x;
	g->moveSelY = k.y;
}

void finishTurn(game* g){
	g->currentTurn_seconds = 0;
	resetPossOnBoard(g);
	checkForChecked(g);
	g->turnCnt++;
	if(g->turn == BLACK){
		g->p_black.turnCnt++;
		g->turn = WHITE;
	}else if(g->turn == WHITE){
		g->p_white.turnCnt++;
		g->turn = BLACK;
	}
	g->mode = MOVE;
	resetSelection(g);
}

char getTypeChar(enum square_type t){
	switch(t){
		case PAWN:
			return 'p';
		case ROOK:
			return 'r';
		case KNIGHT:
			return 'k';
		case BISHOP:
			return 'b';
		case QUEEN:
			return 'q';
		case KING:
			return 'K';
	}
	return ' ';
}

void moveSquare(game* g, int x1, int y1, int x2, int y2){
	//kill the piece at x2,y2
	int kill = 0;
	enum square_type optype;
	if(g->board[y2][x2].owner != VOID){
		optype = g->board[y2][x2].type;
		addDead(g,g->board[y2][x2]);
		kill = 1;
	}
	//set the destination case
	g->board[y2][x2].owner = g->board[y1][x1].owner;
	g->board[y2][x2].type  = g->board[y1][x1].type;
	//set info message
	char letters[8] = "ABCDEFGH";
	char owner;
	if(g->board[y1][x1].owner == WHITE)
		owner = 'w';
	else
		owner = 'b';
	if(kill){
		char opOwner = ' ';
		if(owner == WHITE)
			opOwner = 'b';
		else
			opOwner = 'w';
		sprintf(
			g->game_info,"%c%c moves from %c%i to %c%i, eating %c%c",
			owner, getTypeChar(g->board[y1][x1].type),
			letters[x1], (int)(y1+1),
			letters[x2], (int)(y2+1),
			opOwner, getTypeChar(optype)
		);
	}else
		sprintf(
			g->game_info,"%c%c moves from %c%i to %c%i",
			owner, getTypeChar(g->board[y1][x1].type),
			letters[x1], (int)(y1+1),
			letters[x2], (int)(y2+1)
		);
	//set first king turn
	if(g->board[y1][x1].type == KING){
		if(owner == 'w')
			g->p_white.kingMoved = 1;
		else
			g->p_black.kingMoved = 1;
	}
	//reset the start case
	g->board[y1][x1].owner = VOID;
	//transformation
	if(g->board[y2][x2].type == PAWN && (y2 == 0 || y2 == 7)){
		if(existsDead(g,g->board[y2][x2].owner))
			rmvDead(g,g->board[y2][x2].owner,x2,y2);
	}
}

char* getTimeStr(int seconds){
	int min = seconds/60;
	int sec = seconds-min*60;
	char* text = malloc(24);
	if(min > 9){
		if(sec > 9)
			sprintf(text,"%i:%i",min,sec);
		else
			sprintf(text,"%i:0%i",min,sec);
	}else{
		if(sec > 9)
			sprintf(text,"0%i:%i",min,sec);
		else
			sprintf(text,"0%i:0%i",min,sec);
	}
	return text;
}

char* getNbrStr(int nbr){
	if(nbr > 9999)
		return "  OF";
	char* str = malloc(15);
	if(nbr > 999){
		sprintf(str,"%i",nbr);
		return str;
	}
	if(nbr > 99){
		sprintf(str," %i",nbr);
		return str;
	}
	if(nbr > 9){
		sprintf(str,"  %i",nbr);
		return str;
	}
	sprintf(str,"   %i",nbr);
	return str;
}

void checkForChecked(game* g){
	if(g->turn == VOID){
		printf("ERROR : Utilities.c : checkForChecked() : Incorrect turn value \"VOID\"\n");
		exit(EXIT_FAILURE);
	}
	if(g->turn == BLACK){
		g->p_white.checked = 0;
		square k = getKing(g,WHITE);
		//set all possibilities of enemy pawns
		for(int y=0; y < 8; y++){
			for(int x=0; x < 8; x++){
				if(g->board[y][x].owner == VOID)
					continue;
				if(g->board[y][x].owner == BLACK)
					setPossOnBoard(g,x,y);
			}
		}
		//set checked
		if(g->board[k.y][k.x].able)
			g->p_white.checked = 1;
		/*
		if(g->p_white.checked){
			//set mat
			int mat = 1; //mat fixed
			int kcoo[8][2] = {
				{k.x-1, k.y-1},{k.x,   k.y-1},
				{k.x+1, k.y-1},{k.x+1, k.y  },
				{k.x+1, k.y+1},{k.x,   k.y+1},
				{k.x-1, k.y+1},{k.x-1, k.y  }
			};
			//try to get out of mat
			for(int a=0; a < 8; a++){
				if(kcoo[a][0] < 0 || kcoo[a][0] > 7 || kcoo[a][1] < 0 || kcoo[a][1] > 7)
					continue;
				if(g->board[kcoo[a][1]][kcoo[a][0]].owner != WHITE && !g->board[kcoo[a][1]][kcoo[a][0]].able)
					mat = 0;
			}
			if(mat){
				g->win_mode = 1;
				g->winner = BLACK;
			}
		}
		*/
	}else{
		g->p_black.checked = 0;
		square k = getKing(g,BLACK);
		//set all possibilities of enemy pawns
		for(int y=0; y < 8; y++){
			for(int x=0; x < 8; x++){
				if(g->board[y][x].owner == VOID)
					continue;
				if(g->board[y][x].owner == WHITE)
					setPossOnBoard(g,x,y);
			}
		}
		//set checked
		if(g->board[k.y][k.x].able)
			g->p_black.checked = 1;
		/*
		if(g->p_black.checked){
			//set mat
			int mat = 1; //mat fixed
			int kcoo[8][2] = {
				{k.x-1, k.y-1},{k.x,   k.y-1},
				{k.x+1, k.y-1},{k.x+1, k.y  },
				{k.x+1, k.y+1},{k.x,   k.y+1},
				{k.x-1, k.y+1},{k.x-1, k.y  }
			};
			//try to get out of mat
			for(int a=0; a < 8; a++){
				if(kcoo[a][0] < 0 || kcoo[a][0] > 7 || kcoo[a][1] < 0 || kcoo[a][1] > 7)
					continue;
				if(g->board[kcoo[a][1]][kcoo[a][0]].owner != BLACK && !g->board[kcoo[a][1]][kcoo[a][0]].able)
					mat = 0;
			}
			if(mat){
				g->win_mode = 1;
				g->winner = WHITE;
			}
		}
		*/
	}
	resetPossOnBoard(g);
}

int existsDead(game* g, enum square_owner o){
	for(int a=0; a < 7; a++){
		if(o == WHITE){
			if(g->p_white.dead[a].owner != VOID)
				return 1;
		}else{
			if(g->p_black.dead[a].owner != VOID)
				return 1;
		}
	}
	return 0;
}

void addDead(game* g, square p){
	//don't store the pawns
	if(p.type == PAWN)
		return;
	if(p.owner == VOID){
		printf("ERROR : Utilities.c : addDead() : Can\'t add dead piece to player \"VOID\"\n");
		exit(EXIT_FAILURE);
	}
	//asking for promoted PAWNS
	if(p.owner == WHITE){
		for(int x=0; x < 8; x++){
			if(g->board[7][x].owner == WHITE && g->board[7][x].type == PAWN){
				g->board[7][x].type = p.type;
				char letters[8] = "ABCDEFGH";
				sprintf(
					g->game_info,"wp at %c8 is promoted to w%c",
					letters[x],
					getTypeChar(p.type)
				);
				return;
			}
		}
	}else{
		for(int x=0; x < 8; x++){
			if(g->board[0][x].owner == BLACK && g->board[0][x].type == PAWN){
				g->board[0][x].type = p.type;
				char letters[8] = "ABCDEFGH";
				sprintf(
					g->game_info,"bp at %c1 is promoted to b%c",
					letters[x],
					getTypeChar(p.type)
				);
				return;
			}
		}
	}
	//adding
	for(int a=0; a < 7; a++){
		if(p.owner == WHITE){
			if(g->p_white.dead[a].owner == VOID){
				g->p_white.dead[a].owner = WHITE;
				g->p_white.dead[a].type  = p.type;
				return;
			}
		}else{
			if(g->p_black.dead[a].owner == VOID){
				g->p_black.dead[a].owner = BLACK;
				g->p_black.dead[a].type  = p.type;
				return;
			}
		}
	}
	printf("ERROR : Utilities.c : addDead() : cemetery is full\n");
	exit(EXIT_FAILURE);
}

void rmvDead(game* g, enum square_owner o, int x, int y){
	if(o == VOID){
		printf("ERROR : Utilities.c : rmvDead() : Can\'t remove dead piece from player \"VOID\"\n");
		exit(EXIT_FAILURE);
	}
	//selection loop
	int row,temp;
	while(1){
		if(o == WHITE)
			printf("Select one of your dead piece %s :\n",g->p_white.name);
		else
			printf("Select one of your dead piece %s :\n",g->p_black.name);
		row = 0;
		temp = scanf("%i",&row); //just to shut up compilation warnings
		temp = temp;             //same thing
		//if correct value for row
		if(row >= 0 && row <= 6){
			if(o == WHITE){
				//if occupied row
				if(g->p_white.dead[row].owner != VOID){
					g->board[y][x].type = g->p_white.dead[row].type;
					g->p_white.dead[row].owner = VOID;
					return;
				}
			}else{
				//if occupied row
				if(g->p_black.dead[row].owner != VOID){
					g->board[y][x].type = g->p_black.dead[row].type;
					g->p_black.dead[row].owner = VOID;
					return;
				}
			}
		}
		printf("You can't take this piece\n");
	}
}



// ---- basic possibilities set ----
void setBasicPoss_pawn(game* g, int x, int y){
	if(g->board[y][x].owner == WHITE){
		//double walk
		if(y+2 >= 0 && y+2 <= 7){
			if(y == 1 && g->board[2][x].owner == VOID && g->board[3][x].owner == VOID) //start position for white pawns
				trySetPossOnBoard(g,x,y+2);
		}
		if(y+1 >= 0 && y+1 <= 7){
			//simple walk
			if(g->board[(int)(y+1)][x].owner == VOID)
				trySetPossOnBoard(g,x,y+1);
			//first diagonal
			if(x-1 >= 0 && x-1 <= 7){
				if(g->board[(int)(y+1)][(int)(x-1)].owner == BLACK)
					trySetPossOnBoard(g,x-1,y+1);
			}
			//second diagonal
			if(x+1 >= 0 && x+1 <= 7){
				if(g->board[(int)(y+1)][(int)(x+1)].owner == BLACK)
					trySetPossOnBoard(g,x+1,y+1);
			}
		}
	}else{
		//double walk
		if(y-2 >= 0 && y-2 <= 7){
			if(y == 6 && g->board[5][x].owner == VOID && g->board[4][x].owner == VOID) //start position for black pawns
				trySetPossOnBoard(g,x,y-2);
		}
		if(y-1 >= 0 && y-1 <= 7){
			//simple walk
			if(g->board[(int)(y-1)][x].owner == VOID)
				trySetPossOnBoard(g,x,y-1);
			//first diagonal
			if(x-1 >= 0 && x-1 <= 7){
				if(g->board[(int)(y-1)][(int)(x-1)].owner == WHITE)
					trySetPossOnBoard(g,x-1, y-1);
			}
			//second diagonal
			if(x+1 >= 0 && x+1 <= 7){
				if(g->board[(int)(y-1)][(int)(x+1)].owner == WHITE)
					trySetPossOnBoard(g,x+1,y-1);
			}
		}
	}
}

void setBasicPoss_rook(game* g, int x, int y){
	int poss_set=0;
	int tempX, tempY;
	tempX = x;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX--;
		poss_set = trySetPossOnBoard(g,tempX,y);
		if(!poss_set)
			break;
	}
	tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempY--;
		poss_set = trySetPossOnBoard(g,x,tempY);
		if(!poss_set)
			break;
	}
	tempX = x;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX++;
		poss_set = trySetPossOnBoard(g,tempX,y);
		if(!poss_set)
			break;
	}
	tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempY++;
		poss_set = trySetPossOnBoard(g,x,tempY);
		if(!poss_set)
			break;
	}
}

void setBasicPoss_knight(game* g, int x, int y){
	trySetPossOnBoard(g,x-2,y-1);
	trySetPossOnBoard(g,x-2,y+1);
	trySetPossOnBoard(g,x+2,y-1);
	trySetPossOnBoard(g,x+2,y+1);
	trySetPossOnBoard(g,x-1,y-2);
	trySetPossOnBoard(g,x-1,y+2);
	trySetPossOnBoard(g,x+1,y-2);
	trySetPossOnBoard(g,x+1,y+2);
}

void setBasicPoss_bishop(game* g, int x, int y){
	int poss_set=0;
	int tempX, tempY;
	tempX = x; tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX--; tempY--;
		poss_set = trySetPossOnBoard(g,tempX,tempY);
		if(!poss_set)
			break;
	}
	poss_set=0;
	tempX = x; tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX--; tempY++;
		poss_set = trySetPossOnBoard(g,tempX,tempY);
		if(!poss_set)
			break;
	}
	poss_set=0;
	tempX = x; tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX++; tempY--;
		poss_set = trySetPossOnBoard(g,tempX,tempY);
		if(!poss_set)
			break;
	}
	poss_set=0;
	tempX = x; tempY = y;
	for(int a=0; a < 7; a++){ //in diagonal, maximum is 7 move
		tempX++; tempY++;
		poss_set = trySetPossOnBoard(g,tempX,tempY);
		if(!poss_set)
			break;
	}
}

void setBasicPoss_king(game* g, int x, int y){
	trySetPossOnBoard(g,x-1, y-1);
	trySetPossOnBoard(g,x,   y-1);
	trySetPossOnBoard(g,x+1, y-1);
	trySetPossOnBoard(g,x+1, y  );
	trySetPossOnBoard(g,x+1, y+1);
	trySetPossOnBoard(g,x,   y+1);
	trySetPossOnBoard(g,x-1, y+1);
	trySetPossOnBoard(g,x-1, y  );
}



// ---- special possibilities set ----
void setCastlingPoss(game* g){
	//small castling
	//black
	if(
		g->board[7][3].owner == 2 && g->board[7][3].type == 5 &&
		g->board[7][2].owner == 0 && g->board[7][1].owner == 0 &&
		g->board[7][0].owner == 2 && g->board[7][0].type == 1 &&
		g->turn == BLACK && g->p_black.castling == 0
	){
		g->board[7][1].able = 1;
	}
	//white
	if(
		g->board[0][3].owner == 1 && g->board[0][3].type == 5 &&
	    g->board[0][2].owner == 0 && g->board[0][1].owner == 0 &&
	    g->board[0][0].owner == 1 && g->board[0][0].type == 1 &&
	    g->turn == WHITE && g->p_white.castling == 0
	){
		g->board[0][1].able = 1;
	}

	//large castling
	//black
	if(
		g->board[7][3].owner == 2 && g->board[7][3].type == 5 &&
		g->board[7][4].owner == 0 && g->board[7][5].owner == 0 &&
		g->board[7][6].owner == 0 && g->board[7][7].owner == 2 &&
		g->board[7][7].type == 1 &&
		g->turn == BLACK && g->p_black.castling == 0
	){
		g->board[7][5].able = 1;
	}
	//white
	if(
		g->board[0][3].owner == 1 && g->board[0][3].type == 5 &&
		g->board[0][4].owner == 0 && g->board[0][5].owner == 0 &&
		g->board[0][6].owner == 0 && g->board[0][7].owner == 1 &&
		g->board[0][7].type == 1 &&
		g->turn == WHITE && g->p_white.castling == 0
	){
		g->board[0][5].able = 1;
	}
}



// ---- calculations ----
int trySetPossOnBoard(game* g, int x, int y){
	//if on board coordinates range
	if(x < 0 || x > 7 || y < 0 || y > 7)
		return 0; //possibility unset
	//if not on one of our squares
	if(g->board[y][x].owner == VOID){
		g->board[y][x].able = 1;
		return 1; //possibility set
	}else if(g->board[y][x].owner != g->turn){
		g->board[y][x].able = 1;
		return 0; //possibility unset
	}
	return 0; //posibility unset
}

void setPossOnBoard(game* g, int x, int y){
	switch(g->board[y][x].type){
		case PAWN:
			setBasicPoss_pawn(g,x,y);
		break;
		case ROOK:
			setBasicPoss_rook(g,x,y);
		break;
		case KNIGHT:
			setBasicPoss_knight(g,x,y);
		break;
		case BISHOP:
			setBasicPoss_bishop(g,x,y);
		break;
		case QUEEN:
			setBasicPoss_rook(g,x,y);
			setBasicPoss_bishop(g,x,y);
		break;
		case KING:
			setBasicPoss_king(g,x,y);
			if(
				(g->board[y][x].owner == WHITE && !g->p_white.kingMoved) ||
				(g->board[y][x].owner == BLACK && !g->p_black.kingMoved)
			)
				setCastlingPoss(g);
		break;
	}
}

void resetPossOnBoard(game* g){
	for(int y=0; y < 8; y++)
		for(int x=0; x < 8; x++)
			g->board[y][x].able = 0;
}
