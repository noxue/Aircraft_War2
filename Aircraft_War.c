#include <time.h>
#include <conio.h>
#include <windows.h>
#include "ansi.h"

#define MAX_X 60
#define MAX_Y 35

#define LIFE 6
#define BULLET 500

typedef struct _aircraft_information
{
	int score;
	int speed;
	int bpspeed;
	int life;
	int bullet;
	struct _head_position
	{
		int x;
		int y;
	} hep;
} aci;

typedef struct _position
{
	int x;
	int y;
} * pos;

#define LIST_TYPE pos
#include "list.h"

aci *aci_init();
int map_init();
void init_window();
void refresh_score(aci *aci);
void refresh_life(aci *aci);
void aci_dir(aci *aci);
void aci_move(aci *aci, list *bp, list *ap);
void bp_move(aci *aci, list *bp, list *ap);
void ap_move(list *ap);
void sp_move(list *sp, list *ap);
void gotoxy(int x, int y);
void help();

int main()
{
	system("cls");
	init_window();
	srand((unsigned)time(NULL));
	map_init();
	help();
	list *bp = list_init();
	list *ap = list_init();
	list *sp = list_init();
	aci *aci = aci_init();
	// gotoxy(MAX_X + 6, 9);
	// printf("子弹:");
	gotoxy(MAX_X + 5, 7);
	printf("═══════════════════╣");
	gotoxy(MAX_X + 6, 8);
	printf("子弹:             ║");
	gotoxy(MAX_X + 5, 9);
	printf("═══════════════════╝");
	refresh_score(aci);
	refresh_life(aci);
	int xs = aci->bpspeed;
	int last_score = 0;
	while (1)
	{
		aci_move(aci, bp, ap);
		bp_move(aci, bp, ap);
		xs--;
		if (!xs)
			ap_move(ap);
		if (xs <= 0)
			xs = aci->bpspeed;
		if (last_score != aci->score)
		{
			refresh_score(aci);
			last_score = aci->score;
		}
		sp_move(sp, ap);
		if (aci->score >= 999)
		{
			gotoxy(MAX_X / 2 - 5, MAX_Y / 2);
			printf("你胜利了");
			Sleep(3000);
			break;
		}
		Sleep(aci->speed);
	}
	getch();
	return 0;
}

void init_window()
{

	// 解决中文乱码
	SetConsoleOutputCP(65001);

	ansi_cursor_hide();
}

aci *aci_init()
{
	aci *paci = (aci *)malloc(sizeof(aci));
	paci->score = 0;
	paci->speed = 5;		 //整体速度
	paci->bpspeed = 3;		 //敌机速度
	paci->life = LIFE;		 //生命值
	paci->bullet = BULLET;	 //子弹数
	paci->hep.x = MAX_X / 2; //起始位置
	paci->hep.y = MAX_Y - 3;
	return paci;
}

//初始化地图
int map_init()
{

	for (int i = 0; i < MAX_Y + 2; i++)
	{
		for (int j = 0; j < MAX_X / 2 + 3; j++)
		{
			if (i == MAX_Y + 1)
			{
				if (j == 0)
					printf("╚");
				else if (j == MAX_X / 2 + 2)
					printf("╝");
				else
					printf("══");
			}
			else
			{
				if (j == 0)
					printf("║");
				else if (j == MAX_X / 2 + 2)
					printf("║");
				else
					printf("  ");
			}
		}
		printf("\n");
	}
	// 游戏：飞机大战
	gotoxy(MAX_X + 5, 1);
	printf("═══════════════════╗");
	gotoxy(MAX_X + 5, 2);
	printf(" 游戏:飞机大战     ║");
	ansi_reset();

	gotoxy(MAX_X + 5, 3);
	printf("═══════════════════╣");
	gotoxy(MAX_X + 6, 4);
	printf("分数:             ║");
	gotoxy(MAX_X + 5, 5);
	printf("═══════════════════╣");
	ansi_reset();
	return 1;
}

