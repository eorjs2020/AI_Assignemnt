#include "Enemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "Engine.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include <ctime>

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
	destAngle = 0;
	m_canHit = true;
	hitTimer = 0;
	srand(unsigned int(time));
	m_wait = false;
	m_waitingTimerCheck = m_waitingTimer = 0;
}

MeleeEnemy::~MeleeEnemy()
{
}

void MeleeEnemy::Update(Player* player, bool a, std::vector<PathNode*> b, std::vector<PathNode*> hiding)
{
	dx = dy = 0.0f;
	m_rSearch.x = (m_dst.x + m_dst.w /2) - 100 ;
	m_rSearch.y = (m_dst.y + m_dst.h / 2) - 100;
	m_rSearch.w = 200;
	m_rSearch.h = 200;
	SDL_FRect tempP = { player->GetDstP()->x, player->GetDstP()->y, player->GetDstP()->w, player->GetDstP()->h };
	m_bSearch = COMA::CircleAABBCheck({ m_rSearch.x + m_rSearch.w / 2, m_rSearch.y + m_rSearch.h / 2 }, 300, tempP);
	m_dirtionXCheck = m_dst.x;
	
	if (m_health <= 0)
	{
		SetState(death);
	}
	switch (m_state)
	{
	case idle:
		
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
		
		if (!m_bLOS && m_bSearch)
		{
			//std::cout << "search" << std::endl;;
			m_state = chasing;
		}
		break;
	
	case chasing:

		if (m_health < 10)
		{
			m_state = flee;
			attacksate = a_chasing;
		}

		switch (attacksate)
		{
		case a_chasing:
			if (COMA::AABBCheck(*player->GetDstP(), *this->GetDstP())){
				attacksate = melee_attack;
			}
			if (m_wait)
			{
				++m_waitingTimer;
				if(m_waitingTimer >= m_waitingTimerCheck){
					m_wait = false;
					m_waitingTimer = 0;
				}
				break;
			}
			destAngle = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
				(player->GetDstP()->x + player->GetDstP()->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
			/*	if(HasLineofSight())
					Move2Full(destAngle);
				else*/
			MAMA::SetDeltas(destAngle, dx, dy, 2.0, 2.0);
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, dx, 0))
				GetDstP()->x += dx;
			else
				GetDstP()->y += dx;
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, 0, dy))
				GetDstP()->y += dy;
			else
				GetDstP()->x += dy;
		
		
			break;
		case melee_attack:
			if (m_sword == nullptr) {
				m_sword = new AnimatedSprite({ 0, 109, 30, 12 }, { m_dst.x -16, m_dst.y -16 , 30.0f, 12.0f },
					Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 0, 10);
				m_sword->SetAngle(-45);
				if (m_dir == 1) {
					m_sword->SetDstXY(this->GetDstP()->x - 16, this->GetDstP()->y);
					m_sword->SetFilp(SDL_FLIP_HORIZONTAL);
				}
				else {
					m_sword->SetAngle(45);
					m_sword->SetDstXY(this->GetDstP()->x - 32, this->GetDstP()->y);
				}
			}
			else
			{
				if (hitTimer >= 30)
					m_sword->SetAngle(0);
				if (COMA::AABBCheck(*player->GetDstP(), *m_sword->GetDstP()) && m_canHit == true) {
					player->setHealth(-4);
					m_canHit = false;
					std::cout << "hit\n";
				}
				if (m_dir == 1 && hitTimer >= 10) {
					m_sword->SetDstXY(this->GetDstP()->x - 16, this->GetDstP()->y + 16);
					m_sword->SetFilp(SDL_FLIP_HORIZONTAL);
				}
				else {
					m_sword->SetDstXY(this->GetDstP()->x + 16, this->GetDstP()->y + 16);
				}
				++hitTimer;
				if (hitTimer >= 60)
				{
					hitTimer = 0;
					m_canHit = true;
					attacksate = a_chasing;
					m_sword = nullptr;
					m_waitingTimerCheck = rand() % 120;
					m_wait = true;
				}
				
			}
			break;
		}
	case flee:
		if (m_health < 10 && MAMA::Distance(m_dst.x, tempP.x, m_dst.y, tempP.y) < 150)
		{
			destAngle = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
				(player->GetDstP()->x + player->GetDstP()->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
			MAMA::SetDeltas(destAngle, dx, dy, -2.0, -2.0);

			/*	if(HasLineofSight())
					Move2Full(destAngle);
				else*/



		}
		else if (m_health < 10 && MAMA::Distance(m_dst.x, tempP.x, m_dst.y, tempP.y) > 150)
		{
			for (auto i = 0; i < hiding.size(); ++i)
			{
				if (!hiding[i]->getLOS())
				{
					std::cout << "aa" << std::endl;
					destAngle = MAMA::AngleBetweenPoints((hiding[i]->GetPos().y + hiding[i]->GetHeight() / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
						(hiding[i]->GetPos().x + hiding[i]->GetWidth() / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
					MAMA::SetDeltas(destAngle, dx, dy, 2.0, 2.0);
					break;
				}
			}

		}
		else
			break;
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, dx, 0))
			GetDstP()->x += dx;
		else
			GetDstP()->y += dx;
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, 0, dy))
			GetDstP()->y += dy;
		else
			GetDstP()->x += dy;
		break;

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
	//This is to change direction of enemies sprite should be last calcutlation 
	if (m_dst.x < m_dirtionXCheck){
		m_dir = 1;
	}
	if (m_dst.x > m_dirtionXCheck) {
		m_dir = 0;
	}
}

void MeleeEnemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	if(m_sword != nullptr)
		m_sword->Render();
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

RangeEnemy::RangeEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, std::vector<Bullet*>* EB, std::vector<Tile*> obs, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0), bVec(EB) {
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
	destAngle = 0;
	m_canHit = true;
	hitTimer = 0;
}

RangeEnemy::~RangeEnemy()
{
}

void RangeEnemy::Update(Player* player, bool a, std::vector<PathNode*> b, std::vector<PathNode*> hiding)
{
	dx = dy = 0.0f;
	m_rSearch.x = (m_dst.x + m_dst.w / 2) - 100;
	m_rSearch.y = (m_dst.y + m_dst.h / 2) - 100;
	m_rSearch.w = 200;
	m_rSearch.h = 200;
	SDL_FRect tempP = { player->GetDstP()->x, player->GetDstP()->y, player->GetDstP()->w, player->GetDstP()->h };
	m_bSearch = COMA::CircleAABBCheck({ m_rSearch.x + m_rSearch.w / 2, m_rSearch.y + m_rSearch.h / 2 }, 250, tempP);
	m_dirtionXCheck = m_dst.x;
	if (m_health <= 0)
	{
		SetState(death);
	}
	switch (m_state)
	{
	case idle:
		
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

	
		
		if (!m_bLOS && m_bSearch)
		{
			std::cout << "search" << std::endl;;
			m_state = chasing;
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
			if (COMA::CircleAABBCheck({ m_dst.x + m_dst.w / 2, m_dst.y + m_dst.h / 2 }, 250, tempP) && !HasLineofSight()) {
				attacksate = Ranged_attack;
			}
			destAngle = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
				(player->GetDstP()->x + player->GetDstP()->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);

			/*	if(HasLineofSight())
					Move2Full(destAngle);
				else*/
			MAMA::SetDeltas(destAngle, dx, dy, 2.0, 2.0);

			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, dx, 0))
				GetDstP()->x += dx;
			else
				GetDstP()->y += dx;
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, 0, dy))
				GetDstP()->y += dy;
			else
				GetDstP()->x += dy;
			break;
		case Ranged_attack:
			if (m_sword == nullptr) {
				m_sword = new AnimatedSprite({ 0, 109, 30, 12 }, { m_dst.x - 16, m_dst.y - 16 , 30.0f, 12.0f },
					Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 0, 10);
				m_sword->SetAngle(-45);
				if (m_dir == 1) {
					std::cout << "HitR" << std::endl;
					m_sword->SetDstXY(this->GetDstP()->x - 16, this->GetDstP()->y);
					m_sword->SetFilp(SDL_FLIP_HORIZONTAL);
					bVec->push_back(new Bullet({ 0,126,14,6 }, { GetDstP()->x + GetDstP()->w / 2 ,GetDstP()->y + GetDstP()->h / 2, 14, 6 },
						Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 10, glm::vec2(player->GetDstP()->x + player->GetDstP()->w / 2
							, player->GetDstP()->y + player->GetDstP()->h / 2)));
				}
				else {
					m_sword->SetAngle(45);
					m_sword->SetDstXY(this->GetDstP()->x - 32, this->GetDstP()->y);
					bVec->push_back(new Bullet({ 0,126,14,6 }, { GetDstP()->x + GetDstP()->w / 2 ,GetDstP()->y + GetDstP()->h / 2, 14, 6 },
						Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 10, glm::vec2(player->GetDstP()->x + player->GetDstP()->w / 2
							, player->GetDstP()->y + player->GetDstP()->h / 2)));
					std::cout << "HitR" << std::endl;
				}
			}
			else
			{
				if (hitTimer >= 30)
					m_sword->SetAngle(0);
				if (COMA::AABBCheck(*player->GetDstP(), *m_sword->GetDstP()) && m_canHit == true) {
					player->setHealth(-4);
					m_canHit = false;
					std::cout << "hit\n";
				}
				if (m_dir == 1 && hitTimer >= 10) {
					m_sword->SetDstXY(this->GetDstP()->x - 16, this->GetDstP()->y + 16);
					m_sword->SetFilp(SDL_FLIP_HORIZONTAL);
				}
				else {
					m_sword->SetDstXY(this->GetDstP()->x + 16, this->GetDstP()->y + 16);
				}
				++hitTimer;
				if (hitTimer >= 60)
				{
					hitTimer = 0;
					m_canHit = true;
					attacksate = a_chasing;
					m_sword = nullptr;
				}

			}
			break;
		}
	case flee:
		if (m_health < 10 && MAMA::Distance(m_dst.x, tempP.x, m_dst.y, tempP.y) < 150)
		{
			destAngle = MAMA::AngleBetweenPoints((player->GetDstP()->y + player->GetDstP()->h / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
				(player->GetDstP()->x + player->GetDstP()->w / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
			MAMA::SetDeltas(destAngle, dx, dy, -2.0, -2.0);

			/*	if(HasLineofSight())
					Move2Full(destAngle);
				else*/

		

		}
		else if (m_health < 10 && MAMA::Distance(m_dst.x, tempP.x, m_dst.y, tempP.y) > 150)
		{
			for (auto i = 0; i < hiding.size(); ++i)
			{
				if (!hiding[i]->getLOS())
				{
					std::cout << "aa" << std::endl;
 					destAngle = MAMA::AngleBetweenPoints((hiding[i]->GetPos().y + hiding[i]->GetHeight() / 2.0f) - (GetDstP()->y + GetDstP()->h / 2.0f) + y,
						(hiding[i]->GetPos().x + hiding[i]->GetWidth() / 2.0f) - (GetDstP()->x + GetDstP()->w / 2.0f) + x);
					MAMA::SetDeltas(destAngle, dx, dy, 2.0, 2.0);
					break;
				}
			}
		
		}
		else
			break;
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, dx, 0))
			GetDstP()->x += dx;
		else
			GetDstP()->y += dx;
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)m_dst.w, (int)m_dst.h }, 0, dy))
			GetDstP()->y += dy;
		else
			GetDstP()->x += dy;
		break;

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
	//This is to change direction of enemies sprite should be last calcutlation 
	if (m_dst.x < m_dirtionXCheck) {
		m_dir = 1;
	}
	if (m_dst.x > m_dirtionXCheck) {
		m_dir = 0;
	}
}

void RangeEnemy::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	if (m_sword != nullptr)
		m_sword->Render();
	//SDL_RenderDrawRectF(m_pRend, &m_rSearch);
}

void RangeEnemy::StopX() { m_velX = 0.0; }
void RangeEnemy::StopY() { m_velY = 0.0; }
void RangeEnemy::SetAccelX(double a) { m_accelX = a; }
void RangeEnemy::SetAccelY(double a) { m_accelY = a; }
double RangeEnemy::GetVelX() { return m_velX; }
double RangeEnemy::GetVelY() { return m_velY; }

void RangeEnemy::SetVs(const double angle)
{
	double destAngle = MAMA::Rad2Deg(angle) + 90;
	m_angle += std::min(std::max(MAMA::Angle180(destAngle - m_angle), -5.0), 5.0); // Only rotate slightly towards the destination angle.
	// Now use the new slight rotation to generate dx and dy as normal.


}

void RangeEnemy::Move2Full(double& angle)
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