#include "Snake.h"

void again()
{
	SetPos(32, 11);
	printf("/*------------------------------*/");
	SetPos(32, 12);
	printf("/*-----欢迎来到贪吃蛇小游戏-----*/");
	SetPos(32, 13);
	printf("/*-----  请输入一下选项： ------*/");
	SetPos(32, 14);
	printf("/*--------  1：开始游戏   ------*/");
	SetPos(32, 15);
	printf("/*--------  0：游戏退出   ------*/");
	SetPos(32, 16);
	printf("/*------------------------------*/");
	SetPos(0, 29);
	system("pause");
}

void test()
{
	Snake snake;//
	srand((unsigned)time(NULL));//产生随机数的函数
	GameStart(&snake);//游戏开始函数
					//光标移动函数
	GameRun(&snake);//检测状态，状态为OK才会继续游戏，状态为其他游戏结束
	GameEnd(&snake);
}

int main()
{
	again();
	int input = 0;
	do
	{
		test();
		again();
		scanf_s("%d", &input);
	} while (input);
	
		
	
	
	
	/*int i = 0;
	while (i < 600)
	{
		Color(i);
		printf("%d    HH    ", i);
		i++;
	}*/
	/*COORD ps = { 0 };
	HANDLE handle = NULL;
	system("mode con cols=100 lines=30");*/
	SetPos(0, 29);
	system("pause");
	return 0;
}