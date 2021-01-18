#ifndef __UTILITIES_H__
#define __UTILITIES_H__












// ==================== DEFINITIONS =======================

// ---- squares ----
enum square_owner{
	VOID = 0,
	WHITE,
	BLACK
};
enum square_type{
	PAWN = 0,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING
};
struct SQUARE{
	enum square_owner owner;
	enum square_type type;
	int x,y;
	int able; //useful to know which place is enable for the move of the current player
	//(able == 0) => nothing
	//(able == 1) => possibility to move current piece on this square
};
typedef struct SQUARE square;

// ---- player ----
struct PLAYER{
	char* name;
	int timeSpent, turnCnt;
	int checked;
	square dead[7];
	int castling;
	int kingMoved;
};
typedef struct PLAYER player;

// ---- mode ----
enum gameMode{
	MOVE=0,
	POSSIBILITIES
};

// ---- game ----
struct GAME{
	// ---- game ----
	square board[8][8];
	enum gameMode mode;
	enum square_owner turn;
	int turnCnt;
	int win_mode; //0:in_game, 1:in_win_mode
	enum square_owner winner;
	char* game_info;

	// ---- players ----
	player p_white;
	player p_black;

	// ---- time management ----
	int game_seconds;
	int currentTurn_seconds;

	// ---- selection ----
	int moveSelX; //move
	int moveSelY;
	int possSelX; //possibilities
	int possSelY;

	// ---- save system ----
	int saveState;
	int saveCnt;
};
typedef struct GAME game;












// ===================== UTILITIES ========================

// ---- useful ----
square setBoard(int y, int x, enum square_owner, enum square_type);
square getKing(game* g, enum square_owner);
void resetSelection(game* g);
void finishTurn(game* g);
char getTypeChar(enum square_type t);
void moveSquare(game* g, int x1, int y1, int x2, int y2);
char* getTimeStr(int seconds);
char* getNbrStr(int nbr);
void checkForChecked(game* g);
int existsDead(game* g, enum square_owner);
void addDead(game* g, square p);
void rmvDead(game* g, enum square_owner o, int x, int y);

// ---- basic possibilities set ----
void setBasicPoss_pawn(game* g, int x, int y);
void setBasicPoss_rook(game* g, int x, int y);
void setBasicPoss_knight(game* g, int x, int y);
void setBasicPoss_bishop(game* g, int x, int y);
void setBasicPoss_king(game* g, int x, int y);

// ---- special possibilities set ----
void setCastlingPoss(game* g);

// ---- calculations ----
int trySetPossOnBoard(game* g, int x, int y);
void setPossOnBoard(game* g, int x, int y);
void resetPossOnBoard(game* g);












#endif
