#include "ResourceManager.h"
#include <SDL2/SDL_image.h>
#include "../helpers/Logger.h"

namespace re
{
	ResourceManager::~ResourceManager()
	{
		ClearAssets();
	}

	void ResourceManager::AddTexture(const std::string& assetId, const std::string& path, SDL_Renderer* renderer)
	{
		SDL_Surface* surface = IMG_Load(path.c_str());
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		m_textures.emplace(assetId, texture);

		utils::Logger::Log("New texture added to the Asset Store with id= " + assetId);
	}

	SDL_Texture* ResourceManager::GetTexture(const std::string& assetId) const
	{
		return m_textures.at(assetId);
	}

	void ResourceManager::AddFont(const std::string& assetId, const std::string& path, const int fontSize)
	{
		TTF_Font* font = TTF_OpenFont(path.c_str(), fontSize);
		m_fonts.emplace(assetId, font);

		utils::Logger::Log("New font added to the Asset Store with id= " + assetId);
	}

	TTF_Font* ResourceManager::GetFont(const std::string& assetId) const
	{
		return m_fonts.at(assetId);
	}

	void ResourceManager::ClearAssets()
	{
		for (auto& [id, texture] : m_textures)
			SDL_DestroyTexture(texture);

		m_textures.clear();

		for (const auto& [id, font] : m_fonts)
			TTF_CloseFont(font);

		m_fonts.clear();
	}
}
