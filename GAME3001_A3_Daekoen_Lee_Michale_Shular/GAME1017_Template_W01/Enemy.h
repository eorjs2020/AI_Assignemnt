#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Sprite.h"
#include "Player.h"
#include "DebugManager.h"
#include "vec2.hpp"
#include <vector>
#include "PathNode.h"

class Enemy : public AnimatedSprite
{
public:
	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update(Player* player, bool a, std::vector<PathNode*> b);
	void Render();
	int getHealth() { return m_health; }
	void setHealth(int a) { m_health += a; }
	void StopX();
	void StopY();
	void SetAccelX(double a);
	void SetAccelY(double a);
	void SetX(float y);
	void SetY(float y);
	double GetVelX();
	double GetVelY();
	glm::vec2 getVel() { return m_Vel; }
	glm::vec2 getPos();
	void setPosX(double x) { m_dst.x += x; }
	void setPosY(double y) { m_dst.y += y; }
	void RenderRadius(int rad, int x, int y);
	
	
private:
	enum state { idle, running } m_state;
	bool m_dir;
	void SetState(int s);
	int m_health, m_targetnode = 1;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
	double m_accelX,
		m_accelY,
		m_velX,
		m_maxVelX,
		m_velY,
		m_maxVelY;
	glm::vec2 m_ePos;
	glm::vec2 m_Vel;
	int m_iLOS;
	bool m_bLOS;
	bool m_bSearch;
	SDL_FRect m_rSearch;
	double dx, dy, x, y;
};

#endif