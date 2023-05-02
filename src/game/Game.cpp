#include "Game.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer.h>
#include "../components/TextLabelComponent.h"
#include "../helpers/LevelLoader.h"
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

	int32 utils::Level::m_mapWidth = 0;
	int32 utils::Level::m_mapHeight = 0;

	Game::Game() : m_camera{}, m_isRunning(false), m_isDebug(false), m_previousFrameTime(0), m_accumulator(0)
	{
		m_eventBus = std::make_shared<EventBus>();
		m_registry = std::make_unique<Registry>();
		m_registry->Init(m_eventBus);
		m_resourceManager = std::make_unique<ResourceManager>();
		m_window = std::make_unique<Window>();
	}

	void Game::LoadLevel() const
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

		const auto renderer = m_window->Renderer();

		utils::LevelLoader::LoadResources(m_resourceManager, renderer);
		utils::LevelLoader::LoadMap(m_registry);
		utils::LevelLoader::LoadEntities(m_registry);

		m_fpsCounter = m_registry->CreateEntity();
		constexpr SDL_Color green = { 0, 255, 0, 255 };
		m_registry->AddComponent(m_fpsCounter, TextLabelComponent{ glm::vec2(Window::m_width / 2 - 40, 10), m_fps, "charriot-font", green, true });
	}

	void Game::Setup() const
	{
		LoadLevel();
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
				if (sdlEvent.key.keysym.sym == SDLK_F2)
					m_isDebug = !m_isDebug;
				if (sdlEvent.key.keysym.sym == SDLK_F1)
					m_window->ToggleFullscreen();
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

		const auto currentFrameTime = SDL_GetTicks64();
		const auto deltaMs = currentFrameTime - m_previousFrameTime;
		m_previousFrameTime = currentFrameTime;
		m_accumulator += static_cast<float>(deltaMs);

		if (m_accumulator <= FUZZY_TIME_PER_FRAME_MS)
			return;

		m_accumulator -= TIME_PER_FRAME_MS;
		if (m_accumulator < 0)
			m_accumulator = 0;

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

		// FPS Counter
		m_totalFrames++;

		if (m_updateTimer.ElapsedSeconds() > 1)
		{
			m_fps = "FPS: " + std::to_string(static_cast<double>(m_totalFrames) / m_fpsTimer.ElapsedSeconds());
			auto& comp = m_registry->GetComponent<TextLabelComponent>(m_fpsCounter);
			comp.m_text = m_fps;

			m_updateTimer.Start();
		}
	}

	void Game::Render() const
	{
		m_window->BeginDraw();

		const auto renderer = m_window->Renderer();

		m_registry->GetSystem<RenderSystem>().Draw(renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderTextSystem>().Draw(renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderHealthBarSystem>().Draw(renderer, m_resourceManager, m_camera);

		if (m_isDebug)
		{
			m_registry->GetSystem<RenderGUISystem>().Draw();
			m_registry->GetSystem<CollisionSystem>().Draw(renderer, m_camera);

			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

		m_window->EndDraw();
	}

	void Game::Init()
	{
		m_window->Init("Game", glm::ivec2(1280, 720));

		m_camera.x = 0;
		m_camera.y = 0;
		m_camera.w = Window::m_width;
		m_camera.h = Window::m_height;

		m_isRunning = true;
	}

	void Game::Run()
	{
		Setup();

		m_previousFrameTime = SDL_GetTicks64();

		m_fpsTimer.Start();
		m_updateTimer.Start();

		while (m_isRunning)
		{
			ProcessInputs();
			Update();
			Render();
		}
	}

	void Game::Destroy() const
	{
		m_window->Cleanup();
	}
}
