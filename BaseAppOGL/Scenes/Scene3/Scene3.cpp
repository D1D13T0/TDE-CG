#include "Scene3.h"


Scene3::Scene3()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(-11.0f, 3.0f, 14.0f), glm::vec3(0.0f, 1.0f, 0.0f), -62.0f, -5.0f);
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
	bIluminacao = true;

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

	// Cria o gerenciador de texturas e carrega as texturas da pasta Exer3
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureMipMap(TEX_CASA, "Scenes/Scene3/CasaAtlas.jpg");
	pTextures->LoadTextureMipMap(TEX_CAIXOTE, "Scenes/Scene3/CRATE.BMP");
	pTextures->LoadTextureMipMap(TEX_GRAMA, "Scenes/Scene3/grama.bmp");

	// Geometria
	pCubo = new CPrimitives();
	pCubo->generateCube(1.0f);
	pCubo->setupBuffers();

	pChao = new CPrimitives();
	pChao->generatePlane(1.0f, 1.0f, 1, 1);
	pChao->setupBuffers();

	pCasa = new CSimpleMesh();
	CriaCasa();
}

Scene3::~Scene3()
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

	if (pCasa) { delete pCasa; pCasa = NULL; }
	if (pCubo) { delete pCubo; pCubo = NULL; }
	if (pChao) { delete pChao; pChao = NULL; }
}


/*
	Regiões do atlas CasaAtlas.jpg (coordenadas UV, v = 0 embaixo):
	  - Parede com porta  : u [0.0, 0.5]  v [0.5, 1.0]
	  - Telhado (telhas)  : u [0.5, 1.0]  v [0.5, 1.0]
	  - Parede com janela : u [0.0, 1.0]  v [0.0, 0.5]  (janela em u ~[0.66, 0.86])
*/
void Scene3::CriaCasa()
{
	const float C = 5.0f;		// Meio comprimento (x)
	const float P = 2.5f;		// Meia profundidade (z)
	const float H = 3.0f;		// Altura das paredes
	const float CUME = 5.0f;	// Altura do cume do telhado

	// Atalho para adicionar uma parede retangular vertical
	auto Parede = [&](glm::vec3 a, glm::vec3 b, glm::vec2 uvMin, glm::vec2 uvMax)
	{
		// a e b são os cantos inferiores (esquerdo e direito, vistos de fora)
		pCasa->AddQuadUV(a, b, b + glm::vec3(0.0f, H, 0.0f), a + glm::vec3(0.0f, H, 0.0f), uvMin, uvMax);
	};

	const glm::vec2 lisoMin(0.0f, 0.0f);			// Madeira sem janela
	const glm::vec2 janelaMin(0.5f, 0.0f), janelaMax(1.0f, 0.5f);
	const glm::vec2 portaMin(0.0f, 0.5f), portaMax(0.5f, 1.0f);

	// ---------------- Parede da frente (z = +P) : janela + porta ----------------
	Parede(glm::vec3(-C, 0, P), glm::vec3(-2.0f, 0, P), lisoMin, glm::vec2(0.5f, 0.5f));
	Parede(glm::vec3(-2.0f, 0, P), glm::vec3(1.0f, 0, P), janelaMin, janelaMax);
	Parede(glm::vec3(1.0f, 0, P), glm::vec3(4.0f, 0, P), portaMin, portaMax);
	Parede(glm::vec3(4.0f, 0, P), glm::vec3(C, 0, P), lisoMin, glm::vec2(0.17f, 0.5f));

	// ---------------- Parede de trás (z = -P) : janela ----------------
	Parede(glm::vec3(C, 0, -P), glm::vec3(2.0f, 0, -P), lisoMin, glm::vec2(0.5f, 0.5f));
	Parede(glm::vec3(2.0f, 0, -P), glm::vec3(-1.0f, 0, -P), janelaMin, janelaMax);
	Parede(glm::vec3(-1.0f, 0, -P), glm::vec3(-C, 0, -P), lisoMin, glm::vec2(0.66f, 0.5f));

	// ---------------- Lateral direita (x = +C) ----------------
	Parede(glm::vec3(C, 0, P), glm::vec3(C, 0, 0.5f), lisoMin, glm::vec2(0.33f, 0.5f));
	Parede(glm::vec3(C, 0, 0.5f), glm::vec3(C, 0, -P), janelaMin, janelaMax);

	// ---------------- Lateral esquerda (x = -C) ----------------
	Parede(glm::vec3(-C, 0, -P), glm::vec3(-C, 0, 0.5f), janelaMin, janelaMax);
	Parede(glm::vec3(-C, 0, 0.5f), glm::vec3(-C, 0, P), lisoMin, glm::vec2(0.33f, 0.5f));

	// ---------------- Oitões (triângulos acima das laterais) ----------------
	glm::vec2 tA(0.0f, 0.12f), tB(0.42f, 0.12f), tC(0.21f, 0.47f);
	pCasa->AddTriangle(glm::vec3(C, H, P), glm::vec3(C, H, -P), glm::vec3(C, CUME, 0.0f), tA, tB, tC);
	pCasa->AddTriangle(glm::vec3(-C, H, -P), glm::vec3(-C, H, P), glm::vec3(-C, CUME, 0.0f), tA, tB, tC);

	// ---------------- Telhado (duas águas com beiral) ----------------
	const float BEIRAL = 0.5f;
	const float xIni = -C - BEIRAL, xFim = C + BEIRAL;
	const float inclinacao = (CUME - H) / P;
	const float zBeiral = P + BEIRAL;
	const float yBeiral = H - BEIRAL * inclinacao;
	const float yCume = CUME + 0.03f;

	// O telhado é dividido em placas para repetir a região das telhas do atlas
	const int NX = 6, NS = 2;
	for (int lado = -1; lado <= 1; lado += 2)	// -1 = trás, +1 = frente
	{
		for (int i = 0; i < NX; i++)
		{
			float x0 = xIni + (xFim - xIni) * i / NX;
			float x1 = xIni + (xFim - xIni) * (i + 1) / NX;
			if (lado < 0) std::swap(x0, x1);		// Mantém a orientação vista de fora

			for (int j = 0; j < NS; j++)
			{
				float t0 = (float)j / NS, t1 = (float)(j + 1) / NS;	// 0 = beiral, 1 = cume
				float y0 = yBeiral + (yCume - yBeiral) * t0;
				float y1 = yBeiral + (yCume - yBeiral) * t1;
				float z0 = lado * zBeiral * (1.0f - t0);
				float z1 = lado * zBeiral * (1.0f - t1);

				pCasa->AddQuadUV(glm::vec3(x0, y0, z0), glm::vec3(x1, y0, z0),
					glm::vec3(x1, y1, z1), glm::vec3(x0, y1, z1),
					glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f));
			}
		}

		// Testeira (espessura do telhado na borda do beiral)
		float zb = lado * zBeiral;
		float xa = (lado > 0) ? xIni : xFim, xb = (lado > 0) ? xFim : xIni;
		pCasa->AddQuadUV(glm::vec3(xa, yBeiral - 0.15f, zb), glm::vec3(xb, yBeiral - 0.15f, zb),
			glm::vec3(xb, yBeiral, zb), glm::vec3(xa, yBeiral, zb),
			glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 0.53f));
	}

	pCasa->Upload();
}


