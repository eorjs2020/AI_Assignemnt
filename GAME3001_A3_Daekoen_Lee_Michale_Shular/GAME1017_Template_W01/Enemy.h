#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Sprite.h"
#include "Player.h"
#include "DebugManager.h"
#include "vec2.hpp"
#include <vector>
#include "PathNode.h"
#include  "Tile.h"
#include "Bullet.h"

class MeleeEnemy : public AnimatedSprite
{
public:
	MeleeEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, std::vector<Tile*> obs ,int sstart, int smin, int smax, int nf);
	~MeleeEnemy();
	void Update(Player* player, bool a, std::vector<PathNode*> b, std::vector<PathNode*> hiding);
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
	void SetVs(const double angle);
	void Move2Full(double& angle);
	glm::vec2 getVel() { return m_Vel; }
	glm::vec2 getPos();
	void setPosX(double x) { m_dst.x += x; }
	void setPosY(double y) { m_dst.y += y; }
	void RenderRadius(int rad, int x, int y);
	bool getAlive() { return m_alive; }
	void SetLineofSight(bool los) { m_bLOS = los; }
	bool HasLineofSight() {return  m_bLOS; }
private:
	enum state { idle, chasing, flee ,death } m_state;
	enum chasing_state 
	{
		a_chasing,
		melee_attack
	} attacksate;
	bool m_dir, m_alive = true, m_canHit, m_wait;
	void SetState(int s);
	int m_health, m_targetnode = 1, m_alivetimer = 0, hitTimer, m_waitingTimer, m_waitingTimerCheck;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
	Sprite* m_sword;
	double m_accelX,
		m_accelY,
		m_velX,
		m_maxVelX,
		m_velY,
		m_maxVelY, 
		destAngle, 
		m_dirtionXCheck;
	glm::vec2 m_ePos;
	glm::vec2 m_Vel;
	int m_iLOS, alarm;
	bool m_bLOS;
	bool m_bSearch;
	SDL_FRect m_rSearch;
	double dx, dy, x, y;
	std::vector<Tile*> obs;
	
};

class RangeEnemy : public AnimatedSprite
{
public:
public:
	RangeEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, std::vector<Bullet*>* EB , std::vector<Tile*> obs, int sstart, int smin, int smax, int nf);
	~RangeEnemy();
	void Update(Player* player, bool a, std::vector<PathNode*> b, std::vector<PathNode*> hiding);
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
	void SetVs(const double angle);
	void Move2Full(double& angle);
	glm::vec2 getVel() { return m_Vel; }
	glm::vec2 getPos();
	void setPosX(double x) { m_dst.x += x; }
	void setPosY(double y) { m_dst.y += y; }
	void RenderRadius(int rad, int x, int y);
	bool getAlive() { return m_alive; }
	void SetLineofSight(bool los) { m_bLOS = los; }
	bool HasLineofSight() { return  m_bLOS; }
private:
	enum state { idle, chasing, flee, death } m_state;
	enum chasing_state
	{
		a_chasing,
		Ranged_attack,
	} attacksate;
	bool m_dir, m_alive = true, m_canHit;
	void SetState(int s);
	int m_health, m_targetnode = 1, m_alivetimer = 0, hitTimer;
	Sprite* m_healthBarGreen;
	Sprite* m_healthBarRed;
	Sprite* m_sword;
	double m_accelX,
		m_accelY,
		m_velX,
		m_maxVelX,
		m_velY,
		m_maxVelY,
		destAngle,
		m_dirtionXCheck;
	glm::vec2 m_ePos;
	glm::vec2 m_Vel;
	int m_iLOS, alarm;
	bool m_bLOS;
	bool m_bSearch;
	SDL_FRect m_rSearch;
	double dx, dy, x, y;
	std::vector<Tile*> obs;
	std::vector<Bullet*>* bVec;

};

#endif