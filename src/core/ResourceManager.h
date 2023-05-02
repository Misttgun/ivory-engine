#pragma once

#include <string>
#include <unordered_map>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

namespace re
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager();
		ResourceManager(const ResourceManager& game) = delete;
		ResourceManager(ResourceManager&& game) = delete;
		ResourceManager& operator=(const ResourceManager& game) = delete;
		ResourceManager& operator=(ResourceManager&& game) = delete;

		void AddTexture(const std::string& assetId, const std::string& path, SDL_Renderer* renderer);
		[[nodiscard]] SDL_Texture* GetTexture(const std::string& assetId) const;

		void AddFont(const std::string& assetId, const std::string& path, int fontSize);
		[[nodiscard]] TTF_Font* GetFont(const std::string& assetId) const;

		void ClearAssets();

	private:
		std::unordered_map<std::string, SDL_Texture*> m_textures;
		std::unordered_map<std::string, TTF_Font*> m_fonts;
	};
}
