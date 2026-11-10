#include <fstream>
#include "CSceneManager.h"

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Framebuffer size callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void PrintOpenGLInfo()
{
	std::ofstream outFile("OpenGL_Info.txt", std::ios::out | std::ios::trunc);
	if (outFile.is_open())
	{
		outFile << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
		outFile << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
		outFile << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		outFile << "Major OpenGL Version: " << major << std::endl;
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		outFile << "Minor OpenGL Version: " << minor << std::endl;

		outFile << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		GLint numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		outFile << "\nNumber of Supported Extensions : " << numExtensions << std::endl;
		outFile << "List of Supported Extensions:" << std::endl;
		for (int i = 0; i < numExtensions; i++) {
			const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
			outFile << extension << std::endl;
		}

		outFile.close();
		std::cout << "OpenGL info written to OpenGL_Info.txt" << std::endl;
	}
	else
	{
		std::cout << "Failed to write OpenGL info to file" << std::endl;
	}
}

int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Configure GLFW to support OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Get primary monitor info (optional, can be used for fullscreen or positioning)
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	int monitorX, monitorY;
	glfwGetMonitorPos(primary, &monitorX, &monitorY);

	// Create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL BaseApp", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Calculate the centered position
	int posX = monitorX + (mode->width - SCR_WIDTH) / 2;
	int posY = monitorY + (mode->height - SCR_HEIGHT) / 2;
	glfwSetWindowPos(window, posX, posY);
	glfwShowWindow(window);

	// Initialize input manager
	InputManager& input = InputManager::GetInstance();
	input.Initialize(window);
	input.SetCursorMode(GLFW_CURSOR_DISABLED); // Capture cursor for FPS controls

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Configure OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Print OpenGL info on file
	PrintOpenGLInfo();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	// Initialize scene manager
	CSceneManager* pSceneManager = new CSceneManager();

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Calculate delta time
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process input
		pSceneManager->ProcessSceneInput(window, deltaTime);

		// Render scene
		pSceneManager->DrawScene(deltaTime);

		// Update input manager (must be called at end of frame)
		input.Update();

		// Render ImGui on top of the scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Cleanup scene manager
	if (pSceneManager)
	{
		delete pSceneManager;
		pSceneManager = NULL;
	}

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Terminate GLFW
	glfwTerminate();

	return 0;
}
