#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include <iostream>
#include <fstream>
#include "TextureManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "CollisionManager.h"
#include "Utility.h"

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

PlayState::PlayState()
{
}

void PlayState::Enter()
{
	std::ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			m_tiles.emplace(key, new Tile({ x * 16, y * 16, 16, 16 }, { 0,0,32,32 }, 
				Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				
				inFile >> key;
				Engine::Instance().GetLevel()[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				Engine::Instance().GetLevel()[row][col]->GetDstP()->x = (float)(32 * col);
				Engine::Instance().GetLevel()[row][col]->GetDstP()->y = (float)(32 * row);
				PathNode* temp = new PathNode(Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 16, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 16,
					Engine::Instance().GetLevel()[row][col]->GetDstP()->w, Engine::Instance().GetLevel()[row][col]->GetDstP()->h);
				m_pGrid.push_back(temp);
				if (Engine::Instance().GetLevel()[row][col]->IsObstacle())
					m_pObstacle.push_back(Engine::Instance().GetLevel()[row][col]);
				else if (Engine::Instance().GetLevel()[row][col]->IsHazard())
					m_pHazrad.push_back(Engine::Instance().GetLevel()[row][col]);
			}
		}
	}
	inFile.close();
	m_buildPatrolPath();
	m_displayPatrolPath();
	// Final engine initialization calls.
	m_pPlayer = new Player({ 0,47,15,20 }, { 60.0f,200.0f,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4);
	m_Enemy.push_back(new Enemy({ 0,88,14,21 }, { m_pPatrolPathOne[targetNode + 1]->GetPos().x, m_pPatrolPathOne[targetNode + 1]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4));
	m_Enemy.push_back(new Enemy({ 0,88,14,21 }, { m_pPatrolPathTwo[0]->GetPos().x, m_pPatrolPathTwo[0]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4));
	m_Enemy.push_back(new Enemy({ 0,88,14,21 }, { m_pPatrolPathThree[0]->GetPos().x, m_pPatrolPathThree[0]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4));
	
	
	m_Enemy[0]->SetDstXY(m_pPatrolPathOne[0]->GetPos().x - 15, m_pPatrolPathOne[0]->GetPos().y - 16);
	m_Enemy[1]->SetDstXY(m_pPatrolPathTwo[0]->GetPos().x - 15, m_pPatrolPathTwo[0]->GetPos().y - 16);
	m_Enemy[2]->SetDstXY(m_pPatrolPathThree[0]->GetPos().x - 15, m_pPatrolPathThree[0]->GetPos().y - 16);
	
	
	TempPForEnemyPath = &m_pPatrolPathOne;
	std::cout << "Number of Nodes: " << m_pGrid.size() << std::endl;
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[9][5]->GetDstP()->x , Engine::Instance().GetLevel()[9][5]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 8 + 7, Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 2 + 1});
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[6][21]->GetDstP()->x , Engine::Instance().GetLevel()[6][21]->GetDstP()->y ,
		Engine::Instance().GetLevel()[21][6]->GetDstP()->w * 8 + 7, Engine::Instance().GetLevel()[21][6]->GetDstP()->h * 2 + 1 });
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[14][18]->GetDstP()->x , Engine::Instance().GetLevel()[14][18]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 8 + 7, Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 2 + 1 });
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[18][27]->GetDstP()->x , Engine::Instance().GetLevel()[18][27]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 8 + 7, Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 2 + 1 });
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[20][9]->GetDstP()->x , Engine::Instance().GetLevel()[20][9]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 2 + 1, Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 8 + 7 });

	std::cout << Engine::Instance().GetLevel()[8][1]->GetDstP()->x << " , " << Engine::Instance().GetLevel()[8][1]->GetDstP()->y << std::endl;
}
void PlayState::RenderGrid()
{
	if (m_Debugmode)
	{
		for (int row = 0; row < ROWS; ++row)
		{
			for (int col = 0; col < COLS; ++col)
			{
				auto colour = (!m_pGrid[row * COLS + col]->getLOS()) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

				DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos() - glm::vec2(m_pGrid[row * COLS + col]->GetWidth() * 0.5f,
					m_pGrid[row * COLS + col]->GetHeight() * 0.5f),
					32, 32);
				DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos(),
					5, 5, colour);

			}
		}
		DEMA::DrawRect(glm::vec2(m_pPlayer->GetDstP()->x, m_pPlayer->GetDstP()->y),
			m_pPlayer->GetDstP()->w, m_pPlayer->GetDstP()->h);
	}
}
void PlayState::RenderLOS()
{
	for (auto node : m_pGrid)
	{
		if (!node->getLOS())
		{
			auto colour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

			DEMA::DrawLine({ int(node->GetPos().x), int(node->GetPos().y) }, { int(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w/2) ,int(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h / 2) },
				{Uint8(colour.r), Uint8(colour.g), Uint8(colour.b), Uint8(colour.a)});
		}

	}

}
void PlayState::SetLOS()
{
	for (auto node : m_pGrid)
	{
		node->setLOS(CollisionManager::LOSCheckN(node, m_pPlayer, &m_pLOSobs[0]));
	}
}
void PlayState::Update()
{
	if (EVMA::MousePressed(3) && m_bCanShoot)
	{
		m_bCanShoot = false;
		int x, y;
		SDL_GetMouseState(&x, &y);
		m_pMousePos.x = x;
		m_pMousePos.y = y;
		m_pPlayerBullet.push_back(new Bullet({ 0,126,14,6 }, { m_pPlayer->GetDstP()->x ,m_pPlayer->GetDstP()->y , 14, 6 }, 
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 10, m_pMousePos));
	}
	if (EVMA::MouseReleased(3))
	{
		m_bCanShoot = true;
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_H)) {
		std::cout << "Debug mode\n";
		m_Debugmode = !m_Debugmode;
	}
	if (m_Debugmode) {
		if (EVMA::KeyPressed(SDL_SCANCODE_K)) {
			std::cout << "Damage to Enemy\n";
			for (auto i = 0; i < m_Enemy.size(); ++i)
				m_Enemy[i]->setHealth(-4);
		}
		if (EVMA::KeyPressed(SDL_SCANCODE_P)) {
			std::cout << "Patrol mode\n";
			m_PatrolMode = !m_PatrolMode;
		}
	}
	//LOS = 0;
	SetLOS();
	PlayerHasLinofSight1 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
	PlayerHasLinofSight2 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
	PlayerHasLinofSight3 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
	PlayerHasLinofSight4 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
	PlayerHasLinofSight5 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
	

	if (PlayerHasLinofSight1 && PlayerHasLinofSight2 && PlayerHasLinofSight3 &&
		PlayerHasLinofSight4 && PlayerHasLinofSight5)
		PlayerHasLinofSight = true;
	else
		PlayerHasLinofSight = false;
	/*for (auto i = 0; i < m_pObstacle.size(); ++i)
	{
	
	
	/*if (LOS == 0)
		PlayerHasLinofSight = true;
	else
		PlayerHasLinofSight = false;*/
	for (auto i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		m_pPlayerBullet[i]->Update();
	}

	for (auto i = 0; i < m_Enemy.size(); ++i)
	{
		if (m_pPlayer->getAttack() == true && m_pPlayer->getOneAttack() == false) {
			AnimatedSprite* tempW = &m_pPlayer->getSword();
			SDL_FRect tempS = { tempW->GetDstP()->x, tempW->GetDstP()->y, tempW->GetDstP()->w, tempW->GetDstP()->h };
			SDL_FRect tempE = { m_Enemy[i]->GetDstP()->x, m_Enemy[i]->GetDstP()->y, m_Enemy[i]->GetDstP()->w, m_Enemy[i]->GetDstP()->h };
			if (COMA::AABBCheck(tempS, tempE)) {
				std::cout << "attack\n";
				m_pPlayer->setOneAttack(true);
				m_Enemy[i]->setHealth(-4);
			}
		}
	}
	
	if(m_Enemy[0]->getAlive() == true)
		m_Enemy[0]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathOne);
	if (m_Enemy[1]->getAlive() == true)
		m_Enemy[1]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathTwo);
	if (m_Enemy[2]->getAlive() == true)
		m_Enemy[2]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathThree);


	m_pPlayer->Update();
	CheckCollision();
	for (auto i = 0; i < m_Enemy.size(); ++i)
	{
		if (COMA::AABBCheck({ m_pPlayer->GetDstP()->x, m_pPlayer->GetDstP()->y, m_pPlayer->GetDstP()->w, m_pPlayer->GetDstP()->h },
			{ m_Enemy[i]->GetDstP()->x, m_Enemy[i]->GetDstP()->y, m_Enemy[i]->GetDstP()->w, m_Enemy[i]->GetDstP()->h }) && m_canHit == true) {
			m_pPlayer->setHealth(-4);
			m_canHit = false;
			std::cout << "hit\n";
		}
	}
	++m_hitCoolDown;
	if (m_hitCoolDown >= 60){
		m_hitCoolDown = 0;
		m_canHit = true;
	}
	for (auto i = 0; i < m_Enemy.size(); ++i) {
		if (m_Enemy[i]->getAlive() == false)
			delete m_Enemy[i];
	}
}

