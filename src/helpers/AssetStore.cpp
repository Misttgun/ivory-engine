#include "AssetStore.h"
#include <SDL2/SDL_image.h>
#include "Logger.h"

AssetStore::AssetStore() = default;

AssetStore::AssetStore(SDL_Renderer* renderer) : m_renderer(renderer)
{}

AssetStore::~AssetStore()
{
	ClearAssets();
}

void AssetStore::ClearAssets()
{
	for (auto& [id, texture] : m_textures)
		SDL_DestroyTexture(texture);

	m_textures.clear();
}

void AssetStore::AddTexture(const std::string& assetId, const std::string& path)
{
	SDL_Surface* surface = IMG_Load(path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);

	m_textures.emplace(assetId, texture);

	Logger::Log("New texture added to the Asset Store with id= " + assetId);
}

void AssetStore::SetRenderer(SDL_Renderer* renderer)
{
	m_renderer = renderer;
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) const
{
	return m_textures.at(assetId);
}
