#include "ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lcd.h"
enum states{right,left,up,down};

extern uint8_t snakeThickness, currentState;
void lshift(uint8_t* arr , uint32_t length , uint32_t shiftby , uint8_t zeroTheRest)
{
	memmove(&arr[0] , &arr[shiftby] , (length-shiftby)*sizeof(int));
	if (zeroTheRest == 1)
	{
		int i ;
		for(i = length - shiftby ; i <length ; i++)
		{
			arr[i] = 0 ;
		}
	}
}

void shift(stack *ss)
{
    coord *arr = &ss->list[0];
    for(uint32_t i = 0 ; i<LCD_SIZE; i++)
    {
        arr[i] = arr[i+1];
    }
}


void setPoint(coord* crdptr, uint8_t xx , uint8_t yy)
{
    crdptr -> x = xx; crdptr -> y = yy;
}

void initStack(stack*stckptr)
{
    stckptr ->stackPointer = 0;
}

void push(stack*stckptr, coord *crdptr)
{
    stckptr -> list[stckptr -> stackPointer] = *crdptr; //push point at the top
    stckptr -> stackPointer++;
}

void pop(stack*stckptr, uint8_t *xx, uint8_t *yy)
{
		--(stckptr -> stackPointer);
    *xx = (stckptr->list[(stckptr -> stackPointer)]).x;
    *yy = (stckptr->list[(stckptr -> stackPointer)]).y;
}

void stackLshift(stack*stckptr , uint32_t length , uint32_t shiftby)
{
    coord* arr = &(stckptr->list[0]);
		memmove(&arr[0] , &arr[shiftby] , (length-shiftby)*sizeof(coord));
}

void stackRshift(stack*stckptr , uint32_t length , uint32_t shiftby )
{
    coord* arr = &(stckptr->list[0]);
	memmove( &arr[shiftby], &arr[0] , (length-shiftby)*sizeof(coord));
}

void DrawSnake(stack*stkptr, uint8_t frame[LCD_SIZE])
{
  int c = stkptr -> stackPointer;

    for(int i =0 ; i < c ; ++i)
    {
				setPixel(frame,(stkptr->list[i]).x,(stkptr->list[i]).y);
    }
}

void append(stack*stckptr, coord *crdptr)
{
    push(stckptr,crdptr);//pushes crdptr to the top, but will be sacrificed next step
    stackRshift(stckptr,LCD_SIZE,1);
    stckptr ->list[0] = *crdptr;
}
