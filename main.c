/*
	Snake Renewal �°�̰����
	���ߣ�Umbrellaؼ2
	��Ŀ��ʼʱ�䣺2019-7-25 9:00:00
	��Ŀ����ʱ�䣺2019-7-25 17:53:30
	���ü�����
	1.˫���壬�������̨��˸����
	2.�ļ�I/O��д��ʵ�����а����ݵĶ�ȡ�봢�档
	3.���������㷨��ð�ݡ�
	4.����״̬����������Ϸ״̬��ת����
	�����淶����������д+�»��ߣ�ȫ�ֱ���ʹ��CamelCase���ֲ�����ʹ��camelCase��
	������ϣ�
	˫���壺https://blog.csdn.net/weixinhum/article/details/72179593
	����̨���ھ��У�https://stackoverflow.com/questions/51344985/how-to-center-output-console-window-in-c
	�ܾ�������������/����
	������������ϰC���д��һ��С��Ŀ�����ο���Ӧ�û��кܶ�����Ż��ĵط�/������
	I/O����д��ʱ����ã�4Сʱ���ӹ��ƶ����Ҳ�����/������
	C#����������õ�����/������
*/

#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include <conio.h>
#include <Windows.h>
#include <winuser.h>
#include "wincon.h"
#include <time.h>

// ���崰�ڿ��
#define SCREEN_WIDTH 60
#define SCREEN_HEIGHT 35
#define GAME_WIDTH 59
#define GAME_HEIGHT 34
// ���尴������
#define KEY_ENTER 13
#define KEY_ESCAPE 27
#define KEY_BACKSPACE 8
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
// ������Ϸ����
#define RANKING_SCORE_NUM 10 // ���а��ܹ���¼�ķ�������
#define SCORE_MAXIMUM 9999 // ������
#define GAME_SPEED 60 // ������Ϸ��ˢ��Ƶ�ʣ�������Ĭ��Ϊ60FPS����1��ˢ��60�� 
#define MOVEMENT_INTERVAL 100 // ��Ϸˢ�����ڣ�����
#define FOOD_SCORE 50 // ʳ��ķ���

HANDLE ScreenBufferPrev, ScreenBufferPost;
HANDLE* ScreenOutput;
short ScreenBufferState = 0;

COORD Coord;
DWORD Bytes = 0;

// [y][x]
unsigned char Data[SCREEN_HEIGHT][SCREEN_WIDTH];

double intervalCount = 0;

// ������Ϸö��
// ����ַ��ķ���
enum DrawLineDirection
{
	Horizontal = 1, // ˮƽ
	Vertical // ��ֱ
};

enum GameState
{
	Title = 1,
	Menu,
	InGame,
	GameOver,
	Ranking
};
// ��ǰ��Ϸ״̬
enum GameState CurGameState = Title;

// ��Ϸ����
// �˵�
short MenuIndex = 0;
// ��Ϸ
int SnakeLength = 1; // ���峤��
short SnakeDir = 0; // 1 left 2 right 3 up 4 down ��һ���ƶ��ķ���
COORD SnakePosition[GAME_WIDTH * GAME_HEIGHT]; // ��ͷ+���λ��
COORD FoodPosition = {0, 0}; // ʳ���λ��
COORD PrevTailPosition = {0, 0}; // ��β��һ�����ڵ�λ��
int CurGameScore = 0; // ��ǰ��Ϸ����
short IsGameOver = 0; // �Ƿ���Ϸ����

// ���а�
int RankingScores[RANKING_SCORE_NUM];

