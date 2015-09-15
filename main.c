#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "myLib.h"

int home();
void help();
int game();
void win();
void lose();

//int gameCompleted = 0;

enum {HOME, HELP, GAME, WIN, LOSE};
int state;
int level = 1;
int special = 0;
int specialallowed = 0;

int main() 
{
	state = HOME;
	while(1)
	{
	    if(state == HOME) 
	    {
	        int start = home();
	        if(start == 1)
	        {
	            state = GAME;
	        }
	        else if(start == 0)
	        {
	            state = HELP;
	        }
	        else
	        {
	            special = 1;
	            state = GAME;
	        }
	    }
	    else if(state == HELP)
	    {
	        help();
	        state = HOME;
	    }
	    else if(state == GAME)
	    {
	        int outcome = game();
	        if(outcome == 0)
	        {
	            state = LOSE;
	        }
	        else if(outcome > 0)
	        {
	            state = WIN;
	        }
	        else
	        {
	            state = HOME;
	        }
	    }
	    else if(state == WIN)
	    {
	        win();
	        state = HOME;
	    }
	    else if(state == LOSE)
	    {
	        lose();
	        state = HOME;
	    }
	    else
	    {
	        state = HOME;
	    }
	}
}

int home()
{
    REG_DISPCNT = MODE3 | BG2_ENABLE;
    fillScreen(BLACK);
    drawImage3(20, 30, HOMEPAGE_WIDTH, HOMEPAGE_HEIGHT, homepage);
    Choice start = (Choice) {
        .row = 100,
        .col = 105,
        .str = "START",
        .index = 1
    };
    Choice help = (Choice) {
        .row = 115,
        .col = 96,
        .str = "CONTROLS",
        .index = 0
    };
    Choice specialG = (Choice) {
        .row = 130,
        .col = 99,
        .str = "SPECIAL",
        .index = -1
    };
    Choice curChoice = start;
    while(1)
    {
	    if(KEY_DOWN_NOW(BUTTON_DOWN) && curChoice.index == 1)
	    {
	        curChoice = help;
	        while(!KEY_DOWN_NOW(BUTTON_DOWN));
	        while(KEY_DOWN_NOW(BUTTON_DOWN));
	    }
	    if(KEY_DOWN_NOW(BUTTON_UP) && curChoice.index == 0)
	    {
	        curChoice = start;
	        while(!KEY_DOWN_NOW(BUTTON_UP));
	        while(KEY_DOWN_NOW(BUTTON_UP));
	    }
	    if(KEY_DOWN_NOW(BUTTON_DOWN) && curChoice.index == 0 && specialallowed)
	    {
	        curChoice = specialG;
	        while(!KEY_DOWN_NOW(BUTTON_DOWN));
	        while(KEY_DOWN_NOW(BUTTON_DOWN));
	    }
	    if(KEY_DOWN_NOW(BUTTON_UP) && curChoice.index == -1)
	    {
	        curChoice = help;
	        while(!KEY_DOWN_NOW(BUTTON_UP));
	        while(KEY_DOWN_NOW(BUTTON_UP));
	    }
	    if(KEY_DOWN_NOW(BUTTON_A) | KEY_DOWN_NOW(BUTTON_START))
	    {
	        return curChoice.index;
	    }
	    waitForVblank();
	    drawChoice(start, BLACK, BLOOD);
        drawChoice(help, BLACK, BLOOD);
        if(specialallowed)
        {
            drawChoice(specialG, BLACK, BLOOD);
        } 
	    drawChooser(curChoice);
	}
}

void help()
{
    fillScreen(BLACK);
    drawHollowRect(0,0,239,159,BLUE);
    drawHollowRect(1,1,238,158,BLUE);
    drawString(42, 96, "CONTROLS", YELLOW);
    drawRect(52,20,200,4,YELLOW);
    drawRect(52,20,4,68,YELLOW);
    drawRect(116,20,200,4,YELLOW);
    drawRect(52,220,4,68,YELLOW);
    char list_analog[] = {(char)24,(char)25,(char)26,(char)27,' ',':',' ','m','o','v','e','\0'};
    drawString(64,32, list_analog, YELLOW);
    drawString(100,32, "SELECT : back to menu", YELLOW);
    drawString(150,134, "Press B to return", YELLOW);
    while(!KEY_DOWN_NOW(BUTTON_B));
    while(KEY_DOWN_NOW(BUTTON_B));
}

