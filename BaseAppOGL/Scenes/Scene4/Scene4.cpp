#include "Scene4.h"


Scene4::Scene4()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(-20.0f, 9.0f, 24.0f), glm::vec3(0.0f, 1.0f, 0.0f), -50.0f, -9.0f);
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
	bShowGrid = true;
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

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureMipMap(TEX_CASA, "Scenes/Scene4/CasaTex.jpg");
	pTextures->LoadTextureMipMap(TEX_GRAMA, "Scenes/Textures/grass.bmp");

	// Carrega o modelo da casa usando o loader do Casa.txt
	iNumTriangulos = 0;
	pCasa = new CSimpleMesh();
	bCasaCarregada = CLoaderTxt::Load("Scenes/Scene4/Casa.txt", *pCasa, &iNumTriangulos);

	// Chão
	pChao = new CPrimitives();
	pChao->generatePlane(1.0f, 1.0f, 1, 1);
	pChao->setupBuffers();
}

Scene4::~Scene4()
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
	if (pChao) { delete pChao; pChao = NULL; }
}

void Scene4::DrawScene(float deltaTime)
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
	pShader->SetBool("uTwoSided", true);		// O modelo é aberto (porta/janelas): ilumina as duas faces
	pShader->SetVec3("uLightDir", glm::vec3(-0.6f, 0.8f, 0.5f));
	pShader->SetVec3("uViewPos", pCamera->Position);
	pShader->SetFloat("uAmbient", 0.6f);
	pShader->SetFloat("uSpecular", 0.0f);
	glActiveTexture(GL_TEXTURE0);

	// Chão de grama
	pTextures->ApplyTexture(TEX_GRAMA);
	pShader->SetVec2("uUVScale", 12.0f, 12.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f));
	model = glm::scale(model, glm::vec3(150.0f, 1.0f, 150.0f));
	pShader->SetMat4("model", model);
	pChao->render();
	pShader->SetVec2("uUVScale", 1.0f, 1.0f);

	// Casa carregada do arquivo Casa.txt
	if (bCasaCarregada)
	{
		pTextures->ApplyTexture(TEX_CASA);
		pShader->SetMat4("model", glm::mat4(1.0f));
		pCasa->Draw();
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	if (bShowGrid)
	{
		// Draw the 3D axis (como na imagem de referência)
		pShader->Use("Axis");
		pShader->SetMat4("uProj", projection);
		pShader->SetMat4("uView", view);
		pGridAxis->DrawAxis();
	}

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

	pText->RenderText(bCasaCarregada ? std::format("Casa.txt carregado: {0} triangulos", iNumTriangulos)
		: std::string("ERRO ao carregar Scenes/Scene4/Casa.txt"), 5.0f, 120.0f, 0.3f);
	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);
	pText->RenderText(std::format("[ENTER] Show GUI: {0}", bShowGUI ? "Yes" : "No"), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::string("[CAPSLOCK] Wireframe Mode"), 5.0f, 40.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene4::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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

void Scene4::ShowGUI()
{
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;

	ImGui::Begin("Scene Options:");
	ImGui::Checkbox("Mostrar Eixos", &bShowGrid);
	ImGui::Checkbox("Iluminacao", &bIluminacao);
	ImGui::ColorPicker4("Background Color", (float*)&backgroundColor, flags);
	ImGui::End();
}