void PlayState::Render()
{
	
	SDL_RenderClear(Engine::Instance().GetRenderer()); // Clear the screen with the draw color.
	// Draw anew.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			Engine::Instance().GetLevel()[row][col]->Render();
		}
	}
	m_pPlayer->Render();
	for (auto i = 0; i < m_Enemy.size(); ++i){
		if (m_Enemy[i]->getAlive() == true)
			m_Enemy[i]->Render();
	}
	for (auto i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		m_pPlayerBullet[i]->Render();
	}
	if (m_Debugmode)
	{
		RenderGrid();
		auto LOSColour = (!PlayerHasLinofSight) ? glm::vec4(255.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 255.0f, 0.0f, 1.0f);
		for (auto i = 0; i < m_Enemy.size(); ++i)
		{
			DEMA::DrawLine({ int(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w / 2), int(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h / 2) },
				{ int(m_Enemy[i]->GetDstP()->x + m_Enemy[i]->GetDstP()->w / 2), int(m_Enemy[i]->GetDstP()->y + m_Enemy[i]->GetDstP()->h / 2) },
				{ Uint8(LOSColour.r), Uint8(LOSColour.g), Uint8(LOSColour.b), Uint8(LOSColour.a) });
			m_Enemy[i]->RenderRadius(100, m_Enemy[i]->GetDstP()->x + m_Enemy[i]->GetDstP()->w / 2, m_Enemy[i]->GetDstP()->y + m_Enemy[i]->GetDstP()->h / 2);
		}
	}
	//RenderLOS();

}

