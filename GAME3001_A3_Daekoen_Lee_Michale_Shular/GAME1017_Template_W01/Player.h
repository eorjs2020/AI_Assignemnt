#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "glm.hpp"
#include "Sprite.h"

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update(Sprite * a[], int b[]);
	void Render();
	int getHealth() { return m_health; }
	void setHealth(int a) { m_health += a; }
	int GetDir();
	AnimatedSprite getSword() { return *m_sword; }
	bool getAttack() { return m_attack; }
	bool getOneAttack() { return m_Oneattack; }
	void setOneAttack(bool a) { m_Oneattack = a; }
	bool getAlive() { return m_alive; }
private:
	enum state { idle, running, dead } m_state;
	bool m_dir, m_attack= false, m_Oneattack = false, m_alive, m_boxColl;
	void SetState(int s);
	int m_health, m_attackTimer, m_alivetimer;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
	AnimatedSprite* m_sword;
	enum BullDir
	{
		RIGHT,
		DOWN,
		LEFT,
		UP
	} m_pBulletDir;

};

#endif

