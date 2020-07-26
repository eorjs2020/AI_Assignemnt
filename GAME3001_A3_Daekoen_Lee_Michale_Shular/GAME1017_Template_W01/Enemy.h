#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Sprite.h"
#include "vec2.hpp"

class Enemy : public AnimatedSprite
{
public:
	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update();
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
private:
	enum state { idle, running } m_state;
	bool m_dir;
	void SetState(int s);
	int m_health;
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
};

#endif