#include "Snake.h"

void SetPos(int x, int y)
{
	COORD pos = { 0 };
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle, pos);
}

void Color(int num)
{
	SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), num);
	return;
}

void WelcomeToGame()
{
	//设置窗口大小
	system("mode con cols=100 lines=30");//用win32 API 里的函数来设置窗口的大小
	/*system("color 11");*/
	SetPos(40, 14);//设置下一次打印的光标位置
	printf("欢迎来到贪吃蛇小游戏\n");
	SetPos(0, 29);
	system("pause");//暂停一下
	system("cls");//清屏
	SetPos(30, 13);
	printf("↑ ↓ ← → 控制蛇移动的方向，F1加速，F2减速\n");
	SetPos(0, 29);
	system("pause");
	system("cls");//清屏
	SetPos(35, 13);
	printf("加速获得更多的分数\n");
	SetPos(0, 29);
	system("pause");
	system("cls");//清屏
}

void PrintMap()
{
	int i = 0;
	Color(9);
	//上
	for (i = 0; i <= 58; i += 2)//循环打印，所用字符宽度为2，因此要加等于2
	{
		SetPos(i, 0);
		//睡眠函数，隔100毫秒也就是隔0.1秒打印一个字符
		printf(WALL);//定义的宏
		
	}
	//下
	for (i = 0; i <= 58; i += 2)
	{
		SetPos(i, 27);
		
		printf(WALL);
		
	}
	//左
	for (i = 1; i <= 26; i++)
	{
		SetPos(0, i);
		
		printf(WALL);
		
	}
	//右
	for (i = 1; i <= 26; i++)
	{
		SetPos(58, i);
	
		printf(WALL);
	}
}

pSnakeNode BuyNode()
{
	pSnakeNode pNewNode = (pSnakeNode)malloc(sizeof(SnakeNode));//动态内存开辟
	//初始化蛇身，自己先定义蛇的位置，创建的也是维护 蛇位置的结构体
	//开辟后必须判空
	if (pNewNode == NULL)
	{
		perror("BuyNode::malloc");
		exit(0);
	}
	pNewNode->next = NULL;
	pNewNode->x = 0;
	pNewNode->y = 0;

	return pNewNode;
}

void InitSnake(pSnake snake)
{
	pSnakeNode pFirst = BuyNode();//创建一个新的节点
	pSnakeNode pCur = NULL;
	int i = 0;
	pFirst->x = INIT_X;//初始化第一个蛇节点的位置
	pFirst->y = INIT_Y;//

	for (i = 0; i < 4; i++)//利用头插法的思想，创建蛇身
	{
		pCur = BuyNode();
		pCur->x = pFirst->x + 2;
		pCur->y = pFirst->y;
		pCur->next = pFirst;
		pFirst = pCur;
	}
	while (pCur != NULL)//循环打印蛇身
	{
		SetPos(pCur->x, pCur->y);
		Color(14);
		printf("●");
		pCur = pCur->next;
	}
	snake->psnake = pFirst;//把蛇头保存起来

}

void CreateFood(pSnake snake)//创建食物
{
	pSnakeNode pNewNode = BuyNode();//食物是一个新的节点
	pSnakeNode pCur = snake->psnake;//食物不能在蛇身上，因此要遍历蛇身
	pNewNode->y = (rand() % 26) + 1;//在地图上随机产生纵坐标，表示第几行
	do
	{
		pNewNode->x = rand() % 55 + 2;//在地图上随机产生横坐标，表示第几列
	} while (pNewNode->x % 2 != 0);//因为构成食物的符号宽度为2，因此横坐标不能为奇数

	while (pCur)
	{
		if (pCur->x == pNewNode->x && pCur->y == pNewNode->y)
		{
			CreateFood(snake);//食物的位置不能在蛇身上，因此要循环遍历蛇身
							//如果等于蛇身，就递归重新产生食物的位置
			return;
		}
		pCur = pCur->next;//循环直到蛇身遍历结束
	}

	snake->pFood = pNewNode;//食物完成后，保存到维护贪吃蛇这个结构体里面
	SetPos(pNewNode->x, pNewNode->y);//定位打印
	Color(14);
	printf("●");
	
}