void ScreenDraw(); // �ܻ��ƺ���
void GameLogic(); // ����Ϸ�߼�
// �������˵�OOP˼��
// ���Ʋ���
void Screen_DrawTitle(); // ���Ʊ��⻭��
void Screen_DrawMenu(); // ���Ʋ˵�
void Screen_DrawRanking(); // �������а�
void Screen_DrawGame(); // ������Ϸ
void Screen_DrawGameOver(); // ������Ϸ��������
// �ļ���
void File_WriteScoreByDelimiter(int scores[], const char* filePath, char delimiter); // ���������ָ���ļ�����delimiter���з����ָ�
void File_ReadScoreByDelimiter(const char* filePath, int* scores, const char delimiter); // ��ָ���ļ����������ָ�����飬��delimiter���ж�ȡ�ָ�
// ������
void Utils_MoveCenter(); // �����ھ���
void Utils_DrawWindowBorder(); // ������Ϸ�߿�
void Utils_DrawWindow(COORD windowPosition, COORD size); // ������Ϸ�Ӵ���
void Utils_FillCharInLine(enum DrawLineDirection direction, COORD startPosition, short drawLength, unsigned char drawChar); // ��drawChar��startCoordλ�ó���direction������䣬����ΪdrawLength
void Utils_SetText(COORD textPosition, const char* text); // ��textPositionλ����дtext�����˳��Ϊ��������
void Utils_ResetContent(); // ����������ʾ���ַ�����
void Utils_DoubleBuffer(); // ˫����
void Utils_BubbleSort(int* numArray, size_t numCount);// ð�����򣬿��ܻỻ�ɿ���
void Utils_Swap(int* a, int* b); // ����������ʹ��λ����ʵ�� 
// ��Ϸ��
void Game_InitGameData(); // ��ʼ����Ϸ����
void Game_SaveScore(int score); // �������
void Game_WorldBehave(); // �����ƶ��߼� 
void Game_SpawnFood(); // ����һ���µ�ʳ�� 
void Game_AddNewTail(); // ʹ�ߵĳ�������1 
void Game_MoveBodies(); // �ƶ��������ǵ��߼�˳���ǣ��ȴ�����ͷ���ƶ���Ȼ���ٴ���������ƶ� 
// ���Ĳ���
void Core_DoubleBufferInitialize(); // ��ʼ��˫���� 

int main(int argc, char* argv[])
{
	Game_InitGameData();
	Core_DoubleBufferInitialize();
	for (;;)
	{
		// �����Ҳ��õ����ȴ�����Ϸ�߼��ٽ������������̨�� 
		GameLogic();
		ScreenDraw();
	}
	return 0;
}

void Core_DoubleBufferInitialize()
{
	// ���ÿ���̨���ڴ�С
	system("mode con cols=60 lines=35"); // ���ÿ���̨�Ĵ�С�����������ó�60��35�� 
	SetConsoleOutputCP(437); // �ı����̨ʹ�õ��ַ�����ʹ���ܹ�ʹ�������������߿���ַ������ı�������޷�ͨ������̨��������ַ��� 
	SetConsoleTitle("Snake"); // ���ÿ���̨���ڵı��� 
	// ��ʼ������
	ScreenBufferPrev = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	ScreenBufferPost = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(ScreenBufferPrev, &cursorInfo);
	SetConsoleCursorInfo(ScreenBufferPost, &cursorInfo);
	// ���û������Ĵ�С
	COORD size = {SCREEN_WIDTH, SCREEN_HEIGHT};
	SetConsoleScreenBufferSize(ScreenBufferPrev, size);
	SetConsoleScreenBufferSize(ScreenBufferPost, size);
	// �ÿ���̨����
	Utils_MoveCenter();
}

void ScreenDraw()
{
	Utils_ResetContent();
	switch (CurGameState) // ����ǰ��Ϸ״̬ 
	{
		case Title:
			Screen_DrawTitle();
			break;
		case Menu:
			Screen_DrawMenu();
			break;
		case InGame:
			Screen_DrawGame();
			break;
		case Ranking:
			Screen_DrawRanking();
			break;
		case GameOver:
			Screen_DrawGameOver();
			break;
	}
	Utils_DoubleBuffer();
}

