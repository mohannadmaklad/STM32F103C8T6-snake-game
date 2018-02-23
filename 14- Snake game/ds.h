#ifndef _DS_H_
#define _DS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lcd.h"

#define getStackSize(stkptr) (stkptr.stackPointer)

typedef struct Coord
{
    uint8_t x ;
    uint8_t y;
}coord;

void setPoint(coord* crdptr, uint8_t xx , uint8_t yy);

typedef struct Stack
{
    coord list[LCD_SIZE];
    uint32_t stackPointer;
}stack;

void shift(stack *ss);
void lshift(uint8_t* arr , uint32_t length , uint32_t shiftby , uint8_t zeroTheRest);
void DrawSnake(stack*stkptr, uint8_t frame[LCD_SIZE]);
void initStack(stack*stckptr);
void push(stack*stckptr, coord *crdptr);
void pop(stack*stckptr, uint8_t *x, uint8_t *y);
void stackLshift(stack*stckptr , uint32_t length , uint32_t shiftby );
void stackRshift(stack*stckptr , uint32_t length , uint32_t shiftby );
void append(stack*stckptr, coord *crdptr);


#endif
