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
	std::map<char, Tile*> m_tiles;
	Player* m_pPlayer;
	std::vector<PathNode*> m_pGrid;

	Enemy* m_Enemy;
	bool m_Debugmode = false, m_PatrolMode = false;
	
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
};

class StartState :public State
{
private:
	Label* m_nameOne,* m_nameTwo;
	Button* m_StartBtn;


public:
	StartState();
	void Update();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};


#endif