void GameLogic()
{
	if (kbhit()) // ������⣬ֻ�е��κΰ��������º�Ż���Ӧ 
	{
		char key = _getch(); // ȡ�õ�ǰ���µİ��� 
		if (key == KEY_ESCAPE)
		{
			exit(0);
			return;
		}
		switch (CurGameState)
		{
			case Title:
				if (key == KEY_ENTER)
				{
					CurGameState = Menu;
				}
				break;
			case Menu:
				if (key == KEY_UP)
				{
					MenuIndex--;
					if (MenuIndex < 0)
					{
						MenuIndex = 1;
					}
				}
				if (key == KEY_DOWN)
				{
					MenuIndex++;
					if (MenuIndex > 1)
					{
						MenuIndex = 0;
					}
				}
				if (key == KEY_ENTER)
				{
					switch (MenuIndex)
					{
						case 0:// ��ʼ��Ϸ
							CurGameState = InGame;
							SnakePosition[0].X = 15;
							SnakePosition[0].Y = 15;
							SnakeDir = 0;
							SnakeLength = 1;
							IsGameOver = 0;
							Game_SpawnFood();
							break;
						case 1:// ���а�
							CurGameState = Ranking;
							break;
					}
				}
				break;
			case InGame:
				if (!IsGameOver)
				{
					if (key == KEY_UP)
					{
						SnakeDir = 3;
					}
					if (key == KEY_DOWN)
					{
						SnakeDir = 4;
					}
					if (key == KEY_LEFT)
					{
						SnakeDir = 1;
					}
					if (key == KEY_RIGHT)
					{
						SnakeDir = 2;
					}
				}
				break;
			case Ranking:
				if (key == KEY_BACKSPACE)
				{
					CurGameState = Menu;
				}
				break;
			case GameOver:
				if (key == KEY_BACKSPACE)
				{
					Game_SaveScore(CurGameScore);
					CurGameScore = 0;
					CurGameState = Ranking;
				}
				break;
		}
	}

	if (CurGameState == InGame)
	{
		Game_WorldBehave();
		Sleep(MOVEMENT_INTERVAL);
	}
}

void Screen_DrawTitle()
{
	Utils_DrawWindowBorder();
	COORD drawCoord;
	drawCoord.X = SCREEN_WIDTH / 2 - 10 - 1;
	drawCoord.Y = SCREEN_HEIGHT - 2;
	Utils_SetText(drawCoord, "Press enter to start.");
}

void Screen_DrawMenu()
{
	Utils_DrawWindowBorder();
	COORD drawCoord;
	drawCoord.X = SCREEN_WIDTH / 2 - 4 - 1;
	drawCoord.Y = 8;
	Utils_SetText(drawCoord, "* MENU *");
	drawCoord.X -= 2;
	drawCoord.Y = 12;
	if (MenuIndex == 0)
	{
		Utils_SetText(drawCoord, "* START GAME");
		drawCoord.Y++;
		Utils_SetText(drawCoord, "  RANKING");
	}
	else
	{
		Utils_SetText(drawCoord, "  START GAME");
		drawCoord.Y++;
		Utils_SetText(drawCoord, "* RANKING");
	}
}

void Screen_DrawGame()
{
	Data[SnakePosition[0].Y][SnakePosition[0].X] = '@';
	for (int i = 1; i < SnakeLength; i++)
	{
		Data[SnakePosition[i].Y][SnakePosition[i].X] = '$';
	}
	Data[FoodPosition.Y][FoodPosition.X] = '*';
	Utils_DrawWindowBorder();
}

void Screen_DrawRanking()
{
	Utils_DrawWindowBorder();
	COORD drawCoord = { SCREEN_WIDTH / 2 - 5 - 1, 8};
	Utils_SetText(drawCoord, "* RANKING *");
	drawCoord.X -= 4;
	drawCoord.Y += 2;
	COORD size = {20, 15};
	Utils_DrawWindow(drawCoord, size);
	drawCoord.X += 4;
	drawCoord.Y += 1;
	Utils_SetText(drawCoord, "No.     Score");
	for (int i = 0; i < RANKING_SCORE_NUM; i++)
	{
		char temp[4];
		drawCoord.Y += 1;
		Utils_SetText(drawCoord, itoa(i + 1, temp, 10));
		drawCoord.X += 8;
		Utils_SetText(drawCoord, itoa(RankingScores[i], temp, 10));
		drawCoord.X -= 8;
	}
	drawCoord.X = SCREEN_WIDTH / 2 - 11;
	drawCoord.Y = SCREEN_HEIGHT - 2;
	Utils_SetText(drawCoord, "Press Backspace to back");
}

void Screen_DrawGameOver()
{
	Utils_DrawWindowBorder();
	COORD drawCoord = {15, 15};
	Utils_SetText(drawCoord, "GAME OVER");
	drawCoord.Y += 5;
	Utils_SetText(drawCoord, "Press Backspace to see ranking.");
}

