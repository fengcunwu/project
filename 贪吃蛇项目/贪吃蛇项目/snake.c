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
	//���ô��ڴ�С
	system("mode con cols=100 lines=30");//��win32 API ��ĺ��������ô��ڵĴ�С
	/*system("color 11");*/
	SetPos(40, 14);//������һ�δ�ӡ�Ĺ��λ��
	printf("��ӭ����̰����С��Ϸ\n");
	SetPos(0, 29);
	system("pause");//��ͣһ��
	system("cls");//����
	SetPos(30, 13);
	printf("�� �� �� �� �������ƶ��ķ���F1���٣�F2����\n");
	SetPos(0, 29);
	system("pause");
	system("cls");//����
	SetPos(35, 13);
	printf("���ٻ�ø���ķ���\n");
	SetPos(0, 29);
	system("pause");
	system("cls");//����
}

void PrintMap()
{
	int i = 0;
	Color(9);
	//��
	for (i = 0; i <= 58; i += 2)//ѭ����ӡ�������ַ����Ϊ2�����Ҫ�ӵ���2
	{
		SetPos(i, 0);
		//˯�ߺ�������100����Ҳ���Ǹ�0.1���ӡһ���ַ�
		printf(WALL);//����ĺ�
		
	}
	//��
	for (i = 0; i <= 58; i += 2)
	{
		SetPos(i, 27);
		
		printf(WALL);
		
	}
	//��
	for (i = 1; i <= 26; i++)
	{
		SetPos(0, i);
		
		printf(WALL);
		
	}
	//��
	for (i = 1; i <= 26; i++)
	{
		SetPos(58, i);
	
		printf(WALL);
	}
}

pSnakeNode BuyNode()
{
	pSnakeNode pNewNode = (pSnakeNode)malloc(sizeof(SnakeNode));//��̬�ڴ濪��
	//��ʼ�������Լ��ȶ����ߵ�λ�ã�������Ҳ��ά�� ��λ�õĽṹ��
	//���ٺ�����п�
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
	pSnakeNode pFirst = BuyNode();//����һ���µĽڵ�
	pSnakeNode pCur = NULL;
	int i = 0;
	pFirst->x = INIT_X;//��ʼ����һ���߽ڵ��λ��
	pFirst->y = INIT_Y;//

	for (i = 0; i < 4; i++)//����ͷ�巨��˼�룬��������
	{
		pCur = BuyNode();
		pCur->x = pFirst->x + 2;
		pCur->y = pFirst->y;
		pCur->next = pFirst;
		pFirst = pCur;
	}
	while (pCur != NULL)//ѭ����ӡ����
	{
		SetPos(pCur->x, pCur->y);
		Color(14);
		printf("��");
		pCur = pCur->next;
	}
	snake->psnake = pFirst;//����ͷ��������

}

void CreateFood(pSnake snake)//����ʳ��
{
	pSnakeNode pNewNode = BuyNode();//ʳ����һ���µĽڵ�
	pSnakeNode pCur = snake->psnake;//ʳ�ﲻ���������ϣ����Ҫ��������
	pNewNode->y = (rand() % 26) + 1;//�ڵ�ͼ��������������꣬��ʾ�ڼ���
	do
	{
		pNewNode->x = rand() % 55 + 2;//�ڵ�ͼ��������������꣬��ʾ�ڼ���
	} while (pNewNode->x % 2 != 0);//��Ϊ����ʳ��ķ��ſ��Ϊ2����˺����겻��Ϊ����

	while (pCur)
	{
		if (pCur->x == pNewNode->x && pCur->y == pNewNode->y)
		{
			CreateFood(snake);//ʳ���λ�ò����������ϣ����Ҫѭ����������
							//������������͵ݹ����²���ʳ���λ��
			return;
		}
		pCur = pCur->next;//ѭ��ֱ�������������
	}

	snake->pFood = pNewNode;//ʳ����ɺ󣬱��浽ά��̰��������ṹ������
	SetPos(pNewNode->x, pNewNode->y);//��λ��ӡ
	Color(14);
	printf("��");
	
}

