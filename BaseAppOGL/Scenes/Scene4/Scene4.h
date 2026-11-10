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
#include "CLoaderTxt.h"


// Exercicio 4 - Loader do arquivo Casa.txt
class Scene4 : public CSceneBase
{
public:
		Scene4();
		~Scene4();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void ShowGUI();
private:

	// IDs das texturas (ordem de carregamento)
	enum { TEX_CASA = 0, TEX_GRAMA = 1 };

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

	// Modelo carregado do Casa.txt
	CSimpleMesh* pCasa;
	int iNumTriangulos;
	bool bCasaCarregada;

	// Chão
	CPrimitives* pChao;

	// Mostra a GUI
	bool bShowGUI;
	bool bShowGrid;
	bool bIluminacao;

	// Background color 
	glm::vec4 backgroundColor;

};