void GameStart(pSnake snake)
{
	//打印欢迎界面
	WelcomeToGame();//进入游戏开始界面

	//创建地图
	PrintMap();//打印游戏地图，包括墙的边界

	//初始化蛇
	InitSnake(snake);//初始化蛇身以及保存蛇头

	//初始化食物
	CreateFood(snake);

	//初始化其他对象
	snake->AddScore = 10;
	snake->Dir = RIGHT;
	snake->SleepTime = 200;
	snake->Sta = OK;
	snake->Score = 0;
	//SetPos(0, 29);
	//system("pause");
}

void Pause()
{
	while (1)
	{
		Sleep(100);
		if (GetAsyncKeyState(VK_SPACE))
		{
			break;
		}
	}
}

int NextIsFood(pSnakeNode pNewNode, pSnakeNode pf)
{
	return (pNewNode->x == pf->x) && (pNewNode->y == pf->y);
}

void EatFood(pSnake snake, pSnakeNode pNewNode)//吃食物函数
{
	pSnakeNode pCur = NULL;//
	//用头插法把食物这个节点插入到链表的头部
	pNewNode->next = snake->psnake;
	snake->psnake = pNewNode;
	//用pcur遍历然后打印蛇身
	pCur = snake->psnake;
	/*SetPos(pCur->x, pCur->y);
	Color(199);
	printf(SNAKE);*/
	//吃了食物就要涨分数
	snake->Score += snake->AddScore;
	while (pCur->next)
	{
		SetPos(pCur->x, pCur->y);
		Color(11);
		Sleep(10);
		printf(SNAKE);
		pCur = pCur->next;
	}
	//食物吃了应该再创建一个食物
	CreateFood(snake);
}

void NoFood(pSnake snake, pSnakeNode pNewNode)//不是食物，还要移动的函数
{
	pSnakeNode pCur = NULL;
	//用头插法把新节点插入到链表的头部
	pNewNode->next = snake->psnake;
	snake->psnake = pNewNode;
	//循环遍历并且打印蛇身
	pCur = snake->psnake;
	while (pCur->next->next)
	{
		SetPos(pCur->x, pCur->y);
		printf(SNAKE);
		pCur = pCur->next;
	}
	SetPos(pCur->x, pCur->y);
	printf(SNAKE);
	/*pCur = snake->psnake;
	while (pCur->next->next)
	{
		pCur = pCur->next;
	}
	SetPos(pCur->x, pCur->y);
	printf(SNAKE);*/
	//最后一个节点释放
	SetPos(pCur->next->x, pCur->next->y);
	printf(" ");
	free(pCur->next);
	pCur->next = NULL;
}

void SnakeMove(pSnake snake)
{//蛇的移动方向就是四种
	pSnakeNode pNewNode = BuyNode();//不管有没有食物都要创建新节点，
	//有食物的话吧新节点插入链表并且打印
	//没有食物的话，把新节点头插到头部，链表最后一个打印空格
	switch (snake->Dir)//选择不同的方向移动 移动的方式大致相同
	{
	case UP:
	    {
				pNewNode->x = snake->psnake->x;
				pNewNode->y = snake->psnake->y - 1;
				if (NextIsFood(pNewNode, snake->pFood))//判断下一个节点是不是食物
				{//是食物的话就吃食物，并且打印
					//吃食物
					EatFood(snake, pNewNode);
				}
				else
				{
					NoFood(snake, pNewNode);
				}
	    }
		break;
	case DOWN:
	{
				 pNewNode->x = snake->psnake->x;
				 pNewNode->y = snake->psnake->y + 1;
				 if (NextIsFood(pNewNode, snake->pFood))
				 {
					 //吃食物
					 EatFood(snake, pNewNode);
				 }
				 else
				 {
					 NoFood(snake, pNewNode);
				 }
	}
		break;
	case LEFT:
	{
				 pNewNode->x = snake->psnake->x - 2;
				 pNewNode->y = snake->psnake->y;
				 if (NextIsFood(pNewNode, snake->pFood))
				 {
					 //吃食物
					 EatFood(snake, pNewNode);
				 }
				 else
				 {
					 NoFood(snake, pNewNode);
				 }
	}
		break;
	case RIGHT:
	{
				  pNewNode->x = snake->psnake->x + 2;
				  pNewNode->y = snake->psnake->y;
				  if (NextIsFood(pNewNode, snake->pFood))
				  {
					  //吃食物
					  EatFood(snake, pNewNode);
				  }
				  else
				  {
					  NoFood(snake, pNewNode);
				  }
	}
		break;
	}
}

