#include "Snake.h"

void again()
{
	SetPos(32, 11);
	printf("/*------------------------------*/");
	SetPos(32, 12);
	printf("/*-----��ӭ����̰����С��Ϸ-----*/");
	SetPos(32, 13);
	printf("/*-----  ������һ��ѡ� ------*/");
	SetPos(32, 14);
	printf("/*--------  1����ʼ��Ϸ   ------*/");
	SetPos(32, 15);
	printf("/*--------  0����Ϸ�˳�   ------*/");
	SetPos(32, 16);
	printf("/*------------------------------*/");
	SetPos(0, 29);
	system("pause");
}

void test()
{
	Snake snake;//
	srand((unsigned)time(NULL));//����������ĺ���
	GameStart(&snake);//��Ϸ��ʼ����
					//����ƶ�����
	GameRun(&snake);//���״̬��״̬ΪOK�Ż������Ϸ��״̬Ϊ������Ϸ����
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