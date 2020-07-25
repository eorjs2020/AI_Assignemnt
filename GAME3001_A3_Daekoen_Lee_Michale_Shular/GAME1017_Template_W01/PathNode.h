#pragma once

#include "Sprite.h"
class PathNode
{
public:
	PathNode();
	~PathNode();

	void Render();
	void Update();
	
	void setLOS(bool LOS);
	bool getLOS() const;


private:
	bool m_bLOS;
};

