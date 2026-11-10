#include "Scene1.h"
#include <random>
#include <glm/gtc/constants.hpp>


Scene1::Scene1()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.5f, 3.0f, 11.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -6.0f);
	pCamera->MovementSpeed = 100.0f;

	// Cria o Timer
	pTimer = NULL;
	pTimer = new CTimer();
	pTimer->Init();

	// Cria gerenciador de impressão de texto na tela
	pText = NULL;
	pText = new CText();
	pText->InitTextManager();

	// Mostra a GUI
	bShowGUI = false;
	bShowGrid = false;

	// Background color (céu noturno)
	backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Cria os shaders
	pShader = NULL;
	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Lista1", "Scenes/Common/Lista1.vert", "Scenes/Common/Lista1.frag");


	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas (não usado nesta cena)
	pTextures = NULL;

	// Cria as primitivas básicas
	pEsfera = new CPrimitives();
	pEsfera->generateSphere(1.0f, 36, 18);
	pEsfera->setupBuffers();

	pCone = new CPrimitives();
	pCone->generateCone(1.0f, 1.0f, 36);	// Cone unitário (y de -0.5 a 0.5)
	pCone->setupBuffers();

	pCilindro = new CPrimitives();
	pCilindro->generateCylinder(1.0f, 1.0f, 36);	// Cilindro unitário (y de -0.5 a 0.5)
	pCilindro->setupBuffers();

	pPlano = new CPrimitives();
	pPlano->generatePlane(1.0f, 1.0f, 1, 1);
	pPlano->setupBuffers();

	// Gera posições aleatórias das estrelas / flocos de neve
	std::mt19937 rng(1234);
	std::uniform_real_distribution<float> distXZ(-60.0f, 60.0f);
	std::uniform_real_distribution<float> distY(0.0f, 40.0f);
	for (int i = 0; i < 400; i++)
	{
		glm::vec3 p(distXZ(rng), distY(rng), distXZ(rng) - 25.0f);
		estrelas.push_back(p);
	}

	// Animação: balança 45 graus no total (de -22.5 a +22.5)
	fTempo = 0.0f;
	fAmplitude = 22.5f;
	fVelocidade = 0.5f;
	fAnguloBoneco = 0.0f;
}

Scene1::~Scene1()
{

	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}

	if (pShader)
	{
		delete pShader;
		pShader = NULL;
	}

	if (pGridAxis)
	{
		delete pGridAxis;
		pGridAxis = NULL;
	}

	if (pTextures)
	{
		delete pTextures;
		pTextures = NULL;
	}

	if (pTimer)
	{
		delete pTimer;
		pTimer = NULL;
	}

	if (pText)
	{
		delete pText;
		pText = NULL;
	}

	if (pEsfera) { delete pEsfera; pEsfera = NULL; }
	if (pCone) { delete pCone; pCone = NULL; }
	if (pCilindro) { delete pCilindro; pCilindro = NULL; }
	if (pPlano) { delete pPlano; pPlano = NULL; }
}


void Scene1::DesenhaObjeto(CPrimitives* obj, const glm::mat4& model, const glm::vec3& cor, bool iluminacao)
{
	pShader->SetMat4("model", model);
	pShader->SetVec4("uColor", glm::vec4(cor, 1.0f));
	pShader->SetBool("uUseLighting", iluminacao);
	obj->render();
}


// Função que cria uma árvore. Parâmetros: Posição (x,y,z) e Escala (x,y,z)
void Scene1::DesenhaArvore(glm::vec3 posicao, glm::vec3 escala)
{
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicao);
	base = glm::scale(base, escala);

	glm::mat4 model;

	// Tronco (cilindro laranja)
	model = glm::translate(base, glm::vec3(0.0f, 0.9f, 0.0f));
	model = glm::scale(model, glm::vec3(0.28f, 1.8f, 0.28f));
	DesenhaObjeto(pCilindro, model, glm::vec3(1.0f, 0.45f, 0.0f));

	// Copa inferior (cone verde escuro)
	model = glm::translate(base, glm::vec3(0.0f, 3.1f, 0.0f));
	model = glm::scale(model, glm::vec3(1.6f, 3.0f, 1.6f));
	DesenhaObjeto(pCone, model, glm::vec3(0.0f, 0.55f, 0.0f));

	// Copa superior (cone verde claro)
	model = glm::translate(base, glm::vec3(0.0f, 4.8f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 2.8f, 1.25f));
	DesenhaObjeto(pCone, model, glm::vec3(0.05f, 0.85f, 0.05f));
}


