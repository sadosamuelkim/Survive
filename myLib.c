#include <stdlib.h>
#include <string.h>

#include "myLib.h"

u16 *videoBuffer = (u16 *)0x6000000;

void setPixel(int r, int c, u16 color)
{
    *(videoBuffer + (240*r) + c) = color;
}

void drawRect(int row, int col, int width, int height, u16 color)
{
	int r;
	for(r=0; r<height; r++)
	{
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(row+r, col, 240)];
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void drawHollowRect(int r, int c, int width, int height, u16
color)
{
	int i;
	for (i = 0; i < width; i++) {
		setPixel(r, c + i, color);
		setPixel(r + height - 1, c + i, color);
	}
	for (i = 0; i < height; i++) {
		setPixel(r + i, c, color);
		setPixel(r + i, c + width - 1, color);
	}
}

void drawChar(int row, int col, char ch, unsigned short color)
{
	int r,c;
	for(r=0; r<8; r++)
	{
		for(c=0; c<6; c++)
		{
			if(fontdata_6x8[r*6 + c +ch*48])
			{
				setPixel(row+r, col+c, color);
			}
		}
	}
}

void drawString(int row, int col, char *str, unsigned short color)
{
	while(*str)
	{
		drawChar(row, col, *str++, color);
		col += 6;
	}
}

void drawMovingPiece(int r, int c, u16 color)
{
	int i, j, k = 0;
	videoBuffer[240*r + c] = CYAN;
	for (i = 4; i > 0; i--)
	{
		for (j = 1; j < i; j++) {
		    if(k != 0)
		    {
		        if((c+j) < 239) {
			        videoBuffer[240*(r+k) + (c+j)] = color;
			        videoBuffer[240*(r-k) + (c+j)] = color;
			    }
			    if((c-j) > 0) {
			        videoBuffer[240*(r-k) + (c-j)] = color;
			        videoBuffer[240*(r+k) + (c-j)] = color;
			    }
			}
		}
		k++;
	}	
}



void drawChoice(Choice choice, u16 boxcolor, u16 strcolor)
{
    drawRect(choice.row - 1, choice.col - 1, STRWIDTH(strlen(choice.str)) + 2, STRHEIGHT + 2, boxcolor);
    drawString(choice.row, choice.col, choice.str, strcolor);
}

void drawChooser(Choice choice)
{
    drawHollowRect(choice.row - 1, choice.col - 1, STRWIDTH(strlen(choice.str)) + 2, STRHEIGHT + 2, YELLOW);
}

void fillScreen(volatile unsigned short color)
{
	DMA[3].src = &color;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (160*240) | DMA_SOURCE_FIXED | DMA_ON;
}

void fillBackground(const unsigned short arr[])
{
	DMA[3].src = arr;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (160*240) | DMA_ON;
}

void drawImage3(int r, int c, int width, int height, const u16* image)
{
	for (int i = 0; i < height; i++)
	{
	    DMA[3].src = &image[i*width];
	    DMA[3].dst = &videoBuffer[(i+r)*240 + c];
	    DMA[3].cnt = width | DMA_ON;
	}
}

void drawBorderedRectangle(int row, int col, int height, int width, volatile u16 borderedColor, volatile u16 fillColor)
{
    int r;
    for(r=0; r<height; r++)
    {
        volatile u16 color;
        if(r==0 || r == (height -1))
        {
            color = borderedColor;
        }
        else
        {
            color = fillColor;
        }
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + (240*(row+r) + col);
        DMA[3].cnt = DMA_ON | DMA_SOURCE_FIXED | width;

        videoBuffer[240*(row+r) + col] = borderedColor;
        videoBuffer[240*(row+r) + width + col] = borderedColor;
    
    }
}


void waitForVblank()
{
	while(SCANLINECOUNTER > 160);	
	while(SCANLINECOUNTER < 160);
	
}
