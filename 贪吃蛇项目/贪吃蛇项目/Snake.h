#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
//mode Handle  COORD DWORD 
//GetStdHandle 
//GetAsyncKeyState  返回当前键盘某些键的状态

#define WALL "■"
#define FOOD "●"
#define SNAKE "●"
#define INIT_X 20
#define INIT_Y 20

typedef struct SnakeNode
{
	int x;
	int y;
	struct SnakeNode* next;
}SnakeNode, *pSnakeNode;

typedef enum Direction
{
	UP = 1,
	DOWN,
	LEFT,
	RIGHT
}Direction;

typedef enum Status
{
	OK,
	NORMAL_EXIT,//正常退出
	HIT_WALL,//撞墙
	HIT_OWN//撞自己
}Status;

typedef struct Snake//维护贪吃蛇这个游戏的结构体
{
	pSnakeNode psnake; //维护蛇身的指针蛇头
	pSnakeNode pFood;//维护食物的位置
	int Score;//当前分数
	int AddScore;//增加的分数
	int SleepTime;//休眠时间
	Direction Dir;//控制方向
	Status Sta;//当前状态

}Snake, *pSnake;

void GameStart(pSnake snake);
void SetPos(int x, int y);
void GameRun(pSnake snake);
void SnakeMove(pSnake snake);
void Color(int num);
void WelcomeToGame();
void PrintMap();
pSnakeNode BuyNode();
void InitSnake(pSnake snake);
void CreateFood(pSnake snake);
void GameStart(pSnake snake);
void Pause();
int NextIsFood(pSnakeNode pNewNode, pSnakeNode pf);
void EatFood(pSnake snake, pSnakeNode pNewNode);
void NoFood(pSnake snake, pSnakeNode pNewNode);
void SnakeMove(pSnake snake);
void KillByWall(pSnake snake);
void KillByMy(pSnake snake);
void PrintHelpInFo(snake);
void GameEnd(pSnake snake);

#endif //__SNAKE_H__