void refresh_score(aci *aci)
{
	gotoxy(MAX_X + 13, 4);
	printf("%3d", aci->score);
}

void refresh_life(aci *aci)
{

	gotoxy(MAX_X + 6, 6);
	printf("血量:");
	for (int i = 0; i < LIFE; i++)
		printf("   ");
	gotoxy(MAX_X + 24, 6);
	printf("║");
	gotoxy(MAX_X + 12, 6);
	ansi_fg_red();
	for (int i = 0; i < aci->life; i++)
		printf("血");
	ansi_reset();
	gotoxy(MAX_X + 5, 7);
	printf("═══════════════════╣");
}

int reloadflag;
void aci_move(aci *aci, list *bp, list *ap)
{
	if (kbhit())
	{

		gotoxy(aci->hep.x, aci->hep.y - 1);
		printf("  ");
		gotoxy(aci->hep.x, aci->hep.y);
		printf("  ");
		gotoxy(aci->hep.x - 1, aci->hep.y + 1);
		printf("    ");
		if (GetKeyState(87) < 0 && aci->hep.y >= 3)
			aci->hep.y--;
		if (GetKeyState(83) < 0 && aci->hep.y <= MAX_Y - 2)
			aci->hep.y++;
		if (GetKeyState(65) < 0 && aci->hep.x > 4)
			aci->hep.x--;
		if (GetKeyState(68) < 0 && aci->hep.x <= MAX_X)
			aci->hep.x++;

		reloadflag = 1;
		if (GetKeyState(VK_SPACE) < 0 && aci->bullet >= 0)
		{
			reloadflag = 0;
			if (aci->bullet == 0)
				goto endkbhit;
			pos p = (pos)malloc(sizeof(struct _position));
			p->x = aci->hep.x;
			p->y = aci->hep.y - 1;
			list_push_back(bp, p);
			// Beep(800, 2);
			aci->bullet--;
			gotoxy(MAX_X + 12, 8);
			printf("%3d", aci->bullet);
			ansi_reset();
		}
	}
endkbhit:
	if (reloadflag)
	{
		if (aci->bullet >= BULLET)
			reloadflag = 0;
		else
			aci->bullet++;
		gotoxy(MAX_X + 12, 8);
		printf("%3d", aci->bullet);
		ansi_reset();
	}
	pos atmp;
	for (int j = 0; j < ap->size; j++)
	{
		atmp = list_var(ap, j);
		if ((atmp->x == aci->hep.x || atmp->x == aci->hep.x + 1 || atmp->x == aci->hep.x - 1) && atmp->y == aci->hep.y)
		{
			list_erase(ap, 1, j);
			gotoxy(atmp->x, atmp->y);
			printf("  ");
			aci->life--;
			refresh_life(aci);
			if (aci->life == 0)
			{
				gotoxy(MAX_X / 2 - 5, MAX_Y / 2);
				printf("You Lose!");
				Sleep(3000);
				exit(0);
			}
		}
	}
	ansi_fg_rgb(255, 255, 0);
	ansi_font_flash_fast();

	gotoxy(aci->hep.x, aci->hep.y - 1);
	printf("炮");
	gotoxy(aci->hep.x, aci->hep.y);
	printf("大");
	ansi_font_no_flash();
	gotoxy(aci->hep.x - 1, aci->hep.y + 1);
	printf("飞机");
	ansi_reset();
}

