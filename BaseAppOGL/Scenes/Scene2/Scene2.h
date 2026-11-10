#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CPrimitives.h"
#include <random>


// Exercicio 2 - Simulacao de empilhamento de cubos
class Scene2 : public CSceneBase
{
public:
		Scene2();
		~Scene2();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void ShowGUI();
private:

	// Constantes do exercicio
	static const int NUM_PILHAS = 5;		// Total de 5 pilhas
	static const int MAX_CUBOS = 10;		// Cada pilha armazena 10 cubos
	const float TAM_CUBO = 1.0f;			// Cubos de tamanho 1
	const float ESPACO_PILHAS = 1.5f;		// Distancia entre os centros das pilhas
	const float ALTURA_QUEDA = 13.0f;		// Altura em que o cubo nasce
	const float VEL_QUEDA = 9.0f;			// Velocidade de queda (unidades/s)

	struct Cubo
	{
		int pilha;			// Indice da pilha
		int nivel;			// Posicao dentro da pilha (0 = base)
		float y;			// Altura atual (centro do cubo)
		glm::vec3 cor;		// Cor aleatoria
		bool caindo;
	};

	void ReiniciaSimulacao();
	void GeraCubo();
	float XDaPilha(int pilha) const;
	void DesenhaCaixa(const glm::vec3& centro, const glm::vec3& tamanho, const glm::vec3& cor);

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

	// Cubo unitario
	CPrimitives* pCubo;

	// Estado da simulacao
	std::vector<Cubo> cubos;
	int alturaPilha[NUM_PILHAS];	// Quantidade de cubos (ja reservados) em cada pilha
	float fTempoGeracao;			// Acumulador para gerar 1 cubo por segundo
	float fIntervalo;				// Intervalo de geracao (1 segundo)
	float fTempoReinicio;			// Pausa antes de reiniciar
	int iCiclo;						// Numero da simulacao atual

	std::mt19937 rng;

	// Mostra a GUI
	bool bShowGUI;
	bool bShowGrid;

	// Background color 
	glm::vec4 backgroundColor;

};
