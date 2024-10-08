#pragma once

#include <exception>

#include "stb_image.h"
#include "stb_image_write.h"

#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "Shader.h"
#include "RenderData.h"
#include "SmartTimer.h"

float lerpFunc(float a, float b, float f);
void renderQuad();
void processInput(GLFWwindow* window);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void IconInit(GLFWwindow* window);
void LoadShaders();
void ConfigShaders();
void InitAO();
void InitModels();
void CheckRespolution();
void RenderAllModels(Shader *shader);
void SaveFBOToPNG(std::string fileName, bool includeAlpha);
void InitAnalysisDataLogger();
void RenderStatisticalFrames();
void ScalabilityTests();
void newFrameSetup(GLFWwindow* window);
void endFrameCleanup(GLFWwindow* window);
