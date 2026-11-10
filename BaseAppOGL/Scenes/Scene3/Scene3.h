#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CPrimitives.h"
#include "Scenes/Common/CSimpleMesh.h"


// Exercicio 3 - Casa de madeira com caixotes usando as texturas da pasta Exer3
class Scene3 : public CSceneBase
{
public:
		Scene3();
		~Scene3();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void ShowGUI();
private:

	// Monta a geometria da casa usando as regioes do atlas CasaAtlas.jpg
	void CriaCasa();

	// Desenha um caixote (cubo texturizado)
	void DesenhaCaixote(const glm::vec3& centro, float tamanho, float rotY);

	// IDs das texturas (ordem de carregamento)
	enum { TEX_CASA = 0, TEX_CAIXOTE = 1, TEX_GRAMA = 2 };

	// Camera
	CCamera* pCamera;

	// Axis
	CGridAxis* pGridAxis;

	// Shader
	CShader* pShader;

	// Textures
	CTextures* pTextures;

	// Timer
	CTimer* pTimer;			

	// Texto
	CText* pText;			

	// Geometria
	CSimpleMesh* pCasa;
	CPrimitives* pCubo;
	CPrimitives* pChao;

	// Mostra a GUI
	bool bShowGUI;
	bool bShowGrid;
	bool bIluminacao;

	// Background color 
	glm::vec4 backgroundColor;

};