void PlayState::Exit()
{
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
}

void PlayState::Resume()
{
}

void PlayState::CheckCollision()
{
	for (int i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		SDL_Rect b = { m_pPlayerBullet[i]->GetDstP()->x, m_pPlayerBullet[i]->GetDstP()->y, 
			m_pPlayerBullet[i]->GetDstP()->w, m_pPlayerBullet[i]->GetDstP()->h };
		for (int j = 0; j < (int)m_Enemy.size(); j++)
		{
			if (m_Enemy[j] == nullptr) continue;
			SDL_Rect e = { m_Enemy[j]->GetDstP()->x, m_Enemy[j]->GetDstP()->y, 32, 32 };
			if (SDL_HasIntersection(&b, &e))
			{
				
				
				delete m_pPlayerBullet[i];
				m_pPlayerBullet[i] = nullptr;
				m_bPBNull = true;
				break;
			}
		}
	}
	if (m_bPBNull) CleanVector<Bullet*>(m_pPlayerBullet, m_bPBNull);
	for (int i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		SDL_Rect b = { m_pPlayerBullet[i]->GetDstP()->x, m_pPlayerBullet[i]->GetDstP()->y, 
			m_pPlayerBullet[i]->GetDstP()->w, m_pPlayerBullet[i]->GetDstP()->h };
		for (int j = 0; j < m_pObstacle.size(); j++)
		{
			if (m_pObstacle[j] == nullptr) continue;
			SDL_Rect e = { m_pObstacle[j]->GetDstP()->x, m_pObstacle[j]->GetDstP()->y, 32, 32 };
			if (SDL_HasIntersection(&b, &e))
			{
				delete m_pPlayerBullet[i];
				m_pPlayerBullet[i] = nullptr;
				m_bPBNull = true;
				break;
			}
		}
	}
	if (m_bPBNull) CleanVector<Bullet*>(m_pPlayerBullet, m_bPBNull);
}

void PlayState::m_buildPatrolPath()
{
	//PathOne
	//right
	for (auto i = 33; i < 63; i++)
	{
		m_pPatrolPathOne.push_back(m_pGrid[i]);
	}
	//down
	for (auto i = 3; i < 4; i++)
	{
		m_pPatrolPathOne.push_back(m_pGrid[(i * 32 - 2)]);
	}
	//left
	for (auto i = 126; i > 97; i--)
	{
		m_pPatrolPathOne.push_back(m_pGrid[i]);
	}
	//up
	for (auto i = 3 ; i > 0; i--)
	{
		m_pPatrolPathOne.push_back(m_pGrid[(32 * i +1)]);
	}
	std::cout << "Number of Nodes for path One: " << m_pPatrolPathOne.size() << std::endl;
	//Path two
	for (auto i = 23; i > 20; i--)
	{
		m_pPatrolPathTwo.push_back(m_pGrid[(32 * i - 2)]);
	}
	std::cout << "Number of Nodes for path two: " << m_pPatrolPathTwo.size() << std::endl;
	//Paht three
		m_pPatrolPathThree.push_back(m_pGrid[240]);
		m_pPatrolPathThree.push_back(m_pGrid[230]);
		m_pPatrolPathThree.push_back(m_pGrid[253]);
		m_pPatrolPathThree.push_back(m_pGrid[320]);
	std::cout << "Number of Nodes for path three: " << m_pPatrolPathThree.size() << std::endl;
}

void PlayState::m_displayPatrolPath()
{
	for (auto node : m_pPatrolPathOne)
	{
		std::cout << "(" << node->GetPos().x << ", " << node->GetPos().y << ")" << std::endl;
	}
}

StartState::StartState() {}

void StartState::Update()
{
	if (m_StartBtn->Update() == 1)
		return;
}

void StartState::Render()
{
	
	m_StartBtn->Render();
	State::Render();
}

void StartState::Enter()
{
	
	m_StartBtn = new PlayButton({ 0, 0, 67, 15 }, { 380.0f, 350.0f, 268, 60},
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
}

void StartState::Exit()
{
}

void StartState::Resume()
{
}
