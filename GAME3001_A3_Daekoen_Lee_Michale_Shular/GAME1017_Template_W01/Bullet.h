#pragma once
#ifndef _BULLET_H_
#define _BULLET_H_
#include "Sprite.h"
#include "Player.h"
#include "glm.hpp"

class Bullet : public Sprite
{
private:
	
	int m_speed;
	bool m_active;
	double dx, dy, angle;
	glm::vec2 m_dir;
	glm::vec2 m_position;
public:
	Bullet(SDL_Rect src, SDL_FRect dst, SDL_Renderer* r, SDL_Texture* t, int spd, glm::vec2 vec);
	void Update();
	void Render();

};


#endif // _BULLET_H_