// Boneco de neve: pivô na base para balançar da esquerda para a direita
void Scene1::DesenhaBonecoDeNeve(glm::vec3 posicao, float anguloGraus)
{
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicao);
	base = glm::rotate(base, glm::radians(anguloGraus), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 branco(1.0f, 1.0f, 1.0f);
	glm::mat4 model;

	pShader->SetFloat("uSpecular", 0.6f);

	// Corpo (esfera inferior)
	model = glm::translate(base, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f));
	DesenhaObjeto(pEsfera, model, branco);

	// Tronco (esfera do meio)
	model = glm::translate(base, glm::vec3(0.0f, 2.45f, 0.0f));
	model = glm::scale(model, glm::vec3(0.7f));
	DesenhaObjeto(pEsfera, model, branco);

	// Cabeça
	model = glm::translate(base, glm::vec3(0.0f, 3.45f, 0.0f));
	model = glm::scale(model, glm::vec3(0.48f));
	DesenhaObjeto(pEsfera, model, branco);

	pShader->SetFloat("uSpecular", 0.2f);

	// Botões vermelhos
	model = glm::translate(base, glm::vec3(0.0f, 2.65f, 0.66f));
	model = glm::scale(model, glm::vec3(0.08f));
	DesenhaObjeto(pEsfera, model, glm::vec3(0.9f, 0.0f, 0.0f));

	model = glm::translate(base, glm::vec3(0.0f, 2.35f, 0.69f));
	model = glm::scale(model, glm::vec3(0.08f));
	DesenhaObjeto(pEsfera, model, glm::vec3(0.9f, 0.0f, 0.0f));

	// Olhos
	model = glm::translate(base, glm::vec3(-0.16f, 3.55f, 0.43f));
	model = glm::scale(model, glm::vec3(0.06f));
	DesenhaObjeto(pEsfera, model, glm::vec3(0.05f));

	model = glm::translate(base, glm::vec3(0.16f, 3.55f, 0.43f));
	model = glm::scale(model, glm::vec3(0.06f));
	DesenhaObjeto(pEsfera, model, glm::vec3(0.05f));

	// Nariz (cone laranja apontando para frente)
	model = glm::translate(base, glm::vec3(0.0f, 3.42f, 0.62f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.08f, 0.35f, 0.08f));
	DesenhaObjeto(pCone, model, glm::vec3(1.0f, 0.5f, 0.0f));

	// Chapéu azul: aba + copa
	model = glm::translate(base, glm::vec3(0.0f, 3.86f, 0.0f));
	model = glm::scale(model, glm::vec3(0.55f, 0.06f, 0.55f));
	DesenhaObjeto(pCilindro, model, glm::vec3(0.2f, 0.2f, 1.0f));

	model = glm::translate(base, glm::vec3(0.0f, 4.15f, 0.0f));
	model = glm::scale(model, glm::vec3(0.35f, 0.55f, 0.35f));
	DesenhaObjeto(pCilindro, model, glm::vec3(0.2f, 0.2f, 1.0f));
}


