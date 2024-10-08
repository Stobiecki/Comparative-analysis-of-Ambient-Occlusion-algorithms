#include "UtilityFunctions.h"

float lerpFunc(float a, float b, float f)
{
    return a + f * (b - a);
}

void renderQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0)
    {
        static float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (RenderData::mouseMode == false)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            RenderData::myCamera.ProcessKeyboard(FRONT, RenderData::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            RenderData::myCamera.ProcessKeyboard(BACK, RenderData::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            RenderData::myCamera.ProcessKeyboard(LEFT, RenderData::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            RenderData::myCamera.ProcessKeyboard(RIGHT, RenderData::deltaTime);
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        std::cout << "\nmenu toggled\n";
        RenderData::mouseMode = !RenderData::mouseMode;
        if (RenderData::mouseMode)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (key == GLFW_KEY_P && action == GLFW_RELEASE && RenderData::PrintscreenMode==true)
    {
        std::cout << "\np pressed, screenshot will be taken soon!\n" ;
        RenderData::makePrintscreen = true;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    cout << "\nwidth: " << width << ", height: " << height;
    glViewport(0, 0, width, height);

    RenderData::screenWidth = width;
    RenderData::screenHeight = height;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    static bool firstMouse = true;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        RenderData::lastX = xpos;
        RenderData::lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - RenderData::lastX;
    float yoffset = RenderData::lastY - ypos; // reversed since y-coordinates go from bottom to top

    RenderData::lastX = xpos;
    RenderData::lastY = ypos;
    if(!RenderData::mouseMode)
        RenderData::myCamera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!RenderData::mouseMode)
        RenderData::myCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void IconInit(GLFWwindow* window)
{
    //  lading icon for upper left corner
    int mini_icon_width_mini, mini_icon_height_mini;
    int mini_icon_channels_mini;
    // loading taskbar icon
    int mini_icon_width, mini_icon_height;
    int mini_icon_channels;

    unsigned char* mini_icon_pixels_mini = stbi_load("src/misc/icon16x16.png", &mini_icon_width_mini, &mini_icon_height_mini, &mini_icon_channels_mini, 4);
    unsigned char* mini_icon_pixels = stbi_load("src/misc/icon128x128.png", &mini_icon_width, &mini_icon_height, &mini_icon_channels, 4);

    // no small icon available
    if (mini_icon_pixels_mini == NULL)
        throw std::runtime_error("Coudln't load small icon");

    // no icon available
    if (mini_icon_pixels == NULL)
        throw std::runtime_error("Coudln't load icon");

    GLFWimage mini_icon_image[2];
    mini_icon_image[0].width = mini_icon_width_mini;
    mini_icon_image[0].height = mini_icon_height_mini;
    mini_icon_image[0].pixels = mini_icon_pixels_mini;

    mini_icon_image[1].width = mini_icon_width;
    mini_icon_image[1].height = mini_icon_height;
    mini_icon_image[1].pixels = mini_icon_pixels;

    glfwSetWindowIcon(window, 2, mini_icon_image);

    stbi_image_free(mini_icon_pixels_mini);
    stbi_image_free(mini_icon_pixels);
}

void LoadShaders()
{
    // General use
    RenderData::shaderNotRedAO.Init("shaders/general/renderAO.vs", "shaders/general/renderAO.fs");

    // SSAO
    RenderData::shaderGeometryPassSSAO.Init("shaders/SSAO/ssao_geometry.vs", "shaders/SSAO/ssao_geometry.fs");
    RenderData::shaderLightingPassSSAO.Init("shaders/SSAO/ssao.vs", "shaders/SSAO/ssao_lighting.fs");
    RenderData::shaderSSAO.Init("shaders/SSAO/ssao.vs", "shaders/SSAO/ssao.fs");
    RenderData::shaderSSAOBlur.Init("shaders/SSAO/ssao.vs", "shaders/SSAO/ssao_blur.fs");

    // SSAO+
    RenderData::shaderGeometryPassSSAOplus.Init("shaders/SSAO+/ssao_geometry.vs", "shaders/SSAO+/ssao_geometry.fs");
    RenderData::shaderLightingPassSSAOplus.Init("shaders/SSAO+/ssao.vs", "shaders/SSAO+/ssao_lighting.fs");
    RenderData::shaderSSAOplus.Init("shaders/SSAO+/ssao.vs", "shaders/SSAO+/ssao.fs");
    RenderData::shaderSSAOplusBlur.Init("shaders/SSAO+/ssao.vs", "shaders/SSAO+/ssao_blur.fs");

    // HBAO
    RenderData::shaderGeometryPassHBAO.Init("shaders/HBAO/hbao_geometry.vs", "shaders/HBAO/hbao_geometry.fs");
    RenderData::shaderDepthHBAO.Init("shaders/HBAO/full_screen_quad.vs", "shaders/HBAO/depth_linearize.fs");
    RenderData::shaderCalculateHBAO.Init("shaders/HBAO/full_screen_quad.vs", "shaders/HBAO/hbao.fs");
    RenderData::shaderBlurHBAO.Init("shaders/HBAO/full_screen_quad.vs", "shaders/HBAO/hbao_blur.fs");
    RenderData::shaderBlur2HBAO.Init("shaders/HBAO/full_screen_quad.vs", "shaders/HBAO/hbao_blur.fs");
}

void ConfigShaders()
{
    // General use
    RenderData::shaderNotRedAO.use();
    RenderData::shaderNotRedAO.setInt("UV", 0);

    // SSAOplus
    RenderData::shaderLightingPassSSAO.use();
    RenderData::shaderLightingPassSSAO.setInt("gPosition", 0);
    RenderData::shaderLightingPassSSAO.setInt("gNormal", 1);
    RenderData::shaderLightingPassSSAO.setInt("gAlbedo", 2);
    RenderData::shaderLightingPassSSAO.setInt("ssao", 3);
    RenderData::shaderSSAO.use();
    RenderData::shaderSSAO.setInt("gPosition", 0);
    RenderData::shaderSSAO.setInt("gNormal", 1);
    RenderData::shaderSSAO.setInt("texNoise", 2);
    RenderData::shaderSSAOBlur.use();
    RenderData::shaderSSAOBlur.setInt("ssaoInput", 0);

    // SSAOplus+
    RenderData::shaderLightingPassSSAOplus.use();
    RenderData::shaderLightingPassSSAOplus.setInt("gPosition", 0);
    RenderData::shaderLightingPassSSAOplus.setInt("gNormal", 1);
    RenderData::shaderLightingPassSSAOplus.setInt("gAlbedo", 2);
    RenderData::shaderLightingPassSSAOplus.setInt("ssao", 3);
    RenderData::shaderSSAOplus.use();
    RenderData::shaderSSAOplus.setInt("gPosition", 0);
    RenderData::shaderSSAOplus.setInt("gNormal", 1);
    RenderData::shaderSSAOplus.setInt("texNoise", 2);
    RenderData::shaderSSAOplusBlur.use();
    RenderData::shaderSSAOplusBlur.setInt("ssaoInput", 0);
}

void InitAO()
{
    RenderData::ssao.Init(RenderData::screenWidth, RenderData::screenHeight);
    RenderData::ssaoPlus.Init(RenderData::screenWidth, RenderData::screenHeight);
    RenderData::hbao.Init(RenderData::screenWidth, RenderData::screenHeight);

    RenderData::ssao_800x600.Init(800, 600);
    RenderData::ssaoPlus_800x600.Init(800, 600);
    RenderData::hbao_800x600.Init(800, 600);

    RenderData::ssao_1920x1080.Init(1920, 1080);
    RenderData::ssaoPlus_1920x1080.Init(1920, 1080);
    RenderData::hbao_1920x1080.Init(1920, 1080);

    RenderData::ssao_2560x1440.Init(2560, 1440);
    RenderData::ssaoPlus_2560x1440.Init(2560, 1440);
    RenderData::hbao_2560x1440.Init(2560, 1440);

    RenderData::ssao_3840x2160.Init(3840, 2160);
    RenderData::ssaoPlus_3840x2160.Init(3840, 2160);
    RenderData::hbao_3840x2160.Init(3840, 2160);

    RenderData::ssao_360x800.Init(360, 800);
    RenderData::ssaoPlus_360x800.Init(360, 800);
    RenderData::hbao_360x800.Init(360, 800);
}

void InitModels()
{
    RenderData::modelsList.models.push_back(Model("src/obj/sponza.obj"));
    //RenderData::modelsList.models.push_back(Model("src/obj/Sponza.gltf"));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));

    RenderData::modelsList.matrix.push_back(model);
}

void CheckRespolution()
{
    static ScreenResolution resolutionInPastFrame = Res1440x810;

    if (RenderData::currentScreenResolution == resolutionInPastFrame)
        return;
    else
    {
        resolutionInPastFrame = (ScreenResolution)RenderData::currentScreenResolution;
        switch (RenderData::currentScreenResolution)
        {
        case Res360x800:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 360, 800);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_360x800;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_360x800;
            RenderData::CurrentHbao = &RenderData::hbao_360x800;
            break;
        case Res800x600:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 800, 600);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_800x600;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_800x600;
            RenderData::CurrentHbao = &RenderData::hbao_800x600;
            break;
        case Res1440x810:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1440, 810);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
            RenderData::CurrentHbao = &RenderData::hbao;
            break;
        case Res1920x1080:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1920, 1080);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_1920x1080;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_1920x1080;
            RenderData::CurrentHbao = &RenderData::hbao_1920x1080;
            break;
        case Res2560x1440:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 2560, 1440);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_2560x1440;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_2560x1440;
            RenderData::CurrentHbao = &RenderData::hbao_2560x1440;
            break;
        case Res3840x2160:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 3840, 2160);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_3840x2160;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_3840x2160;
            RenderData::CurrentHbao = &RenderData::hbao_3840x2160;
            break;
        default: //Res1440x810
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1440, 810);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
            RenderData::CurrentHbao = &RenderData::hbao;
            break;
        }
    }
}

