#pragma once
#include <GL/glew.h>
#include <iostream>
#include <map>
#include <string>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <ft2build.h>
#include FT_FREETYPE_H

class CText
{
public:
	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	
	CText();
	~CText();

	int InitTextManager();
	void RenderText(std::string text, float x, float y, float scale);

	

private:
	

	
	std::map<GLchar, Character> Characters;
	unsigned int VAO, VBO;

};