void Scene1::DrawScene(float deltaTime)
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	// Atualiza a animação do boneco: oscila entre -22.5° e +22.5° (45° de variação)
	fTempo += deltaTime;
	fAnguloBoneco = fAmplitude * sinf(2.0f * glm::pi<float>() * fVelocidade * fTempo);

	// Clear the color and depth buffers
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	if (bShowGrid)
	{
		// Draw the 3D grid
		pShader->Use("Grid");
		pShader->SetMat4("uProj", projection);
		pShader->SetMat4("uView", view);
		pShader->SetFloat("uGridSpacing", 1.0f);
		pShader->SetFloat("uFadeStart", 18.f);
		pShader->SetFloat("uFadeEnd", 100.f);
		pShader->SetVec3("uCamPos", pCamera->Position);
		pGridAxis->DrawGrid();
		// Draw the 3D axis
		pShader->Use("Axis");
		pShader->SetMat4("uProj", projection);
		pShader->SetMat4("uView", view);
		pGridAxis->DrawAxis();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	pShader->Use("Lista1");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	pShader->SetVec2("uUVScale", 1.0f, 1.0f);
	pShader->SetBool("uUseTexture", false);
	pShader->SetBool("uTwoSided", false);
	pShader->SetVec3("uLightDir", glm::vec3(0.4f, 0.8f, 0.6f));
	pShader->SetVec3("uViewPos", pCamera->Position);
	pShader->SetFloat("uAmbient", 0.35f);
	pShader->SetFloat("uSpecular", 0.2f);

	// Chão de neve (branco)
	model = glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 1.0f, 200.0f));
	DesenhaObjeto(pPlano, model, glm::vec3(1.0f, 1.0f, 1.0f), false);

	// Estrelas / flocos de neve (sem iluminação)
	// Os flocos caem continuamente; ao tocar o chão voltam para o topo
	const float ALTURA_NEVE = 40.0f;
	for (size_t i = 0; i < estrelas.size(); i++)
	{
		glm::vec3& p = estrelas[i];
		float velocidade = 1.5f + 0.5f * (float)(i % 5);		// Cada floco cai em uma velocidade
		p.y -= velocidade * deltaTime;
		if (p.y < 0.0f)
			p.y += ALTURA_NEVE;

		// Leve balanço lateral durante a queda
		float balanco = 0.3f * sinf(fTempo * 1.5f + (float)i);

		model = glm::translate(glm::mat4(1.0f), p + glm::vec3(balanco, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f));
		DesenhaObjeto(pEsfera, model, glm::vec3(1.0f), false);
	}

	// Árvores: DesenhaArvore(posição, escala)
	DesenhaArvore(glm::vec3(-3.2f, 0.0f, 4.5f), glm::vec3(1.0f, 1.1f, 1.0f));
	DesenhaArvore(glm::vec3(-5.5f, 0.0f, -2.0f), glm::vec3(1.2f, 1.2f, 1.2f));
	DesenhaArvore(glm::vec3(-1.0f, 0.0f, -6.0f), glm::vec3(1.4f, 1.3f, 1.4f));
	DesenhaArvore(glm::vec3(3.5f, 0.0f, -9.0f), glm::vec3(1.6f, 1.5f, 1.6f));
	DesenhaArvore(glm::vec3(8.5f, 0.0f, -2.5f), glm::vec3(1.2f, 1.2f, 1.2f));
	DesenhaArvore(glm::vec3(-10.0f, 0.0f, -10.0f), glm::vec3(1.5f, 1.6f, 1.5f));
	DesenhaArvore(glm::vec3(11.0f, 0.0f, -12.0f), glm::vec3(1.7f, 1.7f, 1.7f));
	DesenhaArvore(glm::vec3(-3.0f, 0.0f, -16.0f), glm::vec3(1.8f, 1.8f, 1.8f));
	DesenhaArvore(glm::vec3(6.0f, 0.0f, -20.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	DesenhaArvore(glm::vec3(-14.0f, 0.0f, -2.0f), glm::vec3(1.3f, 1.3f, 1.3f));

	// Boneco de neve balançando
	DesenhaBonecoDeNeve(glm::vec3(4.8f, 0.0f, -2.0f), fAnguloBoneco);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw text on the screen (disable depth test and use orthographic projection)
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 0.05f, 0.05f, 0.25f);	// Texto escuro para aparecer sobre a neve branca

	pText->RenderText(std::format("Angulo do boneco: {0:.1f} graus", fAnguloBoneco), 5.0f, 120.0f, 0.3f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("[ENTER] Show GUI: {0}", bShowGUI ? "Yes" : "No"), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::string("[CAPSLOCK] Wireframe Mode"), 5.0f, 40.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene1::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Toggle GUI display and cursor mode when Enter key is pressed
	if (input.IsKeyJustPressed(GLFW_KEY_ENTER))
	{
		bShowGUI = !bShowGUI;
		input.SetCursorMode(bShowGUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		std::cout << "Cursor Disabled: '" << bShowGUI << "'" << std::endl;
	}

	// Process camera movement and mouse input only when GUI is not displayed
	if (bShowGUI == false)
	{
		// Camera movement
		if (input.IsKeyPressed(GLFW_KEY_W))
			pCamera->ProcessKeyboard(FORWARD, deltaTime);
		if (input.IsKeyPressed(GLFW_KEY_S))
			pCamera->ProcessKeyboard(BACKWARD, deltaTime);
		if (input.IsKeyPressed(GLFW_KEY_A))
			pCamera->ProcessKeyboard(LEFT, deltaTime);
		if (input.IsKeyPressed(GLFW_KEY_D))
			pCamera->ProcessKeyboard(RIGHT, deltaTime);
		if (input.IsKeyPressed(GLFW_KEY_E))
			pCamera->ProcessKeyboard(UP, deltaTime);
		if (input.IsKeyPressed(GLFW_KEY_Q))
			pCamera->ProcessKeyboard(DOWN, deltaTime);

		// Increase/decrease movement speed
		if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			pCamera->MovementSpeed = SPEED_MULTIPLIER;
		else
			pCamera->MovementSpeed = SPEED;

		// Process mouse movement
		glm::vec2 mouseDelta = input.GetMouseDelta();
		if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
		{
			pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
		}

		// Process scroll
		float scrollOffset = input.GetScrollOffset();
		if (scrollOffset != 0.0f)
		{
			pCamera->ProcessMouseScroll(scrollOffset);
		}
	}
	else
	{
		ShowGUI(); // Call the ShowGUI function to display the GUI when bShowGUI is true
	}
}

void Scene1::ShowGUI()
{
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;

	ImGui::Begin("Scene Options:");
	ImGui::Checkbox("Mostrar Grid/Eixos", &bShowGrid);
	ImGui::SliderFloat("Velocidade (ciclos/s)", &fVelocidade, 0.1f, 2.0f);
	ImGui::ColorPicker4("Background Color", (float*)&backgroundColor, flags);
	ImGui::End();
}
