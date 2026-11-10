#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CPrimitives.h"


// Exercicio 1 - Cena de inverno com arvores e boneco de neve que balanca
class Scene1 : public CSceneBase
{
public:
		Scene1();
		~Scene1();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void ShowGUI();
private:

	// Funcao que desenha uma arvore. Parametros: posicao (x,y,z) e escala (x,y,z)
	void DesenhaArvore(glm::vec3 posicao, glm::vec3 escala);

	// Desenha o boneco de neve (pivo na base), rotacionado em Z pelo angulo informado
	void DesenhaBonecoDeNeve(glm::vec3 posicao, float anguloGraus);

	// Desenha uma primitiva com a matriz de modelo e a cor informadas
	void DesenhaObjeto(CPrimitives* obj, const glm::mat4& model, const glm::vec3& cor, bool iluminacao = true);

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

	// Primitivas basicas
	CPrimitives* pEsfera;
	CPrimitives* pCone;
	CPrimitives* pCilindro;
	CPrimitives* pPlano;

	// Estrelas / flocos de neve
	std::vector<glm::vec3> estrelas;

	// Animacao do boneco de neve
	float fTempo;
	float fAmplitude;		// Metade da variacao (45 graus no total => 22.5)
	float fVelocidade;		// Ciclos por segundo
	float fAnguloBoneco;

	// Mostra a GUI
	bool bShowGUI;
	bool bShowGrid;

	// Background color 
	glm::vec4 backgroundColor;

};
