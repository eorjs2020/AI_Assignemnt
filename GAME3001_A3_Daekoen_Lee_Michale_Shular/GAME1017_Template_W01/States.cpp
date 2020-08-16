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
#include "SoundManager.h"


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
	m_Enemy.push_back(new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathOne[targetNode + 1]->GetPos().x, m_pPatrolPathOne[targetNode + 1]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"),m_pObstacle ,0, 0, 3, 4));
	m_Enemy.push_back(new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathTwo[0]->GetPos().x, m_pPatrolPathTwo[0]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), m_pObstacle, 0, 0, 3, 4));
	m_Enemy.push_back(new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathThree[0]->GetPos().x, m_pPatrolPathThree[0]->GetPos().y ,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), m_pObstacle, 0, 0, 3, 4));
	
	
	m_Enemy[0]->SetDstXY(m_pPatrolPathOne[0]->GetPos().x - 15, m_pPatrolPathOne[0]->GetPos().y - 16);
	m_Enemy[1]->SetDstXY(m_pPatrolPathTwo[0]->GetPos().x - 15, m_pPatrolPathTwo[0]->GetPos().y - 16);
	m_Enemy[2]->SetDstXY(m_pPatrolPathThree[0]->GetPos().x - 15, m_pPatrolPathThree[0]->GetPos().y - 16);
	
	m_box[0] = new Sprite({ 0,133,14,20 }, { Engine::Instance().GetLevel()[4][5]->GetDstP()->x ,Engine::Instance().GetLevel()[4][5]->GetDstP()->y,32.0f,32.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"));
	for (int i = 0; i < 2; i++) {
		m_boxHP[i] = 5;
	}
	TempPForEnemyPath = &m_pPatrolPathOne;
	std::cout << "Number of Nodes: " << m_pGrid.size() << std::endl;
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[8][1]->GetDstP()->x , Engine::Instance().GetLevel()[8][1]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][1]->GetDstP()->w * 8 + 16, Engine::Instance().GetLevel()[10][6]->GetDstP()->h * 2});
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[12][13]->GetDstP()->x + 16, Engine::Instance().GetLevel()[12][13]->GetDstP()->y,
		Engine::Instance().GetLevel()[7][21]->GetDstP()->w * 9 , Engine::Instance().GetLevel()[7][21]->GetDstP()->h * 2});
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[5][16]->GetDstP()->x, Engine::Instance().GetLevel()[5][16]->GetDstP()->y,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 9 , Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 2});
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[16][22]->GetDstP()->x , Engine::Instance().GetLevel()[16][22]->GetDstP()->y ,
		Engine::Instance().GetLevel()[9][6]->GetDstP()->w * 9, Engine::Instance().GetLevel()[9][6]->GetDstP()->h * 2 });
	
	m_pLOSobs.push_back({ Engine::Instance().GetLevel()[17][7]->GetDstP()->x , Engine::Instance().GetLevel()[17][7]->GetDstP()->y,
		Engine::Instance().GetLevel()[9][5]->GetDstP()->w * 3 , Engine::Instance().GetLevel()[9][5]->GetDstP()->h * 8  });


	
	m_gamestatus = new Label("tile", 420, 700, m_enemiesKilled, { 255, 255, 255, 255 });
	m_restart = new RestartButton({ 0, 0, 200, 80 }, { 380.0f, 550, 268, 60 },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("restart"));
	m_newGame = false;
	m_loseGame = false;
	m_win = new Sprite({0,0, 450 , 300}, { 300, 200, 450, 300}, Engine::Instance().GetRenderer(), TEMA::GetTexture("win"));
	m_lose = new Sprite({ 0,0, 450 , 300 }, { 300, 200, 450, 300 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("lose"));
	SOMA::SetSoundVolume(10);
	SOMA::SetMusicVolume(7);
	SOMA::Load("Aud/Turtles.mp3", "BGM", SOUND_MUSIC);
	SOMA::PlayMusic("BGM");
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

				/*DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos() - glm::vec2(m_pGrid[row * COLS + col]->GetWidth() * 0.5f,
					m_pGrid[row * COLS + col]->GetHeight() * 0.5f),
					32, 32);*/
				DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos(),
					5, 5, colour);

			}
		}
		for (int i = 0; i < m_HidingNode.size(); ++i)
		{			
			auto colour = (m_HidingNode[i]->getLOS()) ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			DEMA::DrawRect(m_HidingNode[i]->GetPos() - glm::vec2(m_HidingNode[i]->GetWidth() * 0.5f,
				m_HidingNode[i]->GetHeight() * 0.5f),
					32, 32, colour);

			/*DEMA::DrawRect(m_HidingNode[i]->GetPos(),
				5, 5, {52,70, 235, 255});*/
		}
		DEMA::DrawRect(glm::vec2(m_pPlayer->GetDstP()->x, m_pPlayer->GetDstP()->y),
			m_pPlayer->GetDstP()->w, m_pPlayer->GetDstP()->h);
		for (auto i = 0; i < m_Enemy.size(); ++i)
		{
			if (m_Enemy[i] != nullptr)
			{
				DEMA::DrawRect(glm::vec2(m_Enemy[i]->GetDstP()->x, m_Enemy[i]->GetDstP()->y),
					m_Enemy[i]->GetDstP()->w, m_Enemy[i]->GetDstP()->h);
			}
		}
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

