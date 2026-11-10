#include "CLoaderTxt.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

bool CLoaderTxt::Load(const std::string& path, CSimpleMesh& mesh, int* numTriangulos)
{
	std::ifstream arquivo(path);
	if (!arquivo.is_open())
	{
		std::cerr << "CLoaderTxt: nao foi possivel abrir '" << path << "'" << std::endl;
		return false;
	}

	// Le todos os numeros do arquivo (virgulas e quebras de linha sao separadores)
	std::stringstream buffer;
	buffer << arquivo.rdbuf();
	std::string conteudo = buffer.str();
	for (char& c : conteudo)
		if (c == ',' || c == ';')
			c = ' ';

	std::istringstream entrada(conteudo);
	std::vector<float> valores;
	float valor;
	while (entrada >> valor)
		valores.push_back(valor);

	// 5 valores por vertice (x, y, z, u, v) e 3 vertices por triangulo
	const size_t POR_VERTICE = 5;
	const size_t POR_TRIANGULO = POR_VERTICE * 3;

	if (valores.size() < POR_TRIANGULO)
	{
		std::cerr << "CLoaderTxt: arquivo sem triangulos validos: '" << path << "'" << std::endl;
		return false;
	}
	if (valores.size() % POR_TRIANGULO != 0)
		std::cerr << "CLoaderTxt: aviso - " << valores.size() % POR_TRIANGULO << " valores restantes ignorados" << std::endl;

	mesh.Clear();
	size_t total = valores.size() / POR_TRIANGULO;
	for (size_t t = 0; t < total; t++)
	{
		const float* v = &valores[t * POR_TRIANGULO];
		glm::vec3 p[3];
		glm::vec2 uv[3];
		for (int i = 0; i < 3; i++)
		{
			p[i] = glm::vec3(v[i * 5 + 0], v[i * 5 + 1], v[i * 5 + 2]);
			uv[i] = glm::vec2(v[i * 5 + 3], v[i * 5 + 4]);
		}
		mesh.AddTriangle(p[0], p[1], p[2], uv[0], uv[1], uv[2]);
	}
	mesh.Upload();

	if (numTriangulos)
		*numTriangulos = (int)total;

	std::cout << "CLoaderTxt: '" << path << "' carregado com " << total << " triangulos" << std::endl;
	return true;
}