int game()
{
    typedef struct
    {
       int row;
       int col;
       u16 color; 
    } movingPiece;
    
    typedef struct
    {
        int row;
        int col;
        int rd;
        int cd;
        int color;
        
    } BALL;
    
    BALL balls[level];
    BALL oldballs[level];
    int ballsize = 7;
    BALL *cur;
    movingPiece piece = {
        .row = 0,
        .col = 0,
        .color = SEAFOAM
    };
    movingPiece oldpiece = piece;
    fillScreen(BLACK);
    int dels[] = {-2, -3, 1, 3, -3,2, -1, -2, 3, 2};
	int numdels = sizeof(dels)/sizeof(dels[0]);
    int i = 0;
    int safe = 0;
    unsigned short colors[] = {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, LAVENDER, SEAFOAM, APRICOT, PINK};
    int numcols = sizeof(colors)/sizeof(colors[0]);
    for(i=0; i<level; i++)
	{
		balls[i].row = rand()%25 + 70;
		balls[i].col = rand()%25 + 110;
		balls[i].rd = dels[rand()% numdels];
		balls[i].cd = dels[rand()% numdels];
		balls[i].color = colors[i%numcols];
		oldballs[i] = balls[i];
	}
	
	int lives = 7;
	char lifestring[10];
	char timer[10];

	REG_TM2CNT = 0; // Turn off timer 2
    REG_TM3CNT = 0; // Turn off timer 3
    REG_TM2D = -0x4000; // 1 sec*
    REG_TM2CNT = TM_FREQ_1024 | TM_ON;
    REG_TM3CNT = TM_CASCADE | TM_ON;
    int second;
    char strlvl[10];
    sprintf(strlvl, "LEVEL: %d", level);
    drawString(74, 100, strlvl, SKYBLUE);
    drawString(151,170, "Press START", SKYBLUE);
    while(KEY_DOWN_NOW(BUTTON_START));
    while(!KEY_DOWN_NOW(BUTTON_START));
    while(KEY_DOWN_NOW(BUTTON_START));
    drawRect(74, 100, 64, 8, BLACK);    
    while(1)
	{
	    if(KEY_DOWN_NOW(BUTTON_UP)) {
			if(piece.row<=0) {
				piece.row = 0;
			} else if(!special) {
				piece.row--;
			} else {
			    piece.row -= 2;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_DOWN)) {
			if(piece.row>=149-ballsize+1) {
				piece.row = 149-ballsize+1;
			} else if(!special) {
				piece.row++;
			} else {
			    piece.row += 2;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_LEFT)) {
			if(piece.col<=0) {
				piece.col = 0;
			} else if(!special) {
				piece.col--;
			} else {
			    piece.col -= 2;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
			if(piece.col>=239-ballsize+1) {
				piece.col = 239-ballsize+1;
			} else if(!special) {
				piece.col++;
			} else {
			    piece.col += 2;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_SELECT))
        {
            level = 1;
            return -1;
        }
        safe = 0;
        if((piece.col+3)-4 < 7 && (piece.col+3)-4 > -7 && (piece.row+3)-4 < 7 && (piece.row+3)-4 > -7)
	    {
	        safe = 1;
	    }
	    for(i=0; i<level; i++)
	    {	
		    //cur = &objs[i];
		    cur = balls + i;
		    if(!special)
		    {
		        cur->row += cur->rd;
		        cur->col += cur->cd;
		    }
		    else
		    {
		        cur->row += cur->rd + rand()%1;
		        cur->col += cur->cd + rand()%1;
		    }
		    if(cur->row<0)
		    {
			    cur->row = 0;
			    cur->rd = -cur->rd;
		    }
		    if(cur->row>149-ballsize+1)
		    {
			    cur->row = 149-ballsize+1;
			    cur->rd = -cur->rd;
		    }
		    if(cur->col<0)
		    {
			    cur->col = 0;
			    cur->cd = -cur->cd;
		    }
		    if(cur->col>239-ballsize+1)
		    {
			    cur->col = 239-ballsize+1;
			    cur->cd = -cur->cd;
		    }
		    if((((cur->col)+3)-(piece.col+3)) < 5 && (((cur->col)+3)-(piece.col+3)) > -5 && (((cur->row)+3)-(piece.row+3)) < 5 && (((cur->row)+3)-(piece.row+3)) > -5 && !safe) 
		    {
		        lives--;
		        piece.row = 0;
		        piece.col = 0;
		    }
	    }
	    if(234-(piece.col+3) < 7 && (234-(piece.col+3)) > -7 && (144-(piece.row+3)) < 7 && (144-(piece.row+3)) > -7)
	    {
	        if(level == 10)
	        {
	            return 1;
	        }
	        level++;
	        return game();
	    }
	    sprintf(lifestring, "LIVES: %d", lives);
        second = REG_TM3D;
        if(20-second == 0 || lives == 0)
        {
            return 0;
        }
        sprintf(timer, "TIME: %d", 20-second);
	    waitForVblank();
	    drawRect(0,0,9,9,LIME);
	    drawRect(140,230,9,9,RED);
		for(i=0; i<level; i++)
		{
			drawRect(oldballs[i].row, oldballs[i].col, ballsize, ballsize, BLACK);
		}
		drawRect(151,6,232,8,COOLGREY);
		for(i=0; i<level; i++)
		{
			cur = balls + i;
			drawRect(cur->row, cur->col, ballsize, ballsize, cur->color);
			oldballs[i] = balls[i];
		}
		drawRect(oldpiece.row, oldpiece.col, ballsize, ballsize, BLACK);
		drawRect(piece.row, piece.col, ballsize, ballsize, SEAFOAM);
		    drawString(151, 100, strlvl, SKYBLUE);
	    drawString(151,180,timer,SKYBLUE);
        drawString(151,6,lifestring,SKYBLUE);
        oldpiece = piece;
	}
}

void win()
{
    fillScreen(BLACK);
    fillBackground(winner);
    drawString(20,110, "Press START to return", BLACK);
    level = 1;
	specialallowed = 1;
    while(!KEY_DOWN_NOW(BUTTON_START));
    while(KEY_DOWN_NOW(BUTTON_START));
}

void lose()
{
    fillScreen(BLACK);
    fillBackground(loser);
    drawString(10,20, "You sir, are a sore loser.",MAGENTA);
    drawString(30,20, "BAZINGA!",MAGENTA);
    drawString(140,110, "Press START to return", BROWN);
    level = 1;
    while(!KEY_DOWN_NOW(BUTTON_START));
    while(KEY_DOWN_NOW(BUTTON_START));
}
