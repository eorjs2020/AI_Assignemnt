#include "Player.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "Engine.h"
#include "TextureManager.h"
#include "SoundManager.h"
#define SPEED 6

Player::Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0) {
	m_health = 40;
	m_healthBarGreen = new Sprite({ 0,25,100,9 }, { d.x,d.y - 16, 40.0, 4.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
	m_healthBarRed = new Sprite({ 0,16,100,9 }, { d.x,d.y - 16, 40.0, 4.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
	m_alivetimer = 0;
	m_alive = true;
	m_boxColl = false;
}

void Player::Update()
{
	
		
		
	switch (m_state)
	{
	case idle:
		if (m_health <= 0)
			SetState(dead);
		if (EVMA::KeyHeld(SDL_SCANCODE_W) || EVMA::KeyHeld(SDL_SCANCODE_S) ||
			EVMA::KeyHeld(SDL_SCANCODE_A) || EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			SetState(running);
		}
		break;
	case running:
		if (EVMA::KeyReleased(SDL_SCANCODE_W) || EVMA::KeyReleased(SDL_SCANCODE_S) ||
			EVMA::KeyReleased(SDL_SCANCODE_A) || EVMA::KeyReleased(SDL_SCANCODE_D))
		{
			//?????????
			SetState(idle);
			this->setSrcP(0, 47);
			break; // Skip movement parsing below.
		}
		else
		{
			this->setSrcP(0, 68);
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_W))
		{
			if (m_dst.y > 0 && !COMA::PlayerCollision({(int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
			{
				m_dst.y += -SPEED;
				m_pBulletDir = UP;
			}

		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_S))
		{
			if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({(int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
			{
				m_dst.y += SPEED;
				m_pBulletDir = DOWN;
			}

		}
		if (EVMA::KeyHeld(SDL_SCANCODE_A))
		{
			if (m_dst.x > 0 && !COMA::PlayerCollision({(int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
			{
				m_dst.x += -SPEED;
				m_dir = 1;
				m_pBulletDir = LEFT;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			if (m_dst.x < 1024 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
			{
				m_dst.x += SPEED;
				m_dir = 0;
				m_pBulletDir = RIGHT;

			}

		}
		
		break;
	case dead:

		this->setSrcP(0, 173);
		++m_alivetimer;
		if (m_alivetimer >= 17) {
			m_alive = false;
		}
		break;
	}
	Animate();
	if (EVMA::MousePressed(SDL_BUTTON_LEFT) && !m_attack)
	{
		m_attack = !m_attack;	
		m_Oneattack = false;
		m_sword = new AnimatedSprite({ 0, 109, 30, 12 }, { m_dst.x + 16, m_dst.y + 16, 30.0f, 12.0f },
				Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 0, 10);		
	}
	if (m_attack) {
		if (m_dir == 1) {
			m_sword->SetDstXY(this->GetDstP()->x - 16, this->GetDstP()->y+ 16);
			m_sword->SetFilp(SDL_FLIP_HORIZONTAL);
		}
		else{
			m_sword->SetDstXY(this->GetDstP()->x + 16, this->GetDstP()->y + 16);
		}
		++m_attackTimer;
		if (m_attackTimer >= 10) {
			m_attack = !m_attack;
			m_attackTimer = 0;
			m_Oneattack = true;
			m_sword = nullptr;
		}
	}

	if (EVMA::KeyPressed(SDL_SCANCODE_1))
	{
		setHealth(-4);
	}
	m_healthBarGreen->SetDstWH(getHealth(), 4);
	m_healthBarRed->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	m_healthBarGreen->SetDstXY(this->GetDstP()->x, this->GetDstP()->y);
	
}

void Player::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_healthBarRed->Render();
	m_healthBarGreen->Render();
	if (m_sword != nullptr)
	{
		m_sword->Render();
	}
}

int Player::GetDir()
{
	return m_pBulletDir;
}

void Player::SetState(int s)
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
