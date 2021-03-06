#pragma once
#ifndef _STATES_H_
#define _STATES_H_
#include <map>
#include "Tile.h"
#include "Player.h"
#include <SDL.h>
#include <vector>
#include "PathNode.h"
#include "Button.h"
#include "Label.h"
#include "Enemy.h"
#include "Bullet.h"
#include "glm.hpp"
class State // This is the abstract base class for all specific states.
{
public: // Public methods.
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();

protected: // Private but inherited.
	State() {}
};

class PlayState :public State
{
private:
	std::vector<SDL_FRect> m_pLOSobs;
	std::map<char, Tile*> m_tiles;
	Player* m_pPlayer;
	std::vector<PathNode*> m_pGrid;
	std::vector<Tile*> m_pObstacle;
	std::vector<Tile*> m_pHazrad;
	std::vector<PathNode*> m_pPatrolPathOne;
	std::vector<PathNode*> m_pPatrolPathTwo;
	std::vector<PathNode*> m_pPatrolPathThree;
	std::vector<PathNode*> m_pPatrolPathFour;
	std::vector<Bullet*> m_pPlayerBullet;
	std::vector<Bullet*> m_pEnemyBullet;
	std::vector<MeleeEnemy*> m_Enemy;
	std::vector<RangeEnemy*> m_RangeEnemy;
	glm::vec2 m_pMousePos;
	bool m_bPBNull;
	bool m_bEBNull = false;
	bool m_bCanShoot = true;
	bool PlayerHasLinofSight;
	bool PlayerHasLinofSight1;
	bool PlayerHasLinofSight2;
	bool PlayerHasLinofSight3;
	bool PlayerHasLinofSight4;
	bool PlayerHasLinofSight5;
	bool m_Debugmode = false, m_PatrolMode = false,
		m_winCondition[4] = { false, false, false, false },
		m_newGame, m_loseGame;
	int targetNode = 1, m_hitCoolDown = 0, m_score = 0, m_enemyRespawnTimer[4] = { 0, 0, 0, 0 };
	std::vector<PathNode*> * TempPForEnemyPath;
	std::vector<PathNode*> m_HidingNode;
	Label* m_gamestatus; 
	std::string m_enemiesKilled = "Enemies Killed: 0";
	Sprite* m_win, * m_lose;

	Button* m_restart;

public:
	PlayState();
	void Update();
	void Render();
	void Enter();
	void RenderGrid();
	void RenderLOS();
	void SetLOS();
	void Exit();
	void Resume();
	void CheckCollision();
	void m_buildPatrolPath();
	void m_displayPatrolPath();
	bool LOSNode(int n);
	bool EnemyHasLOS(int n);
	bool RangeHasLos();
};

class StartState :public State
{
private:
	Label* m_nameOne,* m_nameTwo;
	Button* m_StartBtn;
	Sprite* m_background;

public:
	StartState();
	void Update();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};


#endif