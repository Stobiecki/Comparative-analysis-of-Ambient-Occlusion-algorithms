//#pragma once
#pragma warning ( error : 4430 )

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define NOMINMAX

//#include <GLEW/GL/glew.h>
//#include <GLFW/glfw3.h>

#include <glad.h>
#include <GLFW/glfw3.h>

//#define GLEW_STATIC
//#define GLEW_BUILD 
//#include <GLEW/GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

//#include "stb_image.h"

#include <iostream>
#include <windows.h>

#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "SSAOplus.h"
#include "Camera.h"
#include "RenderData.h"
#include "UtilityFunctions.h"
#include "GUI.h"
#include "SmartTimer.h"


#define __CHECK_FOR_ERRORS 	{GLenum errCode; if ((errCode = glGetError()) != GL_NO_ERROR) printf("Error (%d): %s in file %s at line %d !\n", errCode, glad_glGetString(errCode), __FILE__,  __LINE__);}


//const GLubyte* vendor = glGetString(GL_VENDOR); // returns the Vendor of Card

// if we want to run it on GPU not on CPU
#define GPU_USAGE_NVIDIA
//#define GPU_USAGE_AMD
#ifdef GPU_USAGE_NVIDIA
    extern "C" _declspec(dllexport) DWORD NvOptimusEnablement = true;
#elif defined (GPU_USAGE_AMD)
    extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif // GPU_USAGE

void newFrameSetup(GLFWwindow* window);
void endFrameCleanup(GLFWwindow* window);
void renderCube();

using std::string;
using std::cout;
using std::endl;

int main()
{
    GLFWwindow* window = RenderData::window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(RenderData::screenWidth, RenderData::screenHeight, "Ambient Occlusions", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    RenderData::window = window;

    // window setup
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Shaders Init
    LoadShaders();

    // AO init
    InitAO();

    // IconInit
    IconInit(window);

    // Display GPU info
    printf("Vendor graphic card: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version GL: %s\n", glGetString(GL_VERSION));
    printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // ImGui Implementation
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = false;

    // ImGui Setup
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // ImGui Dockspace Init
    GUI::DockspaceInit();

    // Modles Init
    InitModels();

    // shader configuration
    ConfigShaders();

    // Init Logger
    InitAnalysisDataLogger();

    __CHECK_FOR_ERRORS
    while (!glfwWindowShouldClose(window))
    {
        CheckRespolution();

        if (RenderData::enableLogging)
            RenderStatisticalFrames();

        if (RenderData::enableScalabilityLogging)
            ScalabilityTests();

        newFrameSetup(window);
        __CHECK_FOR_ERRORS
        // SSAO
        if (RenderData::chosenAmbientOcclusion == SSAO_)
        {
            SmartTimer("FullTime_SSAO.txt");

            RenderData::CurrentSsao->GeometryPass();
            //render all models
            RenderAllModels(&RenderData::shaderGeometryPassSSAO);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            RenderData::CurrentSsao->RenderToTexture();
            RenderData::CurrentSsao->BlurSSAO();
            RenderData::CurrentSsao->LightiningPass();
            if (RenderData::OnlySSAO)
                RenderData::CurrentSsao->OnlyAO();
        }
        // SSAO+
        else if (RenderData::chosenAmbientOcclusion == SSAOplus_)
        {
            SmartTimer("FullTime_SSAO+.txt");

            RenderData::CurrentSsaoPlus->GeometryPass();
            //render all models
            RenderAllModels(&RenderData::shaderGeometryPassSSAOplus);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            RenderData::CurrentSsaoPlus->RenderToTexture();
            RenderData::CurrentSsaoPlus->BlurSSAO();
            RenderData::CurrentSsaoPlus->LightiningPass();
            if (RenderData::OnlySSAOplus)
                RenderData::CurrentSsaoPlus->OnlyAO();
        }
        else if (RenderData::chosenAmbientOcclusion == HBAO_)
        {
            SmartTimer("FullTime_HBAO.txt");

            RenderData::CurrentHbao->GeometryPass();
            RenderData::CurrentHbao->LightingPass();
            //render all models
            RenderAllModels(&RenderData::shaderGeometryPassHBAO);
            RenderData::CurrentHbao->PrepareDataHBAO();
            //glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
            //         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "draw linear depth");
            RenderData::CurrentHbao->DrawLinearDepth();
            //glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
            //         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "AO Calculation");
            RenderData::CurrentHbao->AOCalculation();
            if(RenderData::HBAOBlur)
                RenderData::CurrentHbao->DrawBlur();
            RenderData::CurrentHbao->Clearing();
            if(RenderData::OnlyHBAO)
                RenderData::CurrentHbao->OnlyAO();
        }

        if (RenderData::mouseMode)
            GUI::Draw();
        if (RenderData::makePrintscreen)
            SaveFBOToPNG(RenderData::PrintscreenName, false);

        endFrameCleanup(window);
        __CHECK_FOR_ERRORS
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}