bool PlayState::LOSNode(int n)
{
	
	for (auto j = 0; j < m_pLOSobs.size(); ++j)
	{
		if (!CollisionManager::LOSCheckN(m_pGrid[n], m_pPlayer, &m_pLOSobs[j]))
			return false;
					
	}
	return true;
}

void PlayState::SetLOS()
{
	for (auto node = 0; node < m_pGrid.size(); node++)
	{
		
		m_pGrid[node]->setLOS(LOSNode(node));
		
	}
}
void PlayState::Update()
{
	if (!m_newGame && !m_loseGame)
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
			SOMA::PlaySound("laser");
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
				for (auto i = 0; i < m_Enemy.size(); ++i) {
					if (m_Enemy[i] != nullptr)
						m_Enemy[i]->setHealth(-4);
				}
			}
			if (EVMA::KeyPressed(SDL_SCANCODE_P)) {
				std::cout << "Patrol mode\n";
				m_PatrolMode = !m_PatrolMode;
			}
		}
		//LOS = 0;

		if (m_Enemy[0] != nullptr) {
			PlayerHasLinofSight1 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
			PlayerHasLinofSight2 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
			PlayerHasLinofSight3 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
			PlayerHasLinofSight4 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
			PlayerHasLinofSight5 = COMA::LOSCheck(m_pPlayer, m_Enemy[0], &m_pLOSobs[0]);
		}

		//LOS = 0;
		SetLOS();


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
			if (m_Enemy[i] != nullptr && m_pPlayer->getAttack() == true && m_pPlayer->getOneAttack() == false) {
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
	
		if (m_Enemy[0] != nullptr) {
			m_Enemy[0]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathOne);
			m_winCondition[0] = false;
		}
		//Enemy respawn reset timer and position 
		else {
			m_enemyRespawnTimer[0]++;
			m_winCondition[0] = true;
			if (m_enemyRespawnTimer[0] >= 600) {
				m_Enemy[0] = new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathOne[targetNode + 1]->GetPos().x, m_pPatrolPathOne[targetNode + 1]->GetPos().y ,32.0f,32.0f },
					Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), m_pObstacle, 0, 0, 3, 4);
				m_Enemy[0]->SetDstXY(m_pPatrolPathOne[0]->GetPos().x - 15, m_pPatrolPathOne[0]->GetPos().y - 16);
				m_enemyRespawnTimer[0] = 0;
			}
		}
		if (m_Enemy[1] != nullptr) {
			m_Enemy[1]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathTwo);
			m_winCondition[1] = false;
		}
		//Enemy respawn reset timer and position 
		else {
			m_enemyRespawnTimer[1]++;
			m_winCondition[1] = true;
			if (m_enemyRespawnTimer[1] >= 600) {
				m_Enemy[1] = new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathTwo[targetNode + 1]->GetPos().x, m_pPatrolPathTwo[targetNode + 1]->GetPos().y ,32.0f,32.0f },
					Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), m_pObstacle, 0, 0, 3, 4);
				m_Enemy[1]->SetDstXY(m_pPatrolPathTwo[0]->GetPos().x - 15, m_pPatrolPathTwo[0]->GetPos().y - 16);
				m_enemyRespawnTimer[1] = 0;
			}
		}
		if (m_Enemy[2] != nullptr) {
			m_Enemy[2]->Update(m_pPlayer, m_PatrolMode, m_pPatrolPathThree);
			m_winCondition[2] = false;
		}
		//Enemy respawn reset timer and position 
		else {
			m_enemyRespawnTimer[2]++;
			m_winCondition[2] = true;
			if (m_enemyRespawnTimer[2] >= 600) {
				m_Enemy[2] = new MeleeEnemy({ 0,88,14,21 }, { m_pPatrolPathThree[targetNode + 1]->GetPos().x, m_pPatrolPathThree[targetNode + 1]->GetPos().y ,32.0f,32.0f },
					Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), m_pObstacle, 0, 0, 3, 4);
				m_Enemy[2]->SetDstXY(m_pPatrolPathThree[0]->GetPos().x - 15, m_pPatrolPathThree[0]->GetPos().y - 16);
				m_enemyRespawnTimer[2] = 0;
			}
		}


		m_pPlayer->Update(m_box, m_boxHP);
		CheckCollision();
		for (auto i = 0; i < m_Enemy.size(); ++i) {
			//if (m_Enemy[i]->getAlive() == false)
			//	++m_score;
			if (m_Enemy[i] != nullptr && m_Enemy[i]->getAlive() == false)
			{
				++m_score;
				delete m_Enemy[i];
				m_Enemy[i] = nullptr;
			}
		}

		m_enemiesKilled = "Enemies Killed: " + std::to_string(m_score);
		m_gamestatus->SetText(m_enemiesKilled);
		if (!m_pPlayer->getAlive()) {
			m_loseGame = true;
		}
	}
	if (m_newGame || m_loseGame) {
		if (m_restart->Update() == 1)
			return;
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
	m_box[0]->Render();
	m_pPlayer->Render();
	for (auto i = 0; i < m_Enemy.size(); ++i){
		if (m_Enemy[i] != nullptr)
			m_Enemy[i]->Render();
	}
	for (auto i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		m_pPlayerBullet[i]->Render();
	}
	if (m_Debugmode)
	{
		RenderGrid();
		
		for (auto i = 0; i < m_Enemy.size(); ++i)
		{
			
			if (m_Enemy[i] != nullptr)
			{
				auto LOSColour = (EnemyHasLOS(i)) ? glm::vec4(255.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 255.0f, 0.0f, 1.0f);
				DEMA::DrawLine({ int(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w / 2), int(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h / 2) },
					{ int(m_Enemy[i]->GetDstP()->x + m_Enemy[i]->GetDstP()->w / 2), int(m_Enemy[i]->GetDstP()->y + m_Enemy[i]->GetDstP()->h / 2) },
					{ Uint8(LOSColour.r), Uint8(LOSColour.g), Uint8(LOSColour.b), Uint8(LOSColour.a) });
				m_Enemy[i]->RenderRadius(100, m_Enemy[i]->GetDstP()->x + m_Enemy[i]->GetDstP()->w / 2, m_Enemy[i]->GetDstP()->y + m_Enemy[i]->GetDstP()->h / 2);
			}
		}
	}
	//RenderLOS();
	if (m_winCondition[0] && m_winCondition[1] && m_winCondition[2]) {
		m_win->Render();
		m_newGame = true;
	}
	if (m_loseGame)
		m_lose->Render();
	if (m_newGame || m_loseGame) {
		m_restart->Render();
	}
	m_gamestatus->Render();
	
}

