#include "Enemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "Engine.h"
#include "TextureManager.h"
#include "SoundManager.h"

MeleeEnemy::MeleeEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, std::vector<Tile*> obs,int sstart, int smin, int smax, int nf)
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
	alarm = 0;
	this->obs = obs;
	attacksate = a_chasing;
	m_angle = 0.0;
}

MeleeEnemy::~MeleeEnemy()
{
}

void MeleeEnemy::Update(Player* player, bool a, std::vector<PathNode*> b)
{
	dx = dy = 0.0f;
	m_rSearch.x = (m_dst.x + m_dst.w /2) - 100 ;
	m_rSearch.y = (m_dst.y + m_dst.h / 2) - 100;
	m_rSearch.w = 200;
	m_rSearch.h = 200;
	SDL_FRect tempP = { player->GetDstP()->x, player->GetDstP()->y, player->GetDstP()->w, player->GetDstP()->h };
	m_bSearch = COMA::CircleAABBCheck({ m_rSearch.x + m_rSearch.w / 2, m_rSearch.y + m_rSearch.h / 2 }, 300, tempP);

	switch (m_state)
	{
	case idle:
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
			if (this->m_dst.x == (b[m_targetnode]->GetPos().x - 16) && this->m_dst.y == (b[m_targetnode]->GetPos().y - 16)) {
				if (m_targetnode == b.size() - 1)
					m_targetnode = 0;
				++m_targetnode;
			}

		}
		if (m_health <= 0)
		{
			SetState(death);
		}
		if (!m_bLOS && m_bSearch)
		{
			//std::cout << "search" << std::endl;;
			//m_state = chasing;
		}
		break;
	
	case chasing:
		
		if (m_health < 10)
		{
			m_state = flee;
		}
		switch (attacksate)
		{
		case a_chasing:


			double destAngle = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
				(player->GetDstP()->x + player->GetDstP()->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
			/*	if(HasLineofSight())
					Move2Full(destAngle);
				else*/
			MAMA::SetDeltas(destAngle, dx, dy, 2.0, 2.0);
			
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, dx, 0))
				GetDstP()->x += dx;
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, 0, dy))
				GetDstP()->y += dy;

			break;

		}
	case flee:
		

		break;

	case death:
		++m_alivetimer;
		if (m_alivetimer >= 17) {
			m_alive = false;
		}
		break;
	}
	Animate();
	m_healthBarRed->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	m_healthBarGreen->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);

	

	//if (alarm == 100)
	//{
	//	SOMA::PlaySound("alarm");
	//	alarm = 0;
	//}

	m_healthBarGreen->SetDstWH(getHealth(), 4);
}

void MeleeEnemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	//SDL_RenderDrawRectF(m_pRend, &m_rSearch);
}

void MeleeEnemy::StopX() { m_velX = 0.0; }
void MeleeEnemy::StopY() { m_velY = 0.0; }
void MeleeEnemy::SetAccelX(double a) { m_accelX = a; }
void MeleeEnemy::SetAccelY(double a) { m_accelY = a; }
double MeleeEnemy::GetVelX() { return m_velX; }
double MeleeEnemy::GetVelY() { return m_velY; }

void MeleeEnemy::SetVs(const double angle)
{
	double destAngle = MAMA::Rad2Deg(angle) + 90;
	m_angle += std::min(std::max(MAMA::Angle180(destAngle - m_angle), -5.0), 5.0); // Only rotate slightly towards the destination angle.
	// Now use the new slight rotation to generate dx and dy as normal.

	
}

void MeleeEnemy::Move2Full(double& angle)
{
	double perc = 0.0;
	for (unsigned i = 0; i < obs.size(); i++)
	{
		SDL_FRect* temp = obs[i]->GetDstP();
		double dist = MAMA::Distance((GetDstP()->x + GetDstP()->w / 2.0f), (temp->x + temp->w / 2.0f),
			(GetDstP()->y + GetDstP()->h / 2.0f), (temp->y + temp->h / 2.0f));
		perc = 1.0 - std::max(std::min((dist - 50.0) / (125.0 - 50.0), 10.0), 0.0);
		double fleeAngle = MAMA::AngleBetweenPoints((GetDstP()->y + GetDstP()->h / 2.0f) - (temp->y + temp->h / 2.0f),
			(GetDstP()->x + GetDstP()->w / 2.0f) - (temp->x + temp->w / 2.0f));
		if (perc > 0.0)
			angle = MAMA::LerpRad(angle, fleeAngle, perc);
	}
	SetVs(angle);
	MAMA::SetDeltas(MAMA::Deg2Rad(m_angle), dx, dy); // Try this: m_vel*(1-perc) 
}

glm::vec2 MeleeEnemy::getPos()
{
	return m_ePos;
}
void MeleeEnemy::SetX(float y) { m_dst.x = y; }
void MeleeEnemy::SetY(float y) { m_dst.y = y; }

void MeleeEnemy::RenderRadius(int rad, int x, int y)
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

void MeleeEnemy::SetState(int s)
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

//RangeEnemy

RangeEnemy::RangeEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
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
	alarm = 0;
}

RangeEnemy::~RangeEnemy()
{
}



void RangeEnemy::Update(Player* player, bool a, std::vector<PathNode*> b)
{
	dx = dy = 0.0f;
	m_rSearch.x = (m_dst.x + m_dst.w / 2) - 100;
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
		if (this->m_dst.x == (b[m_targetnode]->GetPos().x - 16) && this->m_dst.y == (b[m_targetnode]->GetPos().y - 16)) {
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
		if (m_alivetimer >= 17) {
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

void RangeEnemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	//SDL_RenderDrawRectF(m_pRend, &m_rSearch);
}

void RangeEnemy::StopX() { m_velX = 0.0; }
void RangeEnemy::StopY() { m_velY = 0.0; }
void RangeEnemy::SetAccelX(double a) { m_accelX = a; }
void RangeEnemy::SetAccelY(double a) { m_accelY = a; }
double RangeEnemy::GetVelX() { return m_velX; }
double RangeEnemy::GetVelY() { return m_velY; }
glm::vec2 RangeEnemy::getPos()
{
	return m_ePos;
}
void RangeEnemy::SetX(float y) { m_dst.x = y; }
void RangeEnemy::SetY(float y) { m_dst.y = y; }

void RangeEnemy::RenderRadius(int rad, int x, int y)
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


void RangeEnemy::SetState(int s)
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