void GameStart(pSnake snake)
{
	//��ӡ��ӭ����
	WelcomeToGame();//������Ϸ��ʼ����

	//������ͼ
	PrintMap();//��ӡ��Ϸ��ͼ������ǽ�ı߽�

	//��ʼ����
	InitSnake(snake);//��ʼ�������Լ�������ͷ

	//��ʼ��ʳ��
	CreateFood(snake);

	//��ʼ����������
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

void EatFood(pSnake snake, pSnakeNode pNewNode)//��ʳ�ﺯ��
{
	pSnakeNode pCur = NULL;//
	//��ͷ�巨��ʳ������ڵ���뵽�����ͷ��
	pNewNode->next = snake->psnake;
	snake->psnake = pNewNode;
	//��pcur����Ȼ���ӡ����
	pCur = snake->psnake;
	/*SetPos(pCur->x, pCur->y);
	Color(199);
	printf(SNAKE);*/
	//����ʳ���Ҫ�Ƿ���
	snake->Score += snake->AddScore;
	while (pCur->next)
	{
		SetPos(pCur->x, pCur->y);
		Color(11);
		Sleep(10);
		printf(SNAKE);
		pCur = pCur->next;
	}
	//ʳ�����Ӧ���ٴ���һ��ʳ��
	CreateFood(snake);
}

void NoFood(pSnake snake, pSnakeNode pNewNode)//����ʳ���Ҫ�ƶ��ĺ���
{
	pSnakeNode pCur = NULL;
	//��ͷ�巨���½ڵ���뵽�����ͷ��
	pNewNode->next = snake->psnake;
	snake->psnake = pNewNode;
	//ѭ���������Ҵ�ӡ����
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
	//���һ���ڵ��ͷ�
	SetPos(pCur->next->x, pCur->next->y);
	printf(" ");
	free(pCur->next);
	pCur->next = NULL;
}

void SnakeMove(pSnake snake)
{//�ߵ��ƶ������������
	pSnakeNode pNewNode = BuyNode();//������û��ʳ�ﶼҪ�����½ڵ㣬
	//��ʳ��Ļ����½ڵ���������Ҵ�ӡ
	//û��ʳ��Ļ������½ڵ�ͷ�嵽ͷ�����������һ����ӡ�ո�
	switch (snake->Dir)//ѡ��ͬ�ķ����ƶ� �ƶ��ķ�ʽ������ͬ
	{
	case UP:
	    {
				pNewNode->x = snake->psnake->x;
				pNewNode->y = snake->psnake->y - 1;
				if (NextIsFood(pNewNode, snake->pFood))//�ж���һ���ڵ��ǲ���ʳ��
				{//��ʳ��Ļ��ͳ�ʳ����Ҵ�ӡ
					//��ʳ��
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
					 //��ʳ��
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
					 //��ʳ��
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
					  //��ʳ��
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

void KillByWall(pSnake snake)//�ж��ǲ���ײǽ����
{
	if (snake->psnake->x == 0 || snake->psnake->x == 58 ||
		snake->psnake->y == 0 || snake->psnake->y == 27)
	{//����ǽ�ı߽��ж�
		snake->Sta = HIT_WALL;
	}
}

void KillByMy(pSnake snake)//�ж��ǲ���ײ�Լ�����
{
	pSnakeNode pCur = snake->psnake->next;
	while (pCur)
	{//ѭ������������ͷ��ÿ���ڵ����Ƚϣ���ͬ����ײ����
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
{//��Ϸ�����ֲ�
	SetPos(61, 8);
	printf("��Ϸ��ʾ��");
	SetPos(69, 10);
	printf("�� �� �� �� �������ƶ��ķ���");
	SetPos(69, 12);
	printf("F1--����  F2����");
	SetPos(69, 14);
	printf("�ܷ֣�%d ", snake->Score);
	SetPos(69, 16);
	printf("ʳ�����ӷ�����%d ", snake->AddScore);

}

void GameRun(pSnake snake)
{
	do
	{
		//ȷ������ ���������� ----------- ����������
		if (GetAsyncKeyState(VK_UP) && snake->Dir != DOWN)//����ߵķ���
		{//��������������ҷ���������
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
		{//���˿ո������ͣ��Ϸ
			 //��Ϸ��ͣ ˯��״̬
			Pause();
		}
		else if (GetAsyncKeyState(VK_ESCAPE))
		{//����esc�� �˳���Ϸ
			//��Ϸ�˳� �ı�״̬
			snake->Sta = NORMAL_EXIT;
		}
		else if (GetAsyncKeyState(VK_F1))
		{
			//��F1�߼���
			if (snake->SleepTime >= 40)
			{
				snake->SleepTime -= 20;//
				snake->AddScore += 2;
			}
		}
		else if (GetAsyncKeyState(VK_F2))
		{
			//��F2�߼���
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
		//�ߵ��ƶ�
		SnakeMove(snake);
		PrintHelpInFo(snake);
		Sleep(snake->SleepTime);
		KillByWall(snake);
		KillByMy(snake);
	} while (snake->Sta == OK);
}

void GameEnd(pSnake snake)//��ӡ�����Ϸ�˳���ԭ��
{
	pSnakeNode pCur = snake->psnake;
	SetPos(20, 13);
	if (snake->Sta == NORMAL_EXIT)
	{
		printf("�˳���Ϸ");
	}
	else if (snake->Sta == HIT_OWN)
	{
		printf("�Լ�ײ�Լ���");
	}
	else if (snake->Sta == HIT_WALL)
	{
		printf("ײǽ��");
	}
	while (pCur)
	{//���������ÿһ���ڵ�
		pSnakeNode pDel = pCur;
		pCur = pCur->next;
		free(pDel);
		pDel = NULL;
	}
}