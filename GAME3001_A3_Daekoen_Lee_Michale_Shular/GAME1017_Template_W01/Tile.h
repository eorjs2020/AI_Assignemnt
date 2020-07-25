#pragma once
#ifndef _TILE_H_
#define _TILE_H_
#include "Sprite.h"

class Tile : public Sprite
{
public:
	Tile(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, const bool o, const bool h)
		:Sprite(s, d, r, t), m_obstacle(o), m_hazard(h){}
	bool IsObstacle() { return m_obstacle; }
	bool IsHazard() { return m_hazard; }
	Tile* Clone() { return new Tile(m_src, m_dst, m_pRend, m_pText, m_obstacle, m_hazard); }
private:
	bool m_obstacle,
		m_hazard;
};
#endif
