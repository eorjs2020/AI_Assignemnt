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
	m_bSearch = false;
	m_accelX = m_accelY = m_velX = m_velY = 0.0;
	m_maxVelX = 5.0;
	m_ePos.x = m_dst.x;
	m_ePos.y = m_dst.y;

}

Enemy::~Enemy()
{
}



void Enemy::Update(Player* player, bool a, std::vector<PathNode*> b)
{
	dx = dy = 0.0f;
	m_rSearch.x = (m_dst.x + m_dst.w /2) - 100 ;
	m_rSearch.y = (m_dst.y + m_dst.h / 2) - 100;
	m_rSearch.w = 200;
	m_rSearch.h = 200;
	
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
			if (m_targetnode == b.size() - 1)
				m_targetnode = 0;
			++m_targetnode;
		}

	}
	switch (m_state)
	{
	case idle:
		if (m_health <= 0)
		{
			SetState(death);
		}
		break;
	case death:
		++m_alivetimer;
			if (m_alivetimer >= 17){ 
				m_alive = false;
			}
		break;
	case running:
		break;
	}
	Animate();
	m_healthBarRed->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	m_healthBarGreen->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	SDL_FRect tempP = { player->GetDstP()->x, player->GetDstP()->y, player->GetDstP()->w, player->GetDstP()->h };
	m_bSearch = COMA::CircleAABBCheck({ m_rSearch.x + m_rSearch.w / 2, m_rSearch.y + m_rSearch.h / 2 }, 100, tempP);
	if (m_bSearch && m_health > 10)
	{
		if (m_dst.x > player->GetDstP()->x && m_dst.x < player->GetDstP()->w + player->GetDstP()->x)
			x = player->GetDstP()->w;
		else
			x = -GetDstP()->w;
		if (m_dst.y > player->GetDstP()->y)
			y = player->GetDstP()->h;
		else
			y = -GetDstP()->h;
		SDL_Rect temp1 = { MAMA::ConvertFRect2Rect(*GetDstP()) };

		SDL_Rect temp2 = { player->GetDstP()->x + x, player->GetDstP()->y + y, player->GetDstP()->w / 2.0f, player->GetDstP()->h / 2.0f };
		SDL_Rect temp3 = { player->GetDstP()->x, player->GetDstP()->y, player->GetDstP()->w, player->GetDstP()->h };
		

		
		double a = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2 + y) - (GetDstP()->y + GetDstP()->h / 2),
			(player->GetDstP()->x + player->GetDstP()->w / 2) - (GetDstP()->x + GetDstP()->w / 2) + x);
		MAMA::SetDeltas(a, dx, dy, 2.0, 2.0);
		if (SDL_HasIntersection(&temp2, &temp1)) {
			dx = dy = 0.0f;
		}
		GetDstP()->x += (int)round(dx);
		GetDstP()->y += (int)round(dy);
			
	}
	m_healthBarGreen->SetDstWH(getHealth(), 4);
}

void Enemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	//SDL_RenderDrawRectF(m_pRend, &m_rSearch);
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

void Enemy::RenderRadius(int rad, int x, int y)
{
	if (m_bSearch)
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 1);
	else
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 1);

	int old_x = sin(3.14 / 180 * 0) * rad;
	int old_y = cos(3.14 / 180 * 0) * rad;
	for (auto i = 0; i < 360; ++i)
	{
		int x1 = old_x;
		int y1 = old_y;
		int x2 = sin(3.14 / 180 * (i + 1)) * rad;
		int y2 = cos(3.14 / 180 * (i + 1)) * rad;
		
		SDL_RenderDrawLine(m_pRend, x1 + x, y1 + y, x2 + x, y2 + y);

		old_x = x2;
		old_y = y2;
	}

}


void Enemy::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = m_spriteMin = 0;
		m_spriteMax = 3;
	}
	else if (m_state == death)
	{
		m_sprite = m_spriteMin = 4;
		m_spriteMax = 6;
		m_frameMax = 6;
	}
	else // Only other is running for now...
	{
		m_sprite = m_spriteMin = 0;
		m_spriteMax = 3;
	}
}