void RenderAllModels(Shader* shader)
{
    for (int i=0, size=RenderData::modelsList.models.size(); i<size; i++)
    {
        shader->setMat4("model", RenderData::modelsList.matrix[i]);
        RenderData::modelsList.models[i].Draw(*shader);
        if(RenderData::invertedNormals)
            shader->setInt("invertedNormals", 1); // invert normals as we're inside the cube
        renderQuad();
        if (RenderData::invertedNormals)
            shader->setInt("invertedNormals", 0);
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SaveFBOToPNG(std::string fileName, bool includeAlpha)
{
    int channels = includeAlpha ? 4 : 3;
    RenderData::makePrintscreen = false;

    std::string name = "screenshots/" + fileName;
    int width = RenderData::screenWidth;
    int height = RenderData::screenHeight;
    GLubyte *pixels = new GLubyte[width * height * channels];

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glReadPixels(0, 0, width, height, (includeAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pixels);
    stbi_flip_vertically_on_write(1);

    stbi_write_png(name.c_str(), width, height, channels, pixels, width * channels);

    delete[] pixels;
}

void InitAnalysisDataLogger()
{
    RenderData::logger.LogDataLimitPerSet(1000);

    // time log
    RenderData::logger.AddFileToLogger("FullTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("FullTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("FullTime_HBAO.txt");
    RenderData::logger.ClearLogFile("FullTime_SSAO.txt");
    RenderData::logger.ClearLogFile("FullTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("FullTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("OnlyAOTime_HBAO.txt");

    //scalability log
    RenderData::logger.AddFileToLogger("Res360x800_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res360x800_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res360x800_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res360x800_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res360x800_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res360x800_OnlyAOTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("Res800x600_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res800x600_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res800x600_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res800x600_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res800x600_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res800x600_OnlyAOTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("Res1440x810_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res1440x810_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res1440x810_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res1440x810_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res1440x810_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res1440x810_OnlyAOTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("Res1920x1080_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res1920x1080_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res1920x1080_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res1920x1080_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res1920x1080_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res1920x1080_OnlyAOTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("Res2560x1440_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res2560x1440_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res2560x1440_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res2560x1440_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res2560x1440_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res2560x1440_OnlyAOTime_HBAO.txt");

    RenderData::logger.AddFileToLogger("Res3840x2160_OnlyAOTime_SSAO.txt");
    RenderData::logger.AddFileToLogger("Res3840x2160_OnlyAOTime_SSAO+.txt");
    RenderData::logger.AddFileToLogger("Res3840x2160_OnlyAOTime_HBAO.txt");
    RenderData::logger.ClearLogFile("Res3840x2160_OnlyAOTime_SSAO.txt");
    RenderData::logger.ClearLogFile("Res3840x2160_OnlyAOTime_SSAO+.txt");
    RenderData::logger.ClearLogFile("Res3840x2160_OnlyAOTime_HBAO.txt");
}

void RenderStatisticalFrames()
{
    while (RenderData::logger.CheckIterationLimit())
    {
        // FullTime_SSAO
        newFrameSetup(RenderData::window);
        {
            SmartTimer("FullTime_SSAO.txt");
            RenderData::CurrentSsao->GeometryPass();
            RenderAllModels(&RenderData::shaderGeometryPassSSAO);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            RenderData::CurrentSsao->RenderToTexture();
            RenderData::CurrentSsao->BlurSSAO();
            RenderData::CurrentSsao->LightiningPass();
            if (RenderData::OnlySSAO)
                RenderData::CurrentSsao->OnlyAO();
        }
        endFrameCleanup(RenderData::window);
    }
    while (RenderData::logger.CheckIterationLimit())
    {
        // FullTime_SSAO+
        newFrameSetup(RenderData::window);
        {
            SmartTimer("FullTime_SSAO+.txt");
            RenderData::CurrentSsaoPlus->GeometryPass();
            RenderAllModels(&RenderData::shaderGeometryPassSSAOplus);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            RenderData::CurrentSsaoPlus->RenderToTexture();
            RenderData::CurrentSsaoPlus->BlurSSAO();
            RenderData::CurrentSsaoPlus->LightiningPass();
            if (RenderData::OnlySSAOplus)
                RenderData::CurrentSsaoPlus->OnlyAO();
        }
        endFrameCleanup(RenderData::window);
    }
    while (RenderData::logger.CheckIterationLimit())
    {
        //FullTime_HBAO
        newFrameSetup(RenderData::window);
        {
            SmartTimer("FullTime_HBAO.txt");
            RenderData::CurrentHbao->GeometryPass();
            RenderData::CurrentHbao->LightingPass();
            RenderAllModels(&RenderData::shaderGeometryPassHBAO);
            RenderData::CurrentHbao->PrepareDataHBAO();
            RenderData::CurrentHbao->DrawLinearDepth();
            RenderData::CurrentHbao->AOCalculation();
            if (RenderData::HBAOBlur)
                RenderData::CurrentHbao->DrawBlur();
            RenderData::CurrentHbao->Clearing();
            if (RenderData::OnlyHBAO)
                RenderData::CurrentHbao->OnlyAO();
        }
        endFrameCleanup(RenderData::window);
    }
    while (RenderData::logger.CheckIterationLimit())
    {
        // OnlyAOTime_SSAO
        newFrameSetup(RenderData::window);
        RenderData::CurrentSsao->GeometryPass();
        RenderAllModels(&RenderData::shaderGeometryPassSSAO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        {
            SmartTimer("OnlyAOTime_SSAO.txt");
            RenderData::CurrentSsao->RenderToTexture();
        }
        RenderData::CurrentSsao->BlurSSAO();
        RenderData::CurrentSsao->LightiningPass();
        if (RenderData::OnlySSAO)
            RenderData::CurrentSsao->OnlyAO();
        endFrameCleanup(RenderData::window);
    }
    while (RenderData::logger.CheckIterationLimit())
    {
        // OnlyAOTime_SSAO+
        newFrameSetup(RenderData::window);
        RenderData::CurrentSsaoPlus->GeometryPass();
        RenderAllModels(&RenderData::shaderGeometryPassSSAOplus);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        {
            SmartTimer("OnlyAOTime_SSAO+.txt");
            RenderData::CurrentSsaoPlus->RenderToTexture();
        }
        RenderData::CurrentSsaoPlus->BlurSSAO();
        RenderData::CurrentSsaoPlus->LightiningPass();
        if (RenderData::OnlySSAOplus)
            RenderData::CurrentSsaoPlus->OnlyAO();
        endFrameCleanup(RenderData::window);
    }
    while (RenderData::logger.CheckIterationLimit())
    {
        //OnlyAOTime_HBAO
        newFrameSetup(RenderData::window);
        RenderData::CurrentHbao->GeometryPass();
        RenderData::CurrentHbao->LightingPass();
        RenderAllModels(&RenderData::shaderGeometryPassHBAO);
        RenderData::CurrentHbao->PrepareDataHBAO();
        RenderData::CurrentHbao->DrawLinearDepth();
        {
            SmartTimer("OnlyAOTime_HBAO.txt");
            RenderData::CurrentHbao->AOCalculation();
        }
        if (RenderData::HBAOBlur)
            RenderData::CurrentHbao->DrawBlur();
        RenderData::CurrentHbao->Clearing();
        if (RenderData::OnlyHBAO)
            RenderData::CurrentHbao->OnlyAO();
        endFrameCleanup(RenderData::window);
    }

    RenderData::logger.DisplayStatistics();

    RenderData::enableLogging = false;
}

void ScalabilityTests()
{
    RenderData::enableLogging = true;
    for (int i = 0; i < ScreenResolution::ResLen; i++)
    {
        switch (i)
        {
        case Res360x800:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 360, 800);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_360x800;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_360x800;
            RenderData::CurrentHbao = &RenderData::hbao_360x800;
            break;
        case Res800x600:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 800, 600);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_800x600;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_800x600;
            RenderData::CurrentHbao = &RenderData::hbao_800x600;
            break;
        case Res1440x810:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1440, 810);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
            RenderData::CurrentHbao = &RenderData::hbao;
            break;
        case Res1920x1080:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1920, 1080);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_1920x1080;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_1920x1080;
            RenderData::CurrentHbao = &RenderData::hbao_1920x1080;
            break;
        case Res2560x1440:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 2560, 1440);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_2560x1440;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_2560x1440;
            RenderData::CurrentHbao = &RenderData::hbao_2560x1440;
            break;
        case Res3840x2160:
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 3840, 2160);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao_3840x2160;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus_3840x2160;
            RenderData::CurrentHbao = &RenderData::hbao_3840x2160;
            break;
        default: //Res1440x810
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
            glfwSetWindowSize(RenderData::window, 1440, 810);
            glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
            RenderData::CurrentSsao = &RenderData::ssao;
            RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
            RenderData::CurrentHbao = &RenderData::hbao;
            break;
        }
        while (RenderData::logger.CheckIterationLimit())
        {
            // OnlyAOTime_SSAO
            newFrameSetup(RenderData::window);
            RenderData::CurrentSsao->GeometryPass();
            RenderAllModels(&RenderData::shaderGeometryPassSSAO);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            {
                SmartTimer(RenderData::ScalabilityLoggingNames[i] + "OnlyAOTime_SSAO.txt");
                RenderData::CurrentSsao->RenderToTexture();
            }
            RenderData::CurrentSsao->BlurSSAO();
            RenderData::CurrentSsao->LightiningPass();
            if (RenderData::OnlySSAO)
                RenderData::CurrentSsao->OnlyAO();
            endFrameCleanup(RenderData::window);
        }
        while (RenderData::logger.CheckIterationLimit())
        {
            // OnlyAOTime_SSAO+
            newFrameSetup(RenderData::window);
            RenderData::CurrentSsaoPlus->GeometryPass();
            RenderAllModels(&RenderData::shaderGeometryPassSSAOplus);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            {
                SmartTimer(RenderData::ScalabilityLoggingNames[i] + "OnlyAOTime_SSAO+.txt");
                RenderData::CurrentSsaoPlus->RenderToTexture();
            }
            RenderData::CurrentSsaoPlus->BlurSSAO();
            RenderData::CurrentSsaoPlus->LightiningPass();
            if (RenderData::OnlySSAOplus)
                RenderData::CurrentSsaoPlus->OnlyAO();
            endFrameCleanup(RenderData::window);
        }
        while (RenderData::logger.CheckIterationLimit())
        {
            //OnlyAOTime_HBAO
            newFrameSetup(RenderData::window);
            RenderData::CurrentHbao->GeometryPass();
            RenderData::CurrentHbao->LightingPass();
            RenderAllModels(&RenderData::shaderGeometryPassHBAO);
            RenderData::CurrentHbao->PrepareDataHBAO();
            RenderData::CurrentHbao->DrawLinearDepth();
            {
                SmartTimer(RenderData::ScalabilityLoggingNames[i] + "OnlyAOTime_HBAO.txt");
                RenderData::CurrentHbao->AOCalculation();
            }
            if (RenderData::HBAOBlur)
                RenderData::CurrentHbao->DrawBlur();
            RenderData::CurrentHbao->Clearing();
            if (RenderData::OnlyHBAO)
                RenderData::CurrentHbao->OnlyAO();
            endFrameCleanup(RenderData::window);
        }
    }

    RenderData::logger.DisplayStatistics();
    
    glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, true);
    glfwSetWindowSize(RenderData::window, 1440, 810);
    glfwSetWindowAttrib(RenderData::window, GLFW_RESIZABLE, false);
    RenderData::CurrentSsao = &RenderData::ssao;
    RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
    RenderData::CurrentHbao = &RenderData::hbao;

    RenderData::currentScreenResolution = Res1440x810;
    RenderData::enableScalabilityLogging = false;
    RenderData::enableLogging = false;
}

void newFrameSetup(GLFWwindow* window)
{
    float currentFrame = static_cast<float>(glfwGetTime());
    RenderData::deltaTime = currentFrame - RenderData::lastFrame;
    RenderData::lastFrame = currentFrame;

    processInput(window);

    /* Render here */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* New frame for ImGui */
    if (!RenderData::enableScalabilityLogging)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}

void endFrameCleanup(GLFWwindow* window)
{
    if (!RenderData::enableScalabilityLogging)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}