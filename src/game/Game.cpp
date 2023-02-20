#include "Game.h"
#include <fstream>
#include <iostream>

#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"
#include "../systems/AnimationSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"

Game::Game() : m_window{}, m_renderer{}, m_isRunning(false), m_isDebug(false), m_windowWidth(0), m_windowHeight(0), m_previousFrameTime(0)
{
	m_registry = std::make_unique<Registry>();
	m_assetStore = std::make_unique<AssetStore>(m_renderer);
}

void Game::LoadLevel(int level)
{
	m_registry->AddSystem<MovementSystem>();
	m_registry->AddSystem<RenderSystem>();
	m_registry->AddSystem<AnimationSystem>();
	m_registry->AddSystem<CollisionSystem>();

	m_assetStore->AddTexture("tank_image_right", "./assets/images/tank-panther-right.png");
	m_assetStore->AddTexture("truck_image_right", "./assets/images/truck-ford-right.png");
	m_assetStore->AddTexture("tilemap-image", "./assets/tilemaps/jungle.png");
	m_assetStore->AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png");
	m_assetStore->AddTexture("radar-image", "./assets/images/radar.png");

	// Load the tilemap
	int tileSize = 32;
	constexpr double tileScale = 3;
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

	Entity chopper = m_registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 24, true);

	Entity radar = m_registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2( static_cast<float>(m_windowWidth) - 74.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.AddComponent<AnimationComponent>(8, 6, true);

	Entity tank = m_registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(500.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0f, 0.0f));
	tank.AddComponent<SpriteComponent>("tank_image_right", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(glm::vec2(32.0f, 32.0f));

	Entity truck = m_registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0f, 0.0f));
	truck.AddComponent<SpriteComponent>("truck_image_right", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(glm::vec2(32.0f, 32.0f));
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
			if(sdlEvent.key.keysym.sym == SDLK_F1)
				m_isDebug = !m_isDebug;
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
	m_registry->GetSystem<AnimationSystem>().Update(deltaTime);
	m_registry->GetSystem<CollisionSystem>().Update();

	m_registry->Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
	SDL_RenderClear(m_renderer);

	m_registry->GetSystem<RenderSystem>().Draw(m_renderer, m_assetStore);
	m_registry->GetSystem<CollisionSystem>().Draw(m_renderer, m_isDebug);

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
