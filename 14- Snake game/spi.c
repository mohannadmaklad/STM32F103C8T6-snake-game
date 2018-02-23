#include "stm32f10x.h"                  // Device header
#include <stdlib.h>
#include "ds.h"
#include "HWInit.h"
#include "general.h"
#include "lcd.h"
#include "interrupt.h"
#include "GPIO.h"
#include <time.h>

//Game Configs
#define initial_length     (6)
#define gameSpeed         (300000)

/*-----------------------General-----------------------*/
uint8_t randInt(uint8_t max);
void randPoint(coord *point); //To generate fruit in random places
uint8_t samePoints(coord point1, coord point2);

/*-----------------------Globals-----------------------*/
uint8_t headX = initial_length, headY = 0; //horizontal snake
coord fruit;
coord headPoint,headPoint2;
stack snake;
void updateSnake();
/*-----------------------Frames-----------------------*/
uint8_t game[LCD_SIZE],tmpFrame[LCD_SIZE];
void initSnake(void);
extern void DrawSnake(stack*stkptr, uint8_t frame[LCD_SIZE]);
/*---------------------State Machine---------------------*/
enum states{right,left,up,down};
uint8_t currentState = right; //initial state
uint8_t lastState = right;
/*-------------------------------------------------------*/
/*-----------------------DIRECTIONS-----------------------*/
void updateDirection(void);
void goRight();
void goLeft();
void goUp();
void goDown();
/*-----------------------GAME-----------------------*/
uint32_t score = 0;
uint8_t hasEatenAfruit(coord head);
void increaseSnakeLength(stack *snk);
uint8_t isGameOver(coord head);
void gameOver(void);
/*-------------- LCD FUNCTIONS  --------------*/
uint16_t volatile *sr_ptr = &(SPI2->SR) ; 

int main(void)
 { 
  CLK_init ();
	RCC_init ();
	GPIO_init();
	SPI_init ();	
	EXTI_init();
	LCD_init ();
	 
	// Initial commands to init the LCD
	LCD_cmd(0x21);  // Function set : H = 1
	LCD_cmd(0xb1);  // 1011 0001 - Vop register 
	LCD_cmd(0x05);  //
	LCD_cmd(0x14);  //14
	LCD_cmd(0x20);
	LCD_cmd(0x0c); // Normal mode , 0x9 turns all segments on
	LCD_cmd(0xB2);
	 
	 
	 
	initSnake();
	resetFrame(game);
	DrawSnake(&snake,game);
	
	 
	randPoint(&fruit);
	setPixel(game,fruit.x,fruit.y);
	display(game);
	
	
	
	//EXTI0 ----> 6 .. EXTI4 ----> 10
	enableInterrupt(6);    //LEFT
	enableInterrupt(7);    //RIGHT
	enableInterrupt(8);    //UP
	enableInterrupt(9);    //DOWN
	//enableInterrupt(10);   //OK
	while(1)
	{
		resetFrame(game);
		if(isGameOver(headPoint))
		{
			gameOver();
			display(game);
			delay(1000000);
		}
		else
		{
			setPixel(game,fruit.x,fruit.y);
			updateDirection();
			updateSnake();
			DrawSnake(&snake,game);
			if(hasEatenAfruit(headPoint))
			{
				++score;
				increaseSnakeLength(&snake);
				randPoint(&fruit);
			}
			display(game);
			delay(gameSpeed );
		}
		
	}
}

/****************************/
void wait(void)
{
	while ((*sr_ptr & ((uint16_t)0x1 << 1 ) ) != 0x2 );
	
}





void initSnake(void)
{
	coord tmpPoint;
	initStack(&snake);
	 setPoint(&headPoint,headX,headY);
	 for(uint8_t x = 0, y= 0 ; x <=headX ; x++)
	 {
				setPoint(&tmpPoint,x,y);
				push(&snake , &tmpPoint);
	 }
}


