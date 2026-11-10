#pragma once
// Malha simples (posicao, normal, coordenada de textura) desenhada com glDrawArrays.
// Mesmo layout de atributos usado por CPrimitives: 0 = posicao, 1 = normal, 2 = UV.

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class CSimpleMesh
{
public:
	CSimpleMesh() : VAO(0), VBO(0), vertexCount(0) {}
	~CSimpleMesh() { Cleanup(); }

	CSimpleMesh(const CSimpleMesh&) = delete;
	CSimpleMesh& operator=(const CSimpleMesh&) = delete;

	// Adiciona um triangulo calculando a normal da face
	void AddTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
		const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2)
	{
		glm::vec3 n = glm::cross(p1 - p0, p2 - p0);
		float len = glm::length(n);
		n = (len > 0.0f) ? n / len : glm::vec3(0.0f, 1.0f, 0.0f);

		PushVertex(p0, n, t0);
		PushVertex(p1, n, t1);
		PushVertex(p2, n, t2);
	}

	// Adiciona um quadrilatero (p0,p1,p2,p3 em sentido anti-horario)
	void AddQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
		const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3)
	{
		AddTriangle(p0, p1, p2, t0, t1, t2);
		AddTriangle(p0, p2, p3, t0, t2, t3);
	}

	// Quadrilatero com um retangulo de UV (uvMin = canto p0, uvMax = canto p2)
	void AddQuadUV(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
		const glm::vec2& uvMin, const glm::vec2& uvMax)
	{
		AddQuad(p0, p1, p2, p3,
			glm::vec2(uvMin.x, uvMin.y), glm::vec2(uvMax.x, uvMin.y),
			glm::vec2(uvMax.x, uvMax.y), glm::vec2(uvMin.x, uvMax.y));
	}

	// Envia os dados para a GPU
	void Upload()
	{
		Cleanup();
		vertexCount = (GLsizei)(data.size() / 8);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	void Draw() const
	{
		if (!VAO) return;
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(0);
	}

	void Clear() { data.clear(); }
	GLsizei GetVertexCount() const { return (GLsizei)(data.size() / 8); }

	void Cleanup()
	{
		if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
		if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
	}

private:
	void PushVertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& t)
	{
		data.insert(data.end(), { p.x, p.y, p.z, n.x, n.y, n.z, t.x, t.y });
	}

	std::vector<float> data;
	GLuint VAO, VBO;
	GLsizei vertexCount;
};
