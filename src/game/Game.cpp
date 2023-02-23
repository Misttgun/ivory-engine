#include "Game.h"
#include <fstream>
#include <iostream>
#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/CameraFollowComponent.h"
#include "../components/HealthComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"
#include "../systems/AnimationSystem.h"
#include "../systems/CameraMovementSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/DamageSystem.h"
#include "../systems/KeyboardControlSystem.h"
#include "../systems/MovementSystem.h"
#include "../systems/ProjectileEmitSystem.h"
#include "../systems/ProjectileLifecycleSystem.h"
#include "../systems/RenderSystem.h"

int Game::m_windowWidth = 0;
int Game::m_windowHeight = 0;
int Game::m_mapWidth = 0;
int Game::m_mapHeight = 0;

Game::Game() : m_window{}, m_renderer{}, m_camera{}, m_isRunning(false), m_isDebug(false), m_previousFrameTime(0)
{
	m_registry = std::make_unique<Registry>();
	m_assetStore = std::make_unique<AssetStore>(m_renderer);
	m_eventBus = std::make_unique<EventBus>();
}

void Game::LoadLevel(int level) const
{
	m_registry->AddSystem<MovementSystem>();
	m_registry->AddSystem<RenderSystem>();
	m_registry->AddSystem<AnimationSystem>();
	m_registry->AddSystem<CollisionSystem>();
	m_registry->AddSystem<DamageSystem>();
	m_registry->AddSystem<KeyboardControlSystem>();
	m_registry->AddSystem<CameraMovementSystem>();
	m_registry->AddSystem<ProjectileEmitSystem>();
	m_registry->AddSystem<ProjectileLifecycleSystem>();

	m_assetStore->AddTexture("tank_image_right", "./assets/images/tank-panther-right.png");
	m_assetStore->AddTexture("truck_image_right", "./assets/images/truck-ford-right.png");
	m_assetStore->AddTexture("tilemap-image", "./assets/tilemaps/jungle.png");
	m_assetStore->AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png");
	m_assetStore->AddTexture("radar-image", "./assets/images/radar.png");
	m_assetStore->AddTexture("bullet-image", "./assets/images/bullet.png");

	// Load the tilemap
	int tileSize = 32;
	constexpr double tileScale = 3;
	constexpr int mapNumCols = 25;
	constexpr int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for(int y = 0; y < mapNumRows; y++)
	{
		for(int x = 0; x < mapNumCols; x++)
		{
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = m_registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}

	mapFile.close();
	m_mapWidth = static_cast<int>(mapNumCols * tileSize * tileScale);
	m_mapHeight = static_cast<int>(mapNumRows * tileSize * tileScale);

	Entity chopper = m_registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 3);
	chopper.AddComponent<AnimationComponent>(2, 24, true);
	chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -80), glm::vec2(80, 0), glm::vec2(0, 80), glm::vec2(-80, 0));
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(200), 0);
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);

	Entity radar = m_registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(static_cast<float>(m_windowWidth) - 74.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.AddComponent<AnimationComponent>(8, 6, true);

	Entity tank = m_registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(500.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	tank.AddComponent<SpriteComponent>("tank_image_right", 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(glm::vec2(32.0f, 32.0f));
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(200.0f, 0), 1.5f);
	chopper.AddComponent<HealthComponent>(50);

	Entity truck = m_registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	truck.AddComponent<SpriteComponent>("truck_image_right", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(glm::vec2(32.0f, 32.0f));
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(200.0f, 0), 1.5f);
	chopper.AddComponent<HealthComponent>(50);
}

void Game::Setup() const
{
	LoadLevel(1);
}

void Game::ProcessInputs()
{
	SDL_Event sdlEvent{};
	while(SDL_PollEvent(&sdlEvent))
	{
		switch(sdlEvent.type)
		{
			case SDL_QUIT:
				m_isRunning = false;
				break;
			case SDL_KEYDOWN:
				if(sdlEvent.key.keysym.sym == SDLK_ESCAPE)
					m_isRunning = false;
				if(sdlEvent.key.keysym.sym == SDLK_F1)
					m_isDebug = !m_isDebug;
				m_eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
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

	if(timeToWait > 0 && timeToWait <= TIME_PER_FRAME)
		SDL_Delay(timeToWait);

	const float deltaTime = static_cast<float>(currentFrameTime - m_previousFrameTime) / 1000.0f;

	m_previousFrameTime = currentFrameTime;

	m_eventBus->Reset();

	m_registry->GetSystem<DamageSystem>().SubscribeToEvent(m_eventBus);
	m_registry->GetSystem<KeyboardControlSystem>().SubscribeToEvent(m_eventBus);
	m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvent(m_eventBus);

	m_registry->Update();

	m_registry->GetSystem<MovementSystem>().Update(deltaTime);
	m_registry->GetSystem<AnimationSystem>().Update(deltaTime);
	m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);
	m_registry->GetSystem<CameraMovementSystem>().Update(m_camera);
	m_registry->GetSystem<ProjectileEmitSystem>().Update(deltaTime, m_registry);
	m_registry->GetSystem<ProjectileLifecycleSystem>().Update(deltaTime);
}

void Game::Render() const
{
	SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
	SDL_RenderClear(m_renderer);

	m_registry->GetSystem<RenderSystem>().Draw(m_renderer, m_assetStore, m_camera);
	m_registry->GetSystem<CollisionSystem>().Draw(m_renderer, m_camera, m_isDebug);

	SDL_RenderPresent(m_renderer);
}

void Game::Init()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::LogError("Error initializing SDL.");
		return;
	}

	SDL_DisplayMode displayMode{};
	SDL_GetCurrentDisplayMode(0, &displayMode);
	m_windowWidth = displayMode.w;
	m_windowHeight = displayMode.h;

	m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_BORDERLESS);
	if(!m_window)
	{
		Logger::LogError("Error creating SDL window.");
		return;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	if(!m_renderer)
	{
		Logger::LogError("Error creating SDL renderer.");
		return;
	}

	SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

	m_camera.x = 0;
	m_camera.y = 0;
	m_camera.w = m_windowWidth;
	m_camera.h = m_windowHeight;

	m_isRunning = true;

	m_assetStore->SetRenderer(m_renderer);
}

void Game::Run()
{
	Setup();
	while(m_isRunning)
	{
		ProcessInputs();
		Update();
		Render();
	}
}

void Game::Destroy() const
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}