void PlayState::Exit()
{
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
	SOMA::PauseMusic;
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
				m_Enemy[j]->setHealth(-4);
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
	//collision check for boxs
	for (int i = 0; i < (int)m_pPlayerBullet.size(); i++)
	{
		SDL_Rect b = { m_pPlayerBullet[i]->GetDstP()->x, m_pPlayerBullet[i]->GetDstP()->y,
			m_pPlayerBullet[i]->GetDstP()->w, m_pPlayerBullet[i]->GetDstP()->h };
		for (int j = 0; j < 2; j++)
		{
			if (m_box[j] == nullptr || m_boxHP[j] <= 0 ) continue;
			SDL_Rect e = { m_box[j]->GetDstP()->x, m_box[j]->GetDstP()->y, 32, 32 };
			if (SDL_HasIntersection(&b, &e))
			{
				delete m_pPlayerBullet[i];
				--m_boxHP[j];
				if(m_boxHP[j] <= 0)
					m_box[0]->setSrcP(0, 153);
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
	for (auto i = 24; i > 20; i--)
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

	// creation of Hiding Nodes
	m_HidingNode.push_back(m_pGrid[150]);
	m_HidingNode.push_back(m_pGrid[217]);
	m_HidingNode.push_back(m_pGrid[229]);
	m_HidingNode.push_back(m_pGrid[366]);
	m_HidingNode.push_back(m_pGrid[501]);
	m_HidingNode.push_back(m_pGrid[582]);
	m_HidingNode.push_back(m_pGrid[633]);

}

void PlayState::m_displayPatrolPath()
{
	for (auto node : m_pPatrolPathOne)
	{
		std::cout << "(" << node->GetPos().x << ", " << node->GetPos().y << ")" << std::endl;
	}
}

bool PlayState::EnemyHasLOS(int n)
{
	
	for (auto i = 0; i < m_pLOSobs.size(); ++i)
	{
		
		if (!COMA::LOSCheck(m_pPlayer, m_Enemy[n], &m_pLOSobs[i]))
		{
			m_Enemy[n]->SetLineofSight(true);
			return true;
		}
		
	}
	m_Enemy[n]->SetLineofSight(false);
	return false;
}

StartState::StartState() {}

void StartState::Update()
{
	if (m_StartBtn->Update() == 1)
		return;
}

void StartState::Render()
{
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_StartBtn->Render();
	State::Render();
	m_nameOne->Render();
	m_nameTwo->Render();
}

void StartState::Enter()
{
	
	m_StartBtn = new PlayButton({ 0, 0, 67, 15 }, { 380.0f, 350.0f, 268, 60},
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
	std::string c = "Michael Shular 101273089", d = "Daekoen Lee 101076401";
	m_nameOne = new Label("tile", 420, 180, c, { 255, 255, 255, 255 });
	m_nameTwo = new Label("tile", 420, 140, d, { 255, 255, 255, 255 });
}

void StartState::Exit()
{
}

void StartState::Resume()
{
}
