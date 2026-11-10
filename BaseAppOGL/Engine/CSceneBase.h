#pragma once
#pragma warning(disable : 4099)
#pragma warning(disable : 4005)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <format>
#include <string>

#include "Defines.h"
#include "CInputManager.h"

#include "Engine/imgui/imgui.h"
#include "Engine/imgui/imgui_impl_glfw.h"
#include "Engine/imgui/imgui_impl_opengl3.h"

class CSceneBase
{
	public:
		CSceneBase();
		virtual ~CSceneBase();
		virtual void DrawScene(float deltaTime) = 0;
		virtual void ProcessSceneInput(class GLFWwindow* window, float deltaTime) = 0;
};

