#pragma once

#include <fstream>
#include <glm/vec2.hpp>

#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/HealthComponent.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include "../components/tags/CameraFollowTag.h"
#include "../components/tags/EnemyTag.h"
#include "../components/tags/ObstacleTag.h"
#include "../components/tags/PlayerTag.h"
#include "../core/Registry.h"
#include "../core/ResourceManager.h"
#include "../core/Types.h"

namespace re::utils
{
	struct Level
	{
		static int32 m_mapWidth;
		static int32 m_mapHeight;
	};

	namespace LevelLoader
	{
		inline void LoadMap(const std::unique_ptr<Registry>& registry)
		{
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

					const Entity tile = registry->CreateEntity();
					registry->AddComponent(tile, TransformComponent{ glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0 });
					registry->AddComponent(tile, SpriteComponent{ "tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY });
				}
			}

			mapFile.close();
			Level::m_mapWidth = static_cast<int32>(mapNumCols * tileSize * tileScale);
			Level::m_mapHeight = static_cast<int32>(mapNumRows * tileSize * tileScale);
		}

		inline void LoadResources(const std::unique_ptr<ResourceManager>& resourceManager, SDL_Renderer* renderer)
		{
			resourceManager->AddTexture("tank_image_right", "./assets/images/tank-panther-right.png", renderer);
			resourceManager->AddTexture("truck_image_right", "./assets/images/truck-ford-right.png", renderer);
			resourceManager->AddTexture("tilemap-image", "./assets/tilemaps/jungle.png", renderer);
			resourceManager->AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png", renderer);
			resourceManager->AddTexture("radar-image", "./assets/images/radar.png", renderer);
			resourceManager->AddTexture("bullet-image", "./assets/images/bullet.png", renderer);
			resourceManager->AddTexture("tree-image", "./assets/images/tree.png", renderer);

			resourceManager->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 20);
			resourceManager->AddFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
			resourceManager->AddFont("pico8-font-10", "./assets/fonts/pico8.ttf", 10);
		}

		inline void LoadEntities(const std::unique_ptr<Registry>& registry)
		{
			const Entity chopper = registry->CreateEntity();
			registry->AddComponent(chopper, CameraFollowTag{});
			registry->AddComponent(chopper, PlayerTag{});
			registry->AddComponent(chopper, TransformComponent{ glm::vec2(240, 110) });
			registry->AddComponent(chopper, RigidBodyComponent{ glm::vec2(0, 0) });
			registry->AddComponent(chopper, SpriteComponent{ "chopper-image", 32, 32, 2 });
			registry->AddComponent(chopper, AnimationComponent{ 2, 15, true });
			registry->AddComponent(chopper, BoxColliderComponent{ glm::vec2(32, 25), glm::vec2(0, 5) });
			registry->AddComponent(chopper, KeyboardControlComponent{ glm::vec2(0, -50), glm::vec2(50, 0), glm::vec2(0, 50), glm::vec2(-50, 0) });
			registry->AddComponent(chopper, ProjectileEmitterComponent{ glm::vec2(150), 0, 10, 20, true });
			registry->AddComponent(chopper, HealthComponent{ 100 });

			const Entity radar = registry->CreateEntity();
			registry->AddComponent(radar, TransformComponent{ glm::vec2(static_cast<float>(Window::m_width) - 74.0f, 10), glm::vec2(1, 1), 0 });
			registry->AddComponent(radar, RigidBodyComponent{ glm::vec2(0, 0) });
			registry->AddComponent(radar, SpriteComponent{ "radar-image", 64, 64, 10, true });
			registry->AddComponent(radar, AnimationComponent{ 8, 5, true });

			const Entity tank = registry->CreateEntity();
			registry->AddComponent(tank, EnemyTag{});
			registry->AddComponent(tank, TransformComponent{ glm::vec2(500) });
			registry->AddComponent(tank, RigidBodyComponent{ glm::vec2(20, 0) });
			registry->AddComponent(tank, SpriteComponent{ "tank_image_right", 32, 32, 1 });
			registry->AddComponent(tank, BoxColliderComponent{ glm::vec2(25, 18), glm::vec2(5, 7) });
			registry->AddComponent(tank, HealthComponent{ 50 });

			const Entity truck = registry->CreateEntity();
			registry->AddComponent(truck, EnemyTag{});
			registry->AddComponent(truck, TransformComponent{ glm::vec2(120, 500) });
			registry->AddComponent(truck, RigidBodyComponent{ glm::vec2(0) });
			registry->AddComponent(truck, SpriteComponent{ "truck_image_right", 32, 32, 1 });
			registry->AddComponent(truck, BoxColliderComponent{ glm::vec2(25, 20), glm::vec2(5, 7) });
			registry->AddComponent(truck, ProjectileEmitterComponent{ glm::vec2(100, 0), 2 });
			registry->AddComponent(truck, HealthComponent{ 50 });

			const Entity treeA = registry->CreateEntity();
			registry->AddComponent(treeA, ObstacleTag{});
			registry->AddComponent(treeA, TransformComponent{ glm::vec2(600, 495), glm::vec2(1, 1), 0 });
			registry->AddComponent(treeA, SpriteComponent{ "tree-image", 16, 32, 1 });
			registry->AddComponent(treeA, BoxColliderComponent{ glm::vec2(16, 32) });

			const Entity treeB = registry->CreateEntity();
			registry->AddComponent(treeB, ObstacleTag{});
			registry->AddComponent(treeB, TransformComponent{ glm::vec2(400, 495), glm::vec2(1, 1), 0 });
			registry->AddComponent(treeB, SpriteComponent{ "tree-image", 16, 32, 1 });
			registry->AddComponent(treeB, BoxColliderComponent{ glm::vec2(16, 32) });
		}
	}
}
