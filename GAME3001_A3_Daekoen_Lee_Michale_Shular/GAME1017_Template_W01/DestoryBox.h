#pragma once
#ifndef __DestroyBox_H__
#define __DestroyBox_H__
#include "Sprite.h"

class DestroyBox :public Sprite
{
private:
	bool destroy;
	int hp;

public:
	DestroyBox(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t)
		:Sprite(s,d ,r ,t){
		hp = 4;
		destroy = false;
	}
	bool IsDestroyed() { return destroy; }
	void getDmg() 
	{ 

		if (hp > 0)
			--hp;
		else
		{
			setSrcP(0, 153);
			destroy = true;
		}
	}
};


#endif // !__DestroyBox_H__

