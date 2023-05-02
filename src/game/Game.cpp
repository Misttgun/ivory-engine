#include "Game.h"
#include <fstream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <iostream>
#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/HealthComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../components/tags/CameraFollowTag.h"
#include "../components/tags/EnemyTag.h"
#include "../components/tags/ObstacleTag.h"
#include "../components/tags/PlayerTag.h"
#include "../helpers/Logger.h"
#include "../systems/AnimationSystem.h"
#include "../systems/CameraMovementSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/DamageSystem.h"
#include "../systems/KeyboardControlSystem.h"
#include "../systems/MovementSystem.h"
#include "../systems/ProjectileEmitSystem.h"
#include "../systems/ProjectileLifecycleSystem.h"
#include "../systems/RenderGUISystem.h"
#include "../systems/RenderHealthBarSystem.h"
#include "../systems/RenderSystem.h"
#include "../systems/RenderTextSystem.h"

namespace re
{
	int32 Game::m_windowWidth = 0;
	int32 Game::m_windowHeight = 0;
	int32 Game::m_mapWidth = 0;
	int32 Game::m_mapHeight = 0;

	Game::Game() : m_window{}, m_renderer{}, m_camera{}, m_isRunning(false), m_isDebug(false), m_previousFrameTime(0), m_accumulator(0)
	{
		m_eventBus = std::make_shared<EventBus>();
		m_registry = std::make_unique<Registry>();
		m_registry->Init(m_eventBus);
		m_resourceManager = std::make_unique<ResourceManager>();
	}

	void Game::LoadLevel(const int32 level) const
	{
		m_registry->RegisterSystem<MovementSystem>();
		m_registry->RegisterSystem<RenderSystem>();
		m_registry->RegisterSystem<AnimationSystem>();
		m_registry->RegisterSystem<CollisionSystem>();
		m_registry->RegisterSystem<DamageSystem>();
		m_registry->RegisterSystem<KeyboardControlSystem>();
		m_registry->RegisterSystem<CameraMovementSystem>();
		m_registry->RegisterSystem<ProjectileEmitSystem>();
		m_registry->RegisterSystem<ProjectileLifecycleSystem>();
		m_registry->RegisterSystem<RenderTextSystem>();
		m_registry->RegisterSystem<RenderHealthBarSystem>();
		m_registry->RegisterSystem<RenderGUISystem>();

		m_resourceManager->AddTexture("tank_image_right", "./assets/images/tank-panther-right.png", m_renderer);
		m_resourceManager->AddTexture("truck_image_right", "./assets/images/truck-ford-right.png", m_renderer);
		m_resourceManager->AddTexture("tilemap-image", "./assets/tilemaps/jungle.png", m_renderer);
		m_resourceManager->AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png", m_renderer);
		m_resourceManager->AddTexture("radar-image", "./assets/images/radar.png", m_renderer);
		m_resourceManager->AddTexture("bullet-image", "./assets/images/bullet.png", m_renderer);
		m_resourceManager->AddTexture("tree-image", "./assets/images/tree.png", m_renderer);

		m_resourceManager->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 20);
		m_resourceManager->AddFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
		m_resourceManager->AddFont("pico8-font-10", "./assets/fonts/pico8.ttf", 10);

		// Load the tilemap
		constexpr int32 tileSize = 32;
		constexpr double tileScale = 2;
		constexpr int32 mapNumCols = 25;
		constexpr int32 mapNumRows = 20;

		std::fstream mapFile;
		mapFile.open("./assets/tilemaps/jungle.map");

