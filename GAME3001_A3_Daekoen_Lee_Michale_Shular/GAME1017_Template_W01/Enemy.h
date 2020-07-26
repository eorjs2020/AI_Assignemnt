#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Sprite.h"

class Enemy : public AnimatedSprite
{
public:
	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update();
	void Render();
	int getHealth() { return m_health; }
	void setHealth(int a) { m_health += a; }
private:
	enum state { idle, running } m_state;
	bool m_dir;
	void SetState(int s);
	int m_health;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
};

#endif