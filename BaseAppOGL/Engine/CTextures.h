#pragma once
#include <vector>
#include <string>

class CTextures
{
public:
	CTextures();
	~CTextures();

	void LoadTextureMipMap(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureAnisotropic(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureLinear(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureDecal(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureClamp(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureNearest(unsigned int ID, const char* filename, bool flipTexture = true);
	void LoadTextureCubeMap(unsigned int ID, std::vector<std::string> faces, bool flipTexture = true);

	void ApplyTexture(unsigned int ID);
	void ApplyTextureCubeMap(unsigned int ID);

	void DeleteTexture(unsigned int ID);

	unsigned int GetGLTextureID(unsigned int ID);

private:
	std::vector<unsigned int> textures;
};

