#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Sprite.h"
#include "Player.h"
#include "DebugManager.h"

class Enemy : public AnimatedSprite
{
public:
	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update(Player* player);
	void Render();
	int getHealth() { return m_health; }
	void setHealth(int a) { m_health += a; }
	void RenderRadius(int rad, int x, int y);
	
	
private:
	enum state { idle, running } m_state;
	bool m_dir;
	void SetState(int s);
	int m_health;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
	int m_iLOS;
	bool m_bLOS;
	bool m_bSearch;
	SDL_FRect m_rSearch;
	double dx, dy, x, y;
};

#endif