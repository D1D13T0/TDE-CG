#pragma once
#include <string>
#include "Scenes/Common/CSimpleMesh.h"

// Loader do formato do arquivo Casa.txt:
//   Cada vertice ocupa duas linhas:  x, y, z,  (posicao)
//                                    u, v,     (coordenada de textura)
//   A cada 3 vertices forma-se um triangulo (linhas em branco separam os triangulos).
class CLoaderTxt
{
public:
	// Le o arquivo e preenche a malha. Retorna false em caso de erro.
	static bool Load(const std::string& path, CSimpleMesh& mesh, int* numTriangulos = nullptr);
};
