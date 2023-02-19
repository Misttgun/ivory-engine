#include <iostream>
#include <SDL2/SDL_image.h>

#include "Game.h"

#include <fstream>

#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"

Game::Game() : m_window{}, m_renderer{}, m_isRunning(false), m_windowWidth(0), m_windowHeight(0), m_previousFrameTime(0)
{
	m_registry = std::make_unique<Registry>();
	m_assetStore = std::make_unique<AssetStore>(m_renderer);
}

void Game::LoadLevel(int level)
{
	m_registry->AddSystem<MovementSystem>();
	m_registry->AddSystem<RenderSystem>();

	m_assetStore->AddTexture("tank_image_right", "./assets/images/tank-panther-right.png");
	m_assetStore->AddTexture("truck_image_right", "./assets/images/truck-ford-right.png");
	m_assetStore->AddTexture("tilemap-image", "./assets/tilemaps/jungle.png");

	// Load the tilemap
	int tileSize = 32;
	constexpr double tileScale = 2.0;
	constexpr int mapNumCols = 25;
	constexpr int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) 
	{
		for (int x = 0; x < mapNumCols; x++) 
		{
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = m_registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}

	mapFile.close();

	Entity tank = m_registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(30.0f, 0.0f));
	tank.AddComponent<SpriteComponent>("tank_image_right", 32, 32, 2);

	Entity truck = m_registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0f, 0.0f));
	truck.AddComponent<SpriteComponent>("truck_image_right", 32, 32, 1);
}

void Game::Setup()
{
	LoadLevel(1);
}

void Game::ProcessInputs()
{
	SDL_Event sdlEvent{};
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			m_isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				m_isRunning = false;
			break;
		default:
			break;
		}
	}
}

void Game::Update()
{
	const int currentFrameTime = static_cast<int>(SDL_GetTicks());
	const int timeToWait = TIME_PER_FRAME - (currentFrameTime - m_previousFrameTime);

	if (timeToWait > 0 && timeToWait <= TIME_PER_FRAME)
		SDL_Delay(timeToWait);

	const float deltaTime = static_cast<float>(currentFrameTime - m_previousFrameTime) / 1000.0f;

	m_previousFrameTime = currentFrameTime;

	m_registry->GetSystem<MovementSystem>().Update(deltaTime);

	m_registry->Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
	SDL_RenderClear(m_renderer);

	m_registry->GetSystem<RenderSystem>().Draw(m_renderer, m_assetStore);

	SDL_RenderPresent(m_renderer);
}

void Game::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::LogError("Error initializing SDL.");
		return;
	}

	SDL_DisplayMode displayMode{};
	SDL_GetCurrentDisplayMode(0, &displayMode);
	m_windowWidth = displayMode.w;
	m_windowHeight = displayMode.h;

	m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_BORDERLESS);
	if (!m_window)
	{
		Logger::LogError("Error creating SDL window.");
		return;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	if (!m_renderer)
	{
		Logger::LogError("Error creating SDL renderer.");
		return;
	}

	SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

	m_isRunning = true;

	m_assetStore->SetRenderer(m_renderer);
}

void Game::Run()
{
	Setup();
	while (m_isRunning)
	{
		ProcessInputs();
		Update();
		Render();
	}
}

void Game::Destroy()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}
