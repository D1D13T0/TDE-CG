#include "Scene2.h"


Scene2::Scene2()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 6.5f, 19.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -3.0f);
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

	// Background color
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

	// Cubo de tamanho 1
	pCubo = new CPrimitives();
	pCubo->generateCube(TAM_CUBO);
	pCubo->setupBuffers();

	// Simulação
	rng.seed(std::random_device{}());
	fIntervalo = 1.0f;
	iCiclo = 0;
	ReiniciaSimulacao();
}

Scene2::~Scene2()
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

	if (pCubo)
	{
		delete pCubo;
		pCubo = NULL;
	}
}


void Scene2::ReiniciaSimulacao()
{
	cubos.clear();
	for (int i = 0; i < NUM_PILHAS; i++)
		alturaPilha[i] = 0;

	fTempoGeracao = 0.0f;
	fTempoReinicio = 0.0f;
	iCiclo++;
}


float Scene2::XDaPilha(int pilha) const
{
	// Pilhas centralizadas em x = 0
	return (pilha - (NUM_PILHAS - 1) * 0.5f) * ESPACO_PILHAS;
}


// Gera um cubo com cor aleatória e o insere em uma pilha aleatória que ainda tenha espaço
void Scene2::GeraCubo()
{
	std::vector<int> disponiveis;
	for (int i = 0; i < NUM_PILHAS; i++)
		if (alturaPilha[i] < MAX_CUBOS)
			disponiveis.push_back(i);

	if (disponiveis.empty())
		return;

	std::uniform_int_distribution<int> escolhePilha(0, (int)disponiveis.size() - 1);
	std::uniform_real_distribution<float> cor(0.0f, 1.0f);

	Cubo c;
	c.pilha = disponiveis[escolhePilha(rng)];
	c.nivel = alturaPilha[c.pilha];		// Fica exatamente acima do último cubo da pilha
	c.y = ALTURA_QUEDA;
	c.cor = glm::vec3(cor(rng), cor(rng), cor(rng));
	c.caindo = true;

	alturaPilha[c.pilha]++;
	cubos.push_back(c);
}


void Scene2::DesenhaCaixa(const glm::vec3& centro, const glm::vec3& tamanho, const glm::vec3& cor)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), centro);
	model = glm::scale(model, tamanho);
	pShader->SetMat4("model", model);
	pShader->SetVec4("uColor", glm::vec4(cor, 1.0f));
	pCubo->render();
}


void Scene2::DrawScene(float deltaTime)
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	/////////////////////////////////////////////////////////////////////////////////
	// Atualiza a simulação
	bool todasCompletas = true;
	for (int i = 0; i < NUM_PILHAS; i++)
		if (alturaPilha[i] < MAX_CUBOS)
			todasCompletas = false;

	if (!todasCompletas)
	{
		// 1 cubo a cada 1 segundo de simulação
		fTempoGeracao += deltaTime;
		while (fTempoGeracao >= fIntervalo)
		{
			fTempoGeracao -= fIntervalo;
			GeraCubo();
		}
	}

	// Queda dos cubos até a posição final (exatamente acima do último cubo)
	bool algumCaindo = false;
	for (Cubo& c : cubos)
	{
		if (!c.caindo) continue;

		float yFinal = TAM_CUBO * 0.5f + c.nivel * TAM_CUBO;
		c.y -= VEL_QUEDA * deltaTime;
		if (c.y <= yFinal)
		{
			c.y = yFinal;
			c.caindo = false;
		}
		else
			algumCaindo = true;
	}

	// Reinicia quando todas as pilhas estiverem completas
	if (todasCompletas && !algumCaindo)
	{
		fTempoReinicio += deltaTime;
		if (fTempoReinicio >= 1.0f)
			ReiniciaSimulacao();
	}

	// Clear the color and depth buffers
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();

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
	pShader->SetBool("uUseLighting", true);
	pShader->SetBool("uTwoSided", false);
	pShader->SetVec3("uLightDir", glm::vec3(0.5f, 0.7f, 1.0f));
	pShader->SetVec3("uViewPos", pCamera->Position);
	pShader->SetFloat("uAmbient", 0.35f);
	pShader->SetFloat("uSpecular", 0.15f);

	// Estrutura cinza que contém as pilhas (base + 2 colunas laterais)
	glm::vec3 cinza(0.55f, 0.55f, 0.55f);
	float larguraInterna = NUM_PILHAS * ESPACO_PILHAS;
	float xLateral = larguraInterna * 0.5f + 0.25f;
	float alturaColuna = MAX_CUBOS * TAM_CUBO + 1.5f;

	DesenhaCaixa(glm::vec3(0.0f, -0.25f, 0.0f), glm::vec3(larguraInterna + 1.0f, 0.5f, 1.4f), cinza);
	DesenhaCaixa(glm::vec3(-xLateral, alturaColuna * 0.5f - 0.5f, 0.0f), glm::vec3(0.5f, alturaColuna, 0.5f), cinza);
	DesenhaCaixa(glm::vec3(xLateral, alturaColuna * 0.5f - 0.5f, 0.0f), glm::vec3(0.5f, alturaColuna, 0.5f), cinza);

	// Cubos
	for (const Cubo& c : cubos)
		DesenhaCaixa(glm::vec3(XDaPilha(c.pilha), c.y, 0.0f), glm::vec3(1.0f), c.cor);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw text on the screen (disable depth test and use orthographic projection)
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText(std::format("Simulacao: {0}   Cubos: {1}/{2}   Pilhas: {3} {4} {5} {6} {7}",
		iCiclo, (int)cubos.size(), NUM_PILHAS * MAX_CUBOS,
		alturaPilha[0], alturaPilha[1], alturaPilha[2], alturaPilha[3], alturaPilha[4]), 5.0f, 120.0f, 0.3f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("[ENTER] Show GUI: {0}", bShowGUI ? "Yes" : "No"), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::string("[CAPSLOCK] Wireframe Mode"), 5.0f, 40.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene2::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Toggle GUI display and cursor mode when Enter key is pressed
	if (input.IsKeyJustPressed(GLFW_KEY_ENTER))
	{
		bShowGUI = !bShowGUI;
		input.SetCursorMode(bShowGUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		std::cout << "Cursor Disabled: '" << bShowGUI << "'" << std::endl;
	}

	// Reinicia a simulação manualmente
	if (input.IsKeyJustPressed(GLFW_KEY_R))
		ReiniciaSimulacao();

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

void Scene2::ShowGUI()
{
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;

	ImGui::Begin("Scene Options:");
	ImGui::Checkbox("Mostrar Grid/Eixos", &bShowGrid);
	if (ImGui::Button("Reiniciar simulacao"))
		ReiniciaSimulacao();
	ImGui::ColorPicker4("Background Color", (float*)&backgroundColor, flags);
	ImGui::End();
}
