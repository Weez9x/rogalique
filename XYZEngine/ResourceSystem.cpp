#include "pch.h"
#include "ResourceSystem.h"

namespace XYZEngine
{
	ResourceSystem* ResourceSystem::Instance()
	{
		static ResourceSystem resourceSystem;
		return &resourceSystem;
	}

	void ResourceSystem::LoadTexture(const std::string& name, std::string sourcePath, bool isSmooth)
	{
		if (textures.find(name) != textures.end())
		{
			return;
		}

		sf::Texture* newTexture = new sf::Texture();
		if (newTexture->loadFromFile(sourcePath))
		{
			newTexture->setSmooth(isSmooth);
			textures.emplace(name, newTexture);
		}
	}
	const sf::Texture* ResourceSystem::GetTextureShared(const std::string& name) const
	{
		return textures.find(name)->second;
	}
	sf::Texture* ResourceSystem::GetTextureCopy(const std::string& name) const
	{
		return new sf::Texture(*textures.find(name)->second);
	}
	void ResourceSystem::DeleteSharedTexture(const std::string& name)
	{
		auto texturePair = textures.find(name);

		sf::Texture* deletingTexure = texturePair->second;
		textures.erase(texturePair);
		delete deletingTexure;
	}

	void ResourceSystem::LoadTextureMap(const std::string& name, std::string sourcePath, sf::Vector2u elementPixelSize, int totalElements, bool isSmooth)
	{
		if (textureMaps.find(name) != textureMaps.end())
		{
			return;
		}

		sf::Texture textureMap;
		if (textureMap.loadFromFile(sourcePath))
		{
			auto textureMapElements = new std::vector<sf::Texture*>();

			auto textureSize = textureMap.getSize();
			int loadedElements = 0;

			for (int y = 0; y <= textureSize.y - elementPixelSize.y; y += elementPixelSize.y)
			{
				if (loadedElements == totalElements)
				{
					break;
				}

				for (int x = 0; x <= textureSize.x - elementPixelSize.x; x += elementPixelSize.x)
				{
					if (loadedElements == totalElements)
					{
						break;
					}

					sf::Texture* newTextureMapElement = new sf::Texture();
					if (newTextureMapElement->loadFromFile(sourcePath, sf::IntRect(x, y, elementPixelSize.x, elementPixelSize.y)))
					{
						newTextureMapElement->setSmooth(isSmooth);
						textureMapElements->push_back(newTextureMapElement);
					}
					loadedElements++;
				}
			}

			textureMaps.emplace(name, *textureMapElements);
		}
	}
	const sf::Texture* ResourceSystem::GetTextureMapElementShared(const std::string& name, int elementIndex) const
	{
		auto textureMap = textureMaps.find(name);
		auto textures = textureMap->second;
		return textures[elementIndex];
	}
	sf::Texture* ResourceSystem::GetTextureMapElementCopy(const std::string& name, int elementIndex) const
	{
		auto textureMap = textureMaps.find(name);
		auto textures = textureMap->second;
		return new sf::Texture(*textures[elementIndex]);
	}
	int ResourceSystem::GetTextureMapElementsCount(const std::string& name) const
	{
		auto textureMap = textureMaps.find(name);
		auto textures = textureMap->second;
		return textures.size();
	}
	void ResourceSystem::DeleteSharedTextureMap(const std::string& name)
	{
		auto textureMap = textureMaps.find(name);
		auto deletingTextures = textureMap->second;

		for (int i = 0; i < deletingTextures.size(); i++)
		{
			delete deletingTextures[i];
		}

		textureMaps.erase(textureMap);
	}

	void ResourceSystem::loadMusic(const std::string& name, const std::string& sourcePath)
	{
		if (musics.find(name) != musics.end())
		{
			return;
		}
		sf::Music* newMusic = new sf::Music();
		if (newMusic->openFromFile(sourcePath))
		{
			musics.emplace(name, newMusic);
		}
		else
		{
			delete newMusic;
		}
	}

	sf::Music* ResourceSystem::GetMusicShared(const std::string& name) const
	{
		auto it = musics.find(name);
		if (it == musics.end())
		{
			return nullptr;
		}
		return it->second;
	}

	void ResourceSystem::DeleteSharedMusic(const std::string& name)
	{
		auto it = musics.find(name);
		if (it == musics.end())
			return;
		sf::Music* deletingMusic = it->second;

		if (currentMusicName == name)
		{
			deletingMusic->stop();
			currentMusicName.clear();
		}

		musics.erase(it);
		delete deletingMusic;
	}

	bool ResourceSystem::PlayMusic(const std::string& name, bool loop, float volume)
	{
		sf::Music* music = GetMusicShared(name);
		if (music)
			return false;
		if (!currentMusicName.empty() && currentMusicName != name)
		{
			StopMusic();
		}
		
		music->setLoop(loop);
		music->setVolume(volume);
		music->play();

		currentMusicName = name;
		return true;
	}

	void ResourceSystem::StopMusic()
	{
		if (currentMusicName.empty())
			return;

		sf::Music* music = GetMusicShared(currentMusicName);
		if (music)
		{
			music->stop();
		}

		currentMusicName.clear();
	}

	void ResourceSystem::SetMusicVolume(float volume)
	{
		if (currentMusicName.empty())
			return;
		sf::Music* music = GetMusicShared(currentMusicName);
		if (music)
		{
			music->setVolume(volume);
		}
	}

	void ResourceSystem::Clear()
	{
		DeleteAllTextures();
		DeleteAllTextureMaps();
		DeleteAllMusics();
	}

	void ResourceSystem::DeleteAllTextures()
	{
		std::vector<std::string> keysToDelete;

		for (const auto& texturePair : textures)
		{
			keysToDelete.push_back(texturePair.first);
		}

		for (const auto& key : keysToDelete)
		{
			DeleteSharedTexture(key);
		}
	}
	void ResourceSystem::DeleteAllTextureMaps()
	{
		std::vector<std::string> keysToDelete;

		for (const auto& textureMapPair : textureMaps)
		{
			keysToDelete.push_back(textureMapPair.first);
		}

		for (const auto& key : keysToDelete)
		{
			DeleteSharedTextureMap(key);
		}
	}
	void ResourceSystem::DeleteAllMusics()
	{
		std::vector<std::string> keysToDelete;
		for (const auto& pair : musics)
			keysToDelete.push_back(pair.first);
		for (const auto& key : keysToDelete)
			DeleteSharedMusic(key);
	}
}