void Game_InitGameData()
{
	for (int i = 0; i < 10; i++)
	{
		RankingScores[i] = 0;
	}
	if (access("scores.dat", 0) == 0)
	{
		File_ReadScoreByDelimiter("scores.dat", RankingScores, '#');
	}
}

void Game_SaveScore(int score)
{
	int tempArray[RANKING_SCORE_NUM + 1];
	for (int i = 0; i < RANKING_SCORE_NUM + 1; i++)
	{
		tempArray[i] = RankingScores[i];
	}
	tempArray[RANKING_SCORE_NUM] = score;
	Utils_BubbleSort(tempArray, RANKING_SCORE_NUM + 1);
	for (int i = 0; i < RANKING_SCORE_NUM; i++)
	{
		RankingScores[i] = tempArray[RANKING_SCORE_NUM - i];
	}
	File_WriteScoreByDelimiter(RankingScores, "scores.dat", '#');
}

void Game_WorldBehave()
{
	Game_MoveBodies();
	switch (SnakeDir)
	{
		case 1:
			SnakePosition[0].X--;
			if (SnakePosition[0].X <= 0)
			{
				CurGameState = GameOver;
				IsGameOver = 1;
			}
			break;
		case 2:
			SnakePosition[0].X++;
			if (SnakePosition[0].X >= GAME_WIDTH)
			{
				CurGameState = GameOver;
				IsGameOver = 1;
			}
			break;
		case 3:
			SnakePosition[0].Y--;
			if (SnakePosition[0].Y <= 0)
			{
				CurGameState = GameOver;
				IsGameOver = 1;
			}
			break;
		case 4:
			SnakePosition[0].Y++;
			if (SnakePosition[0].Y >= GAME_HEIGHT)
			{
				CurGameState = GameOver;
				IsGameOver = 1;
			}
			break;
	}
	
	if (Data[SnakePosition[0].Y][SnakePosition[0].X] == '*')
	{
		CurGameScore += FOOD_SCORE;
		Game_MoveBodies();
		Game_AddNewTail();
		Game_SpawnFood();
	}
}

void Game_SpawnFood()
{
	BOOL checked = FALSE;
	while (!checked)
	{
		srand((unsigned)time(NULL));// ������������� 
		FoodPosition.X = 2 + rand() % (GAME_WIDTH - 5);
		FoodPosition.Y = 2 + rand() % (GAME_HEIGHT - 5);
		if (Data[FoodPosition.Y][FoodPosition.X] != '@' && Data[FoodPosition.Y][FoodPosition.X] != '$')
		{
			checked = TRUE;
		}
	}
}

void Game_AddNewTail()
{
	SnakeLength++;
	SnakePosition[SnakeLength - 1].X = PrevTailPosition.X;
	SnakePosition[SnakeLength - 1].Y = PrevTailPosition.Y;
}

void Game_MoveBodies()
{
	PrevTailPosition.X = SnakePosition[SnakeLength - 1].X;
	PrevTailPosition.Y = SnakePosition[SnakeLength - 1].Y;
	for (int i = SnakeLength - 1; i > 0; i--)
	{
		SnakePosition[i].X = SnakePosition[i - 1].X;
		SnakePosition[i].Y = SnakePosition[i - 1].Y;
	}
}

void File_WriteScoreByDelimiter(int scores[], const char* filePath, char delimiter)
{
	FILE* filePtr = fopen(filePath, "w");
	char output[55];
	short index = 0;
	for (int i = 0; i < RANKING_SCORE_NUM; i++)
	{
		char* scoreStr;
		scoreStr = (char*)malloc(5);
		itoa(scores[i], scoreStr, 10);
		strcat(scoreStr, &delimiter);
		if (index == 0)
		{
			strcpy(output, scoreStr);
			index = 1;
		}
		else
		{
			strcat(output, scoreStr);
		}
		free(scoreStr);
	}
	fputs(output, filePtr);
	fclose(filePtr);
}