void KillByWall(pSnake snake)//判断是不是撞墙死了
{
	if (snake->psnake->x == 0 || snake->psnake->x == 58 ||
		snake->psnake->y == 0 || snake->psnake->y == 27)
	{//根据墙的边界判断
		snake->Sta = HIT_WALL;
	}
}

void KillByMy(pSnake snake)//判断是不是撞自己死了
{
	pSnakeNode pCur = snake->psnake->next;
	while (pCur)
	{//循环遍历，用蛇头和每个节点做比较，相同就是撞上了
		if ((pCur->x == snake->psnake->x) &&
			(pCur->y == snake->psnake->y))
		{
			snake->Sta = HIT_OWN;
			return;
		}
		pCur = pCur->next;
	}
}

void PrintHelpInFo(pSnake snake)
{//游戏帮助手册
	SetPos(61, 8);
	printf("游戏提示：");
	SetPos(69, 10);
	printf("↑ ↓ ← → 控制蛇移动的方向");
	SetPos(69, 12);
	printf("F1--加速  F2减速");
	SetPos(69, 14);
	printf("总分：%d ", snake->Score);
	SetPos(69, 16);
	printf("食物增加分数：%d ", snake->AddScore);

}

void GameRun(pSnake snake)
{
	do
	{
		//确定方向 按↑向上走 ----------- 按↓向下走
		if (GetAsyncKeyState(VK_UP) && snake->Dir != DOWN)//检测蛇的方向
		{//按了向↑键，并且方向不是向下
			snake->Dir = UP;
		}
		else if (GetAsyncKeyState(VK_DOWN) && snake->Dir != UP)
		{
			snake->Dir = DOWN;
		}
		else if (GetAsyncKeyState(VK_LEFT) && snake->Dir != RIGHT)
		{
			snake->Dir = LEFT;
		}
		else if (GetAsyncKeyState(VK_RIGHT) && snake->Dir != LEFT)
		{
			snake->Dir = RIGHT;
		}
		else if (GetAsyncKeyState(VK_SPACE))
		{//按了空格键，暂停游戏
			 //游戏暂停 睡眠状态
			Pause();
		}
		else if (GetAsyncKeyState(VK_ESCAPE))
		{//按了esc键 退出游戏
			//游戏退出 改变状态
			snake->Sta = NORMAL_EXIT;
		}
		else if (GetAsyncKeyState(VK_F1))
		{
			//按F1蛇加速
			if (snake->SleepTime >= 40)
			{
				snake->SleepTime -= 20;//
				snake->AddScore += 2;
			}
		}
		else if (GetAsyncKeyState(VK_F2))
		{
			//按F2蛇减速
			if (snake->SleepTime < 280)
			{
				snake->SleepTime += 20;//
				snake->AddScore -=2;
			}
			else
			{
				snake->AddScore = 2;
			}
		}
		//蛇的移动
		SnakeMove(snake);
		PrintHelpInFo(snake);
		Sleep(snake->SleepTime);
		KillByWall(snake);
		KillByMy(snake);
	} while (snake->Sta == OK);
}

void GameEnd(pSnake snake)//打印最后游戏退出的原因
{
	pSnakeNode pCur = snake->psnake;
	SetPos(20, 13);
	if (snake->Sta == NORMAL_EXIT)
	{
		printf("退出游戏");
	}
	else if (snake->Sta == HIT_OWN)
	{
		printf("自己撞自己了");
	}
	else if (snake->Sta == HIT_WALL)
	{
		printf("撞墙了");
	}
	while (pCur)
	{//销毁链表的每一个节点
		pSnakeNode pDel = pCur;
		pCur = pCur->next;
		free(pDel);
		pDel = NULL;
	}
}