#include "DxLib.h"
#include <math.h>
#define SHOT_MAX 50
#define MOB_COUNT 7400
#define PI 3.1415926F
#define ANIM_FRAMES 30
#define BOMB_ANIM_NUM 20
struct POINT2DF {
	float x = 0;
	float y = 0;
};
struct POINT2D {
	int x = 0;
	int y = 0;
};
struct CHARADATA {
	 struct POINT2D move,pos;
	 int r = 0;
	 unsigned int color = 0;
	 int life = 0;
	 struct POINT2DF p;
	 int count=0;
};
struct MOBDATA {
	int type = 0;
	int spawn_pointX = 0;
	int spawn_pointY = 0;
	int spawn_interval = 90;
	int move = 640;
	int life = 0;
	int effectIndex = 0;
};
struct TIMER {
	int start = 0;
	int end = 0;
	int interval = 1;
};
struct MOBSHOT {
	double EShotX = 0;
	double EShotY = 0;
	double EShotSx = 0;
	double EShotSy = 0;
	double sb = 0;
	double sbx = 0;
	double sby = 0;
	double shotX = 0;
	double shotY = 0;
	int playerNowPosX = 0;
	int playerNowPosY = 0;
	int life = 0;
};
 int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lpC, int nC)
 {
	 const float deg2rad = PI / 180;
	 int deg = 0, dDeg = 10;
	 int r = 40;
	 int hit=0;
	 int rank[3]{0,0,0};
	 int bombSingleImageWidth=192,bombSingleImageHeight=192;
	 const int screenW = 640, screenH = 980;
	 const float dt = 1.0F / 60;
	 unsigned int scrollCount = 0;
	 int destX[2], srcX[2], srcL[2], imgW, imgH, mobcount = 0, MobShot_speed=1;
	 char keyBuf[256], keyBufOld[256];
	 int bomb[BOMB_ANIM_NUM];
	 int bombCount = 0;
	 int overflag = 0;
	 float playerMove = 160;
	 struct POINT2DF playerPos = { 100, 240 };
	 struct CHARADATA shot[SHOT_MAX],shot_ch, enemy, enemy2;
	 struct MOBDATA mob[MOB_COUNT];
	 struct TIMER time;
	 struct MOBSHOT eshot[MOB_COUNT];
	 ChangeWindowMode(TRUE);
	 if (DxLib_Init() == -1)
		 return -1;
	 SetDrawScreen(DX_SCREEN_BACK);
	 LoadDivGraph("..\\sozai\\grenade_Bomb.png", BOMB_ANIM_NUM, 5, 4, bombSingleImageWidth,bombSingleImageHeight, bomb);
	 enemy.r = 10;
	 for (int i = 0; i < SHOT_MAX; i++)
		shot[i].life = 0;
	 shot_ch.life = 0;
	 for (int i = 0; i < MOB_COUNT; i++) {
		 mob[i].type = GetRand(1);
		 mob[i].spawn_pointX = GetRand(200)+400;
		 mob[i].spawn_pointY = GetRand(420)+20;
		 mob[i].life = 1;
		 mob[i].effectIndex = -1;
		 eshot[i].shotX = mob[i].spawn_pointX;
		 eshot[i].shotY = mob[i].spawn_pointY;
	 }
	 for (int i = 0; i < MOB_COUNT; i++) {
		 if (mob[i].type == 1)
			 mob[i].life = 2;
	 }
	 int bgImg = LoadGraph("..\\sozai\\無題3_20221228201633.png");
	 int mainchar = LoadGraph("..\\sozai\\2091142.png");
	 int mobA = LoadGraph("..\\sozai\\IMG_4432.png");
	 int mobB = LoadGraph("..\\sozai\\IMG_4433.png");
	 int shotSE = LoadSoundMem("..\\sozai\\ショット.mp3");
	 int shot_zSE = LoadSoundMem("..\\sozai\\手裏剣を投げる.mp3");
	 int explosionSE = LoadSoundMem("..\\sozai\\魔王魂  レトロ12.mp3");
	 int backBGM = LoadSoundMem("..\\sozai\\魔王魂  8bit05.mp3");
	 GetGraphSize(bgImg, &imgW, &imgH);
	 unsigned int white = GetColor(255, 255, 255);
	 unsigned int blue = GetColor(0, 255, 255);
	 unsigned int red = GetColor(255, 0, 0);
	 unsigned int yellow = GetColor(255,255,0);
	 const int refreshRate = 60;
	 const int animNext = refreshRate / ANIM_FRAMES;
	 GetHitKeyStateAll(keyBufOld);
	 time.start=GetNowCount();
	 PlaySoundMem(backBGM, DX_PLAYTYPE_LOOP);
	 while (ProcessMessage() != -1) {
		 GetHitKeyStateAll(keyBuf);
		 if (keyBuf[KEY_INPUT_ESCAPE] == 1)
			 break;
		 if (keyBuf[KEY_INPUT_RIGHT] == 1 && playerPos.x < 600) 
			 playerPos.x += playerMove * dt;
		 if (keyBuf[KEY_INPUT_LEFT] == 1 && playerPos.x > 0) 
			 playerPos.x -= playerMove * dt;
		 if (keyBuf[KEY_INPUT_DOWN] == 1 && playerPos.y < 435) 
			 playerPos.y += playerMove * dt;
		 if (keyBuf[KEY_INPUT_UP] == 1 && playerPos.y > 0) 
			 playerPos.y -= playerMove * dt;
		 if (keyBuf[KEY_INPUT_Z] == 1 && keyBufOld[KEY_INPUT_Z] == 0) { 
			 for (int i = 0; i < SHOT_MAX; i++)
			 {
				 if (!shot[i].life) { 
					 shot[i].life = 1; 
					 shot[i].pos.x = playerPos.x + 20; 
					 shot[i].pos.y = playerPos.y + 20; 
					 shot[i].move.x = 3; 
					 shot[i].move.y = 0;
					 shot[i].r = 4;
					 shot[i].color = GetColor(255, 255, 255);
					 break;
				 }
			 }
			 PlaySoundMem(shotSE, DX_PLAYTYPE_BACK);
		 }
		 if (keyBuf[KEY_INPUT_X] == 1 && keyBufOld[KEY_INPUT_X] == 0) {
			 if (!shot_ch.life) {
				 shot_ch.life = 1;
				 shot_ch.pos.x = playerPos.x + 20;
				 shot_ch.pos.y = playerPos.y + 20;
				 shot_ch.move.x = 3;
				 shot_ch.move.y = 0;
				 shot_ch.color = GetColor(255, 255, 255);
			 }
			 if (shot_ch.count < 5 && shot_ch.pos.x == playerPos.x + 20)
				 PlaySoundMem(shot_zSE, DX_PLAYTYPE_BACK);
		}
		 for (int i = 0; i < SHOT_MAX; i++)
		 {
			 if (shot[i].life) { 
				 shot[i].pos.x += shot[i].move.x; 
				 if (shot[i].pos.x > 640)
					 shot[i].life = 0;
			 }
		 }
		 if (shot_ch.life) {
			 float rad = deg * deg2rad;
			 shot_ch.pos.x += shot_ch.move.x;
			 enemy.p.x = (float)(r * sin(rad) + (int)shot_ch.pos.x);
			 enemy.p.y = (float)(-r * cos(rad) + shot_ch.pos.y);
			 enemy2.p.x = (float)(r * cos(rad) + (int)shot_ch.pos.x);
			 enemy2.p.y = (float)(r * sin(rad) + shot_ch.pos.y);
			 deg += dDeg;
			 if (deg >= 360)
				 deg = 0;
			 if (shot_ch.pos.x > 640) {
				 shot_ch.life = 0;
				 shot_ch.count++;
			 }
		 }
		 destX[0] = 0;
		 srcX[0] = scrollCount % imgW;
		 srcL[0] = imgW - srcX[0];
		 if (srcL[0] > screenW)
			 srcL[0] = screenW;
		 destX[1] = srcL[0];
		 srcX[1] = 0;
		 srcL[1] = screenW - srcL[0];
		 for (int i = 0; i < 256; i++)
			 keyBufOld[i] = keyBuf[i];
		 if (scrollCount % mob->spawn_interval == 0 && mobcount < MOB_COUNT) {
			 mobcount++;
		 }
		 time.end = GetNowCount();
		 time.interval = (time.end - time.start) / 1000;
		 DrawRectGraph(destX[0], 0, srcX[0], 0, srcL[0], screenH, bgImg, FALSE, FALSE);
		 DrawRectGraph(destX[1], 0, srcX[1], 0, srcL[1], screenH, bgImg, FALSE, FALSE);
		 DrawExtendGraph((int)playerPos.x, (int)playerPos.y, (int)playerPos.x + 40, (int)playerPos.y + 40, mainchar, TRUE);
		 DrawLine((int)playerPos.x + 250, (int)playerPos.y + 10, (int)playerPos.x + 250, (int)playerPos.y + 30, blue);
		 DrawLine((int)playerPos.x + 240, (int)playerPos.y + 20, (int)playerPos.x + 260, (int)playerPos.y + 20, blue);
		 for (int i = 0; i < SHOT_MAX; i++)
		 {
			 if (shot[i].life) {
				 DrawCircle(shot[i].pos.x, shot[i].pos.y, shot[i].r, shot[i].color, TRUE);
			 }
		 }
		 if (shot_ch.count < 5) {
			 if (shot_ch.life) {
				 DrawOval((int)enemy.p.x, (int)enemy.p.y, enemy.r + 10, enemy.r - 4, white, TRUE);
				 DrawOval((int)enemy.p.x, (int)enemy.p.y, enemy.r - 4, enemy.r + 10, white, TRUE);
			 }
		 }
		 for (int i = 0; i < SHOT_MAX; i++) {
			 for (int j = 0; j < MOB_COUNT; j++)
			 {
				 if (mob[j].effectIndex == -1) {
					 if (mob[j].life > 0) {
						 if (shot[i].life) {
							 if (shot[i].pos.x > mob[j].move && shot[i].pos.y > mob[j].spawn_pointY &&
								 shot[i].pos.x < mob[j].move + 70 && shot[i].pos.y < mob[j].spawn_pointY + 70) {
								 shot[i].life = 0;
								 PlaySoundMem(explosionSE, DX_PLAYTYPE_BACK);
								 mob[j].effectIndex = 0;
							 }
						 }
						 if (shot_ch.life && shot_ch.count < 5) {
							 if ((int)enemy.p.x - 10 > mob[j].move && (int)enemy.p.y - 10 > mob[j].spawn_pointY &&
								 (int)enemy.p.x + 10 < mob[j].move + 70 && (int)enemy.p.y + 10 < mob[j].spawn_pointY + 70) {
								 PlaySoundMem(explosionSE, DX_PLAYTYPE_BACK);
								 mob[j].effectIndex = 0;
							 }
						 }
					 }
				 }
			 }
		 }
		 for (int i = 0; i < MOB_COUNT; i++) {
			 if (mob[i].effectIndex >= 0) {
				 if (bombCount == animNext) {
					 bombCount = 0;
					 mob[i].effectIndex++;
					 if (mob[i].effectIndex == BOMB_ANIM_NUM)
					 {
						 mob[i].life--;
						 mob[i].effectIndex = -1;
						
					 }
				 }
				 bombCount++;
			 }
		 }
		 for (int i = 0; i < mobcount; i++) {
			 if (!mob[i].type) {
				 if (mob[i].life == 1) {
					 if (mob[i].effectIndex == -1) {
						 DrawExtendGraph(mob[i].move, mob[i].spawn_pointY, mob[i].move + 70, mob[i].spawn_pointY + 70, mobA, TRUE);
						 if (mob[i].move > mob[i].spawn_pointX) {
							 mob[i].move--;
							 eshot[i].playerNowPosX = (int)playerPos.x;
							 eshot[i].playerNowPosY = (int)playerPos.y;
						 }
						 if (mob[i].move == mob[i].spawn_pointX) {
							 eshot[i].sbx = mob[i].spawn_pointX - eshot[i].playerNowPosX;
							 eshot[i].sby = mob[i].spawn_pointY - eshot[i].playerNowPosY;
							 eshot[i].sb = sqrt(eshot[i].sbx * eshot[i].sbx + eshot[i].sby * eshot[i].sby);
							 eshot[i].EShotSx = eshot[i].sbx / eshot[i].sb * MobShot_speed;
							 eshot[i].EShotSy = eshot[i].sby / eshot[i].sb * MobShot_speed;
							 eshot[i].shotX -= eshot[i].EShotSx;
							 eshot[i].shotY -= eshot[i].EShotSy;
							 DrawCircle((int)eshot[i].shotX, (int)eshot[i].shotY + 30, 5, yellow, TRUE);
							 if (eshot[i].shotX < 0) {
								 eshot[i].shotX = mob[i].spawn_pointX;
								 eshot[i].shotY = mob[i].spawn_pointY;
								 eshot[i].playerNowPosX = (int)playerPos.x;
								 eshot[i].playerNowPosY = (int)playerPos.y;
							 }
							 if ((int)eshot[i].shotX > (int)playerPos.x && (int)eshot[i].shotY + 30 > (int)playerPos.y
								 && (int)eshot[i].shotX < (int)playerPos.x + 40 && (int)eshot[i].shotY + 30 < (int)playerPos.y + 40)
								 overflag = 1;

						 }
					 }
					 else {
						 DrawGraph((int)mob[i].spawn_pointX - bombSingleImageWidth / 2, (int)mob[i].spawn_pointY - bombSingleImageHeight / 2, bomb[mob[i].effectIndex], TRUE);
						 hit++;
					 }
				 }
				 if (mob[i].life == 0 || mob[i].effectIndex != -1) {
					 eshot[i].sbx = mob[i].spawn_pointX - eshot[i].playerNowPosX;
					 eshot[i].sby = mob[i].spawn_pointY - eshot[i].playerNowPosY;
					 eshot[i].sb = sqrt(eshot[i].sbx * eshot[i].sbx + eshot[i].sby * eshot[i].sby);
					 eshot[i].EShotSx = eshot[i].sbx / eshot[i].sb * MobShot_speed;
					 eshot[i].EShotSy = eshot[i].sby / eshot[i].sb * MobShot_speed;
					 eshot[i].shotX -= eshot[i].EShotSx;
					 eshot[i].shotY -= eshot[i].EShotSy;
					 if (!eshot[i].life)
						 DrawCircle((int)eshot[i].shotX, (int)eshot[i].shotY + 30, 5, yellow, TRUE);
					 if (eshot[i].shotX < 0) {
						 eshot[i].life = 0;
					 }
					 if ((int)eshot[i].shotX > (int)playerPos.x && (int)eshot[i].shotY + 30 > (int)playerPos.y
						 && (int)eshot[i].shotX < (int)playerPos.x + 40 && (int)eshot[i].shotY + 30 < (int)playerPos.y + 40)
						 overflag = 1;
				 }
			 }
			 if (mob[i].type) {
				 if (mob[i].life == 2) {
					 	 DrawExtendGraph(mob[i].move, mob[i].spawn_pointY, mob[i].move + 70, mob[i].spawn_pointY + 70, mobB, TRUE);
						 DrawLine(mob[i].move, mob[i].spawn_pointY + 10, mob[i].move, mob[i].spawn_pointY + 60, blue);
						 if (mob[i].move > mob[i].spawn_pointX)
							 mob[i].move--;
						 if (mob[i].move == mob[i].spawn_pointX) {
							 eshot[i].shotX = eshot[i].shotX - 1 * MobShot_speed;
							DrawCircle(eshot[i].shotX, mob[i].spawn_pointY + 30, 5, blue, TRUE);		
							 if (eshot[i].shotX < 0) {
								 eshot[i].shotX = mob[i].spawn_pointX;
							 }
							 if ((int)eshot[i].shotX > (int)playerPos.x && (int)eshot[i].shotY + 30 > (int)playerPos.y
								 && (int)eshot[i].shotX < (int)playerPos.x + 40 && (int)eshot[i].shotY + 30 < (int)playerPos.y + 40)
								 overflag = 1;
						 }
				 }
				 else if (mob[i].life == 1) {
					 if (mob[i].effectIndex == -1) {
						 DrawExtendGraph(mob[i].move, mob[i].spawn_pointY, mob[i].move + 70, mob[i].spawn_pointY + 70, mobB, TRUE);
						 if (mob[i].move > mob[i].spawn_pointX)
							 mob[i].move--;
						 if (mob[i].move == mob[i].spawn_pointX) {	
							 eshot[i].shotX = eshot[i].shotX - 2 * MobShot_speed;
							DrawCircle(eshot[i].shotX, mob[i].spawn_pointY + 30, 5, blue, TRUE);
							 if (eshot[i].shotX < 0) {
								 eshot[i].shotX = mob[i].spawn_pointX;
							 }
							 if ((int)eshot[i].shotX > (int)playerPos.x && (int)eshot[i].shotY + 30 > (int)playerPos.y
								 && (int)eshot[i].shotX < (int)playerPos.x + 40 && (int)eshot[i].shotY + 30 < (int)playerPos.y + 40)
								 overflag = 1;
						 }
					 }
					 else {
						 DrawGraph((int)mob[i].spawn_pointX - bombSingleImageWidth / 2, (int)mob[i].spawn_pointY - bombSingleImageHeight / 2, bomb[mob[i].effectIndex], TRUE);
						 hit++;
					 }
				 }
				 if (mob[i].life == 0 || mob[i].effectIndex !=-1) {
					 eshot[i].shotX = eshot[i].shotX - 2 * MobShot_speed;
					 if (!eshot[i].life)
						 DrawCircle(eshot[i].shotX, mob[i].spawn_pointY + 30, 5, blue, TRUE);
					 if (eshot[i].shotX < 0) {
						 eshot[i].life = 0;
					 }
					 if ((int)eshot[i].shotX > (int)playerPos.x && (int)eshot[i].shotY + 30 > (int)playerPos.y
						 && (int)eshot[i].shotX < (int)playerPos.x + 40 && (int)eshot[i].shotY + 30 < (int)playerPos.y + 40)
						 overflag = 1;
				 }
			 }
		 }	 
		 if (overflag == 1) {
			 int rastscore = 0;
			 int tmp = 0, tmp2 = 0;
			 DrawBox(0, 0, 640, 480, white, TRUE);
			 DrawFormatString(10, 10, blue, "今回のスコア：%d00", time.interval*hit);
			 if (rank[2] < time.interval * hit) {
				 rank[2] = time.interval* hit;
			 }
			 if (rank[1] < time.interval * hit) {
				 rank[2] = rank[1];
				 rank[1] = time.interval * hit;
			 }
			 if (rank[0] < time.interval * hit) {
				 rank[1] = rank[0];
				 rank[0] = time.interval * hit;
			 }
			 DrawFormatString(10, 30, blue, "Aでリスタート");
			 DrawFormatString(10, 60, blue, "A以外のキーで終了");
			 DrawFormatString(10, 90, blue, "＜ランキング＞");
			 DrawFormatString(10, 120, blue, "１位:%d00",rank[0]);
			 DrawFormatString(10, 150, blue, "２位:%d00", rank[1]);
			 DrawFormatString(10, 180, blue, "３位:%d00", rank[2]);
			 ScreenFlip();
			 WaitTimer(300);
			 WaitKey();
			 GetHitKeyStateAll(keyBuf);
			 if (keyBuf[KEY_INPUT_A] == 1) {
				 playerPos = { 100, 240 };
				 time.start = GetNowCount();
				 for (int i = 0; i < SHOT_MAX; i++)
					 shot[i].life = 0;
				 shot_ch.life = 0;
				 for (int i = 0; i < MOB_COUNT; i++) {
					 mob[i].type = GetRand(1);
					 mob[i].spawn_pointX = GetRand(200) + 400;
					 mob[i].spawn_pointY = GetRand(420) + 20;
					 mob[i].life = 1;
					 mob[i].effectIndex = -1;
					 eshot[i].shotX = mob[i].spawn_pointX;
					 eshot[i].shotY = mob[i].spawn_pointY;
					 mob[i].move = 640;
				 }
				 for (int i = 0; i < MOB_COUNT; i++) {
					 if (mob[i].type == 1)
						 mob[i].life = 2;
				 }
				 shot_ch.count = 0;
				 mobcount = 0;
				 hit = 0;
				 overflag = 0;
				 scrollCount = 0;	 
			 }
			 if (keyBuf[KEY_INPUT_A] != 1) 
				 break;
		 }
		 	 ScreenFlip();
			 scrollCount++;
	 }
	 DxLib_End();
	 return 0;
 }