void Scene3::DesenhaCaixote(const glm::vec3& centro, float tamanho, float rotY)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), centro);
	model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(tamanho));
	pShader->SetMat4("model", model);
	pCubo->render();
}


void Scene3::DrawScene(float deltaTime)
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

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
	pShader->SetInt("Texture0", 0);
	pShader->SetVec4("uColor", glm::vec4(1.0f));
	pShader->SetBool("uUseTexture", true);
	pShader->SetBool("uUseLighting", bIluminacao);
	pShader->SetBool("uTwoSided", true);
	pShader->SetVec3("uLightDir", glm::vec3(-0.5f, 0.8f, 0.7f));
	pShader->SetVec3("uViewPos", pCamera->Position);
	pShader->SetFloat("uAmbient", 0.6f);
	pShader->SetFloat("uSpecular", 0.0f);
	glActiveTexture(GL_TEXTURE0);

	// Chão de grama (textura repetida)
	pTextures->ApplyTexture(TEX_GRAMA);
	pShader->SetVec2("uUVScale", 40.0f, 40.0f);
	model = glm::scale(glm::mat4(1.0f), glm::vec3(120.0f, 1.0f, 120.0f));
	pShader->SetMat4("model", model);
	pChao->render();
	pShader->SetVec2("uUVScale", 1.0f, 1.0f);

	// Casa
	pTextures->ApplyTexture(TEX_CASA);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);
	pCasa->Draw();

	// Caixotes (um grande e dois pequenos empilhados)
	pTextures->ApplyTexture(TEX_CAIXOTE);
	DesenhaCaixote(glm::vec3(9.0f, 1.6f, -0.8f), 3.2f, 0.0f);
	DesenhaCaixote(glm::vec3(6.9f, 0.75f, 1.6f), 1.5f, 0.0f);
	DesenhaCaixote(glm::vec3(6.9f, 2.25f, 1.6f), 1.5f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, 0);

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

	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("[ENTER] Show GUI: {0}", bShowGUI ? "Yes" : "No"), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::string("[CAPSLOCK] Wireframe Mode"), 5.0f, 40.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene3::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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

void Scene3::ShowGUI()
{
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;

	ImGui::Begin("Scene Options:");
	ImGui::Checkbox("Mostrar Grid/Eixos", &bShowGrid);
	ImGui::Checkbox("Iluminacao", &bIluminacao);
	ImGui::ColorPicker4("Background Color", (float*)&backgroundColor, flags);
	ImGui::End();
}