void bp_move(aci *aci, list *bp, list *ap)
{
	pos tmp;
	if (bp->size >= 55)
		list_erase(bp, 20, 0);
	for (int i = 0; i < bp->size; i++)
	{
		tmp = list_var(bp, i);
		gotoxy(tmp->x, tmp->y);
		printf("  ");
		if (tmp->y <= 1)
			continue;
		tmp->y--;
		gotoxy(tmp->x, tmp->y);
		ansi_fg_yellow();
		ansi_font_bold();
		printf("炮");
		ansi_reset();
		pos atmp;
		for (int j = 0; j < ap->size; j++)
		{
			atmp = list_var(ap, j);
			if ((atmp->x == tmp->x || atmp->x == tmp->x + 1 || atmp->x == tmp->x - 1) && atmp->y == tmp->y)
			{
				list_erase(ap, 1, j);
				list_erase(bp, 1, i);
				gotoxy(tmp->x, tmp->y);
				printf("  ");
				gotoxy(atmp->x, atmp->y);
				printf("  ");
				aci->score++;
				Beep(5000, 2);
				break;
			}
		}
	}
}

void ap_move(list *ap)
{
	static int ap_f = 1;
	if (ap->size >= 40)
		list_erase(ap, 19, 0);
	ap_f--;
	if (!ap_f)
	{
		pos p = (pos)malloc(sizeof(struct _position));
		p->x = rand() % (MAX_X - 1) + 3;
		p->y = 1;
		list_push_back(ap, p);
	}
	if (ap_f < 0)
		ap_f = 1;
	pos tmp;
	for (int i = 0; i < ap->size; i++)
	{
		tmp = list_var(ap, i);
		gotoxy(tmp->x, tmp->y);
		printf("  ");
		if (tmp->y >= MAX_Y - 1)
		{
			tmp->y = MAX_Y + 3;
			continue;
		}
		tmp->y++;
		gotoxy(tmp->x, tmp->y);
		printf("船");
	}
}

void sp_move(list *sp, list *ap)
{
	if (sp->size >= 70)
		list_erase(sp, 30, 0);
	pos p = (pos)malloc(sizeof(struct _position));
	p->x = rand() % (MAX_X - 1) + 3;
	p->y = 1;
	list_push_back(sp, p);
	pos tmp;
	pos atmp;
	for (int i = 0; i < sp->size; i++)
	{
		tmp = list_var(sp, i);
		for (int j = 0; j < ap->size; j++)
		{
			atmp = list_var(ap, j);
			if ((atmp->x == tmp->x && atmp->y == tmp->y) || (atmp->x + 1 == tmp->x && atmp->y == tmp->y))
				goto CON;
		}
		gotoxy(tmp->x, tmp->y);
		printf(" ");
		if (tmp->y >= MAX_Y - 1)
		CON:
			continue;
		tmp->y++;
		gotoxy(tmp->x, tmp->y);
		for (int j = 0; j < ap->size; j++)
		{
			atmp = list_var(ap, j);
			if ((atmp->x == tmp->x && atmp->y == tmp->y) || (atmp->x + 1 == tmp->x && atmp->y == tmp->y))
				goto CON;
		}
		ansi_fg_rgb(100, 100, 100);
		printf(".");
		ansi_reset();
	}
}

void gotoxy(int x, int y)
{
	ansi_cursor_position(y, x);
}

void help()
{

	gotoxy(MAX_X + 8, 24);
	ansi_fg_yellow();
	ansi_font_bold();
	printf("需要源码请关注我");
	ansi_font_underline();

	gotoxy(MAX_X + 8, 26);
	printf("抖音：不学网");

	ansi_font_underline_off();
	ansi_reset();
	gotoxy(MAX_X + 12, 29);
	printf("游戏说明");
	gotoxy(MAX_X + 5, 30);
	printf("═══════════════════╗");
	for (int y = 31; y < 37; y++)
	{
		gotoxy(MAX_X + 24, y);
		printf("║");
	}
	gotoxy(MAX_X + 5, 37);
	printf("═══════════════════╝");
	ansi_fg_green();
	gotoxy(MAX_X + 10, 31);
	printf("控制键");
	gotoxy(MAX_X + 14, 32);
	printf("w");
	gotoxy(MAX_X + 12, 33);
	printf("a s d");
	gotoxy(MAX_X + 10, 35);
	printf("发射键");
	gotoxy(MAX_X + 12, 36);
	printf("空格键");
	ansi_reset();
}