void updateDirection(void)
{
	switch(currentState)
	{
		case left  : {goLeft(); break; }
		case right : {goRight(); break; }
		case up    : {goUp(); break; }
		case down  : {goDown(); break; }
	}
}

void updateSnake()
{
	uint8_t trashX, trashY;
	shift(&snake);
	pop(&snake,&trashX, &trashY);
	setPoint(&headPoint,headX,headY);
	push(&snake,&headPoint);
}

void goRight(void)
{
	if(headX < WIDTH-1) ++headX;
	else headX = 0 ;
}
void goLeft()
{
	if(headX > 0) --headX;
	else headX = WIDTH-1 ;
}

void goUp()
{
	if(headY > 0) --headY;
	else headY = HEIGHT-1;
}

void goDown()
{
	if(headY < HEIGHT-1) ++headY;
	else headY = 0;
}



void EXTI0_IRQHandler(void) //left
{
	
	switch(currentState)
	{
		case up:   {lastState= currentState; currentState = left; goLeft(); break;}
		case down: {lastState= currentState; currentState = left; goLeft(); break;}
	}
	updateSnake();
	DrawSnake(&snake,game);
	display(game);
	delay(gameSpeed);
	resetPendingBit(0);
}

void EXTI1_IRQHandler(void) //right
{
	switch(currentState)
	{
		case up:   {lastState= currentState; currentState = right; goRight(); break;}
		case down: {lastState= currentState; currentState = right; goRight(); break;}
	}
	updateSnake();
	DrawSnake(&snake,game);
	display(game);
	delay(gameSpeed);
	resetPendingBit(1);
}


void EXTI2_IRQHandler(void) //up
{
	switch(currentState)
	{
		case left:   {lastState= currentState; currentState = up; goUp(); break;}
		case right:  {lastState= currentState; currentState = up; goUp(); break;}
	}
		updateSnake();
		DrawSnake(&snake,game);
		display(game);
		delay(gameSpeed);
		resetPendingBit(2);
}

void EXTI3_IRQHandler(void) //up
{
	switch(currentState)
	{
		case left:   {lastState= currentState; currentState = down; goDown(); break;}
		case right:  {lastState= currentState; currentState = down; goDown(); break;}
	}
		updateSnake();
		DrawSnake(&snake,game);
		display(game);
		delay(gameSpeed);
		resetPendingBit(3);
}


uint8_t randInt(uint8_t max)
{
		return (rand()% max);
}

void randPoint(coord *point)
{
	uint8_t x ,y;
	x = randInt(84);
	y = randInt(48);
	setPoint(point,x,y);
}

uint8_t samePoints(coord point1, coord point2)
{
	return(point1.x == point2.x && point1.y == point2.y);
}


uint8_t hasEatenAfruit(coord head)
{
	return(samePoints(head,fruit));
}

void increaseSnakeLength(stack *snk)
{
	coord tail = snk->list[0];
	tail.x++;tail.y++;
	uint32_t c = snk->stackPointer;
	push(snk,&headPoint);//dummypoint
	stackRshift(snk,c,1);
	append(snk,&tail);
}

uint8_t isGameOver(coord head)
{
	uint32_t c = snake.stackPointer ;
	for(uint32_t i = 0 ; i < c-2 ; i++)
	{
		if(samePoints(snake.list[i],head))return 1  ;
	}
	return 0 ;
}

void gameOver(void)
{
	// Game over frame :(
	 for(uint32_t i = 0 ; i < 300 ; i++)
	 {
		extern uint16_t indx;
		coord tmpPoint;
		randPoint(&tmpPoint);
		setPixel(game,tmpPoint.x,tmpPoint.y);
		indx = XYtoIndex(10,10);
		write(game,"GAME OVER");
		indx = XYtoIndex(10,20);
		write(game,"Your score :"); write(game,"       ");
		writeInt(game,score);
	 }
 }