void File_ReadScoreByDelimiter(const char* filePath, int* scores, const char delimiter)
{
	FILE* filePtr = fopen(filePath, "r");
	unsigned short readCount = 0;
	char* score;
	score = (char*)malloc(8);
	short index = 0;
	for (int i = 0; i < 255; i++)
	{
		char letter;
		letter = fgetc(filePtr);
		if (letter == -1)
		{
			break;
		}
		if (letter == delimiter)
		{
			score[++index] = '\0';
			scores[readCount++] = atoi(score);
			index = 0;
		}
		else
		{
			score[index++] = letter;
		}
	}
	free(score);
	fclose(filePtr);
}

void Utils_DrawWindowBorder()
{
	COORD pos = {0, 0};
	COORD size = {SCREEN_WIDTH, SCREEN_HEIGHT};
	Utils_DrawWindow(pos, size);
}

void Utils_DrawWindow(COORD windowPosition, COORD size)
{
	COORD tempPos = windowPosition;
	// ���ƶ���
	Data[tempPos.Y][tempPos.X] = 201;
	tempPos.X++;
	enum DrawLineDirection drawDirection = Horizontal;
	Utils_FillCharInLine(drawDirection, tempPos, size.X - 2, 205);// ��������
	tempPos.Y = windowPosition.Y + size.Y - 1;
	Utils_FillCharInLine(drawDirection, tempPos, size.X - 2, 205);// �ײ�����
	Data[windowPosition.Y][windowPosition.X + size.X - 1] = 187;
	// �����м�
	tempPos.Y = windowPosition.Y + 1;
	for (int i = 0; i < 2; i++)
	{
		tempPos.X = windowPosition.X + (size.X - 1) * i;
		drawDirection = Vertical;
		Utils_FillCharInLine(drawDirection, tempPos, size.Y - 2, 186);
	}
	// ���Ƶײ�
	Data[windowPosition.Y + size.Y - 1][windowPosition.X] = 200;
	Data[windowPosition.Y + size.Y - 1][windowPosition.X + size.X - 1] = 188;
}

void Utils_FillCharInLine(enum DrawLineDirection direction, COORD startPosition, short drawLength, unsigned char drawChar)
{
	for (int i = 0; i < drawLength; i++)
	{
		switch (direction)
		{
			case Horizontal:
				Data[startPosition.Y][startPosition.X + i] = drawChar;
				break;
			case Vertical:
				Data[startPosition.Y + i][startPosition.X] = drawChar;
				break;
		}
	}
}

void Utils_SetText(COORD textPosition, const char* text)
{
	for (int i = 0; i < strlen(text); i++)
	{
		Data[textPosition.Y][textPosition.X + i] = text[i];
	}
}

void Utils_ResetContent()
{
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT; j++)
		{
			Data[j][i] = ' ';
		}
	}
}

void Utils_DoubleBuffer()
{
	ScreenBufferState = !ScreenBufferState;
	if (ScreenBufferState)
	{
		ScreenOutput = &ScreenBufferPrev;
	}
	else
	{
		ScreenOutput = &ScreenBufferPost;
	}
	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		Coord.Y = i;
		WriteConsoleOutputCharacter(*ScreenOutput, (char*)Data[i], SCREEN_WIDTH, Coord, &Bytes);
	}
	SetConsoleActiveScreenBuffer(*ScreenOutput);
}

void Utils_BubbleSort(int* numArray, size_t numCount)
{
	for (int i = 0; i < numCount - 1; i++)
	{
		for (int j = 0; j < numCount - i - 1; j++)
		{
			if (numArray[j] > numArray[j + 1])
			{
				Utils_Swap(&numArray[j], &numArray[j + 1]);
			}
		}
	}
}

void Utils_Swap(int* a, int* b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}

//extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow ();
void Utils_MoveCenter()
{
	RECT rectClient, rectWindow;
	HWND handle = GetConsoleWindow();
	GetClientRect(handle, &rectClient);
	GetWindowRect(handle, &rectWindow);
	int posx = GetSystemMetrics(SM_CXSCREEN) / 2 - (rectWindow.right - rectWindow.left) / 2;
	int posy = GetSystemMetrics(SM_CYSCREEN) / 2 - (rectWindow.bottom - rectWindow.top) / 2;
	MoveWindow(handle, posx, posy, rectClient.right - rectClient.left, rectClient.bottom - rectClient.top, TRUE);
}
