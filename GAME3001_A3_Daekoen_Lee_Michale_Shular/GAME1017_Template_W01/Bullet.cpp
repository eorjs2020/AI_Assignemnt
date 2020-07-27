#include "Bullet.h"
#include "MathManager.h"



Bullet::Bullet(SDL_Rect src, SDL_FRect dst, SDL_Renderer* r, SDL_Texture* t, int spd, glm::vec2 vec) : Sprite (src, dst, r, t), m_speed(spd), m_dir(vec)
{
	m_position.x = dst.x;
	m_position.y = dst.y;
}

void Bullet::Update()
{
	
	
	auto targetVector = MAMA::normalize(m_dir - m_position);
	m_dst.x += targetVector.x * m_speed;
	m_dst.y += targetVector.y * m_speed;
	double a = MAMA::AngleBetweenPoints(m_dir.y - (m_dst.y + m_dst.h / 2), m_dir.x - (m_dst.x + m_dst.w / 2));

	angle = MAMA::Rad2Deg(a);
}

void Bullet::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), angle, 0, SDL_FLIP_NONE);
}
