#include "RenderData.h"

ModelList RenderData::modelsList;
bool RenderData::invertedNormals = false;

Shader RenderData::shaderNotRedAO;

Shader RenderData::shaderGeometryPassSSAO;
Shader RenderData::shaderLightingPassSSAO;
Shader RenderData::shaderSSAO;
Shader RenderData::shaderSSAOBlur;
bool RenderData::SSAOBlur = true;
bool RenderData::OnlySSAO = false;

Shader RenderData::shaderGeometryPassSSAOplus;
Shader RenderData::shaderLightingPassSSAOplus;
Shader RenderData::shaderSSAOplus;
Shader RenderData::shaderSSAOplusBlur;
bool RenderData::SSAOplusBlur = true;
bool RenderData::OnlySSAOplus = false;

Shader RenderData::shaderGeometryPassHBAO;
Shader RenderData::shaderDepthHBAO;
Shader RenderData::shaderCalculateHBAO;
Shader RenderData::shaderBlurHBAO;
Shader RenderData::shaderBlur2HBAO;
bool RenderData::HBAOBlur = true;
bool RenderData::OnlyHBAO = false;

float RenderData::deltaTime = 0.0f;
float RenderData::lastFrame = 0.0f;

GLFWwindow* RenderData::window;
bool RenderData::mouseMode = 0;
int RenderData::screenWidth = 1920*3/4;
int RenderData::screenHeight = 1080*3/4;
int RenderData::currentScreenResolution = Res1440x810;

float RenderData::GUIWidth = 330;
int RenderData::chosenAmbientOcclusion = HBAO_;
bool RenderData::displayTexture = false;
bool RenderData::makePrintscreen = false;
bool RenderData::PrintscreenMode = false;
std::string RenderData::PrintscreenName = "screen.png";

Camera RenderData::myCamera = Camera(glm::vec3(-30.0f, 10.0f, -2.0f));
float RenderData::nearP = 0.1f;
float RenderData::farP = 500.f;
float RenderData::lastX = (float)screenWidth / 2.0;
float RenderData::lastY = (float)screenHeight / 2.0;
glm::mat4 RenderData::projection;
glm::mat4 RenderData::view;

Light RenderData::light;
Material RenderData::material;

SSAO RenderData::ssao;
SSAOplus RenderData::ssaoPlus;
HBAO RenderData::hbao;

SSAO RenderData::ssao_800x600;
SSAOplus RenderData::ssaoPlus_800x600;
HBAO RenderData::hbao_800x600;

SSAO RenderData::ssao_1920x1080;
SSAOplus RenderData::ssaoPlus_1920x1080;
HBAO RenderData::hbao_1920x1080;

SSAO RenderData::ssao_2560x1440;
SSAOplus RenderData::ssaoPlus_2560x1440;
HBAO RenderData::hbao_2560x1440;

SSAO RenderData::ssao_3840x2160;
SSAOplus RenderData::ssaoPlus_3840x2160;
HBAO RenderData::hbao_3840x2160;

SSAO RenderData::ssao_360x800;
SSAOplus RenderData::ssaoPlus_360x800;
HBAO RenderData::hbao_360x800;

SSAO* RenderData::CurrentSsao = &RenderData::ssao;
SSAOplus* RenderData::CurrentSsaoPlus = &RenderData::ssaoPlus;
HBAO* RenderData::CurrentHbao = &RenderData::hbao;

AnalysisDataLogger RenderData::logger;
bool RenderData::enableLogging = false;
bool RenderData::enableScalabilityLogging = false;
std::vector<std::string> RenderData::ScalabilityLoggingNames = { "Res360x800_", "Res800x600_", "Res1440x810_", "Res1920x1080_", "Res2560x1440_", "Res3840x2160_"};