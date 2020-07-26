#include "Enemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "Engine.h"
#include "TextureManager.h"

Enemy::Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0) {
	m_health = 40;
	m_healthBarGreen = new Sprite({ 0,25,100,9 }, { d.x,d.y - 16, 40.0, 4.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
	m_healthBarRed = new Sprite({ 0,16,100,9 }, { d.x,d.y - 16, 40.0, 4.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
	m_accelX = m_accelY = m_velX = m_velY = 0.0;
	m_maxVelX = 5.0;
	m_ePos.x = m_dst.x;
	m_ePos.y = m_dst.y;

}

void Enemy::Update(bool a, std::vector<PathNode*> b)
{
	if (a)
	{
		if (this->m_dst.x < (b[m_targetnode]->GetPos().x - 16)) {
			++m_dst.x;
		}
		if (this->m_dst.x > (b[m_targetnode]->GetPos().x - 16)) {
			--m_dst.x;
		}
		if (this->m_dst.y < (b[m_targetnode]->GetPos().y - 16)) {
			++m_dst.y;
		}
		if (this->m_dst.y > (b[m_targetnode]->GetPos().y - 16)) {
			--m_dst.y;
		}
		if (this->m_dst.x == (b[m_targetnode]->GetPos().x - 16) && this->m_dst.y == (b[m_targetnode]->GetPos().y - 16)){
			if (m_targetnode == 62)
				m_targetnode = 0;
			++m_targetnode;
			
		}

	}
	switch (m_state)
	{
	case idle:
		break;
	case running:
		break;
	}
	Animate();
	m_healthBarRed->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	m_healthBarGreen->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
}

void Enemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
}

void Enemy::StopX() { m_velX = 0.0; }
void Enemy::StopY() { m_velY = 0.0; }
void Enemy::SetAccelX(double a) { m_accelX = a; }
void Enemy::SetAccelY(double a) { m_accelY = a; }
double Enemy::GetVelX() { return m_velX; }
double Enemy::GetVelY() { return m_velY; }
glm::vec2 Enemy::getPos()
{
	return m_ePos;
}
void Enemy::SetX(float y) { m_dst.x = y; }
void Enemy::SetY(float y) { m_dst.y = y; }

void Enemy::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = m_spriteMin = 0;
		m_spriteMax = 3;
	}
	else // Only other is running for now...
	{
		m_sprite = m_spriteMin = 0;
		m_spriteMax = 3;
	}
}