		for (int32 y = 0; y < mapNumRows; y++)
		{
			for (int32 x = 0; x < mapNumCols; x++)
			{
				char ch;
				mapFile.get(ch);
				const int32 srcRectY = std::atoi(&ch) * tileSize;
				mapFile.get(ch);
				const int32 srcRectX = std::atoi(&ch) * tileSize;
				mapFile.ignore();

				const Entity tile = m_registry->CreateEntity();
				m_registry->AddComponent(tile, TransformComponent{glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0});
				m_registry->AddComponent(tile, SpriteComponent{"tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY});
			}
		}

		mapFile.close();
		m_mapWidth = static_cast<int32>(mapNumCols * tileSize * tileScale);
		m_mapHeight = static_cast<int32>(mapNumRows * tileSize * tileScale);

		const Entity chopper = m_registry->CreateEntity();
		m_registry->AddComponent(chopper, CameraFollowTag{});
		m_registry->AddComponent(chopper, PlayerTag{});
		m_registry->AddComponent(chopper, TransformComponent{glm::vec2(240, 110)});
		m_registry->AddComponent(chopper, RigidBodyComponent{glm::vec2(0, 0)});
		m_registry->AddComponent(chopper, SpriteComponent{"chopper-image", 32, 32, 2});
		m_registry->AddComponent(chopper, AnimationComponent{2, 15, true});
		m_registry->AddComponent(chopper, BoxColliderComponent{glm::vec2(32, 25), glm::vec2(0, 5)});
		m_registry->AddComponent(chopper, KeyboardControlComponent{glm::vec2(0, -50), glm::vec2(50, 0), glm::vec2(0, 50), glm::vec2(-50, 0)});
		m_registry->AddComponent(chopper, ProjectileEmitterComponent{glm::vec2(150), 0, 10, 20, true});
		m_registry->AddComponent(chopper, HealthComponent{100});

		const Entity radar = m_registry->CreateEntity();
		m_registry->AddComponent(radar, TransformComponent{glm::vec2(static_cast<float>(m_windowWidth) - 74.0f, 10), glm::vec2(1, 1), 0});
		m_registry->AddComponent(radar, RigidBodyComponent{glm::vec2(0, 0)});
		m_registry->AddComponent(radar, SpriteComponent{"radar-image", 64, 64, 10, true});
		m_registry->AddComponent(radar, AnimationComponent{8, 5, true});

		const Entity tank = m_registry->CreateEntity();
		m_registry->AddComponent(tank, EnemyTag{});
		m_registry->AddComponent(tank, TransformComponent{glm::vec2(500)});
		m_registry->AddComponent(tank, RigidBodyComponent{glm::vec2(20, 0)});
		m_registry->AddComponent(tank, SpriteComponent{"tank_image_right", 32, 32, 1});
		m_registry->AddComponent(tank, BoxColliderComponent{glm::vec2(25, 18), glm::vec2(5, 7)});
		m_registry->AddComponent(tank, HealthComponent{50});

		const Entity truck = m_registry->CreateEntity();
		m_registry->AddComponent(truck, EnemyTag{});
		m_registry->AddComponent(truck, TransformComponent{glm::vec2(120, 500)});
		m_registry->AddComponent(truck, RigidBodyComponent{glm::vec2(0)});
		m_registry->AddComponent(truck, SpriteComponent{"truck_image_right", 32, 32, 1});
		m_registry->AddComponent(truck, BoxColliderComponent{glm::vec2(25, 20), glm::vec2(5, 7)});
		m_registry->AddComponent(truck, ProjectileEmitterComponent{glm::vec2(100, 0), 2});
		m_registry->AddComponent(truck, HealthComponent{50});

		const Entity treeA = m_registry->CreateEntity();
		m_registry->AddComponent(treeA, ObstacleTag{});
		m_registry->AddComponent(treeA, TransformComponent{glm::vec2(600, 495), glm::vec2(1, 1), 0});
		m_registry->AddComponent(treeA, SpriteComponent{"tree-image", 16, 32, 1});
		m_registry->AddComponent(treeA, BoxColliderComponent{glm::vec2(16, 32)});

		const Entity treeB = m_registry->CreateEntity();
		m_registry->AddComponent(treeB, ObstacleTag{});
		m_registry->AddComponent(treeB, TransformComponent{glm::vec2(400, 495), glm::vec2(1, 1), 0});
		m_registry->AddComponent(treeB, SpriteComponent{"tree-image", 16, 32, 1});
		m_registry->AddComponent(treeB, BoxColliderComponent{glm::vec2(16, 32)});

		const Entity label = m_registry->CreateEntity();
		constexpr SDL_Color green = {0, 255, 0, 255};
		m_registry->AddComponent(label, TextLabelComponent{glm::vec2(m_windowWidth / 2 - 40, 10), "CHOPPER 1.0", "charriot-font", green, true});
	}

	void Game::Setup() const
	{
		LoadLevel(1);
	}

	void Game::ProcessInputs()
	{
		SDL_Event sdlEvent{};
		while (SDL_PollEvent(&sdlEvent))
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
			ImGuiIO& io = ImGui::GetIO();
			int32 mouseX, mouseY;
			const auto buttons = SDL_GetMouseState(&mouseX, &mouseY);
			io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
			io.MouseDown[0] = buttons && SDL_BUTTON(SDL_BUTTON_LEFT);
			io.MouseDown[1] = buttons && SDL_BUTTON(SDL_BUTTON_RIGHT);

			switch (sdlEvent.type)
			{
			case SDL_QUIT:
				m_isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
					m_isRunning = false;
				if (sdlEvent.key.keysym.sym == SDLK_F1)
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
		constexpr float deltaTime = DELTA_TIME_S;

		const auto currentFrameTime = SDL_GetTicks();
		const auto deltaMs = currentFrameTime - m_previousFrameTime;
		m_previousFrameTime = currentFrameTime;
		m_accumulator += static_cast<float>(deltaMs);

		if (m_accumulator <= FUZZY_TIME_PER_FRAME_MS)
			SDL_Delay(static_cast<uint32>(m_accumulator));

		m_accumulator -= TIME_PER_FRAME_MS;
		if (m_accumulator < 0)
			m_accumulator = 0;

		/*const int currentFrameTime = static_cast<int>(SDL_GetTicks());
		const int timeToWait = TIME_PER_FRAME - (currentFrameTime - m_previousFrameTime);

		if (timeToWait > 0 && timeToWait <= TIME_PER_FRAME)
			SDL_Delay(timeToWait);

		m_previousFrameTime = currentFrameTime;*/

		m_eventBus->Reset();

		m_registry->GetSystem<DamageSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<KeyboardControlSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<MovementSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<CollisionSystem>().SubscribeToEvent(m_eventBus);

		m_registry->Update();

		m_registry->GetSystem<MovementSystem>().Update(deltaTime);
		m_registry->GetSystem<AnimationSystem>().Update(deltaTime);
		m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);
		m_registry->GetSystem<CameraMovementSystem>().Update(m_camera);
		m_registry->GetSystem<ProjectileEmitSystem>().Update(deltaTime);
		m_registry->GetSystem<ProjectileLifecycleSystem>().Update(deltaTime);
	}

	void Game::Render() const
	{
		SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
		SDL_RenderClear(m_renderer);

		m_registry->GetSystem<RenderSystem>().Draw(m_renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderTextSystem>().Draw(m_renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderHealthBarSystem>().Draw(m_renderer, m_resourceManager, m_camera);

		if (m_isDebug)
		{
			m_registry->GetSystem<RenderGUISystem>().Draw();
			m_registry->GetSystem<CollisionSystem>().Draw(m_renderer, m_camera);

			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

		SDL_RenderPresent(m_renderer);
	}

	void Game::Init()
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			Logger::LogError("Error initializing SDL.");
			return;
		}

		if (TTF_Init() != 0)
		{
			Logger::LogError("Error initializing SDL TTF.");
			return;
		}

		m_windowWidth = 1280;
		m_windowHeight = 720;

		m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_BORDERLESS);
		if (!m_window)
		{
			Logger::LogError("Error creating SDL window.");
			return;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);
		if (!m_renderer)
		{
			Logger::LogError("Error creating SDL renderer.");
			return;
		}

		// Initialize IMGUI context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
		ImGui_ImplSDLRenderer_Init(m_renderer);

		m_camera.x = 0;
		m_camera.y = 0;
		m_camera.w = m_windowWidth;
		m_camera.h = m_windowHeight;

		m_isRunning = true;
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

	void Game::Destroy() const
	{
		// Cleanup
		ImGui_ImplSDLRenderer_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}
}
