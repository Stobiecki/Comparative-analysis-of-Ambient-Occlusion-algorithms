#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "SSAO.h"
#include "SSAOplus.h"
#include "HBAO.h"
#include "Shader.h"
#include "Model.h"
#include "AnalysisDataLogger.h"

enum ScreenResolution
{
	Res360x800 = 0,
	Res800x600 = 1,
	Res1440x810 = 2,
	Res1920x1080 = 3,
	Res2560x1440 = 4,
	Res3840x2160 = 5,
	ResLen = 6,
};

enum AmbientOcclusion
{
	SSAO_		= 0,
	SSAOplus_	= 1,
	HBAO_		= 2
};

class Light 
{
public:
	glm::vec3 Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Specular = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 Attenuation = glm::vec3(0.1f, 0.01f, 0.005f);
	glm::vec3 Position = glm::vec3(0.0f, 20.0f, 0.0f);
};

class Material
{
public:
	glm::vec3 Ambient = glm::vec3(1.000000f, 1.000000f, 1.000000f);
	glm::vec3 Diffuse = glm::vec3(1.000000f, 1.000000f, 1.000000f);
	glm::vec3 Specular = glm::vec3(1.000000f, 1.000000f, 1.000000f);
	float Shininess = 40.0f;
};

class RenderData
{
public:
	// models related
	static class ModelList	modelsList;
	static bool				invertedNormals;

	// shaders realted
	// General use
	static Shader			shaderNotRedAO;
	// SSAO
	static Shader			shaderGeometryPassSSAO;
	static Shader			shaderLightingPassSSAO;
	static Shader			shaderSSAO;
	static Shader			shaderSSAOBlur;
	static bool				SSAOBlur;
	static bool				OnlySSAO;
	// SSAOplus+ 
	static Shader			shaderGeometryPassSSAOplus;
	static Shader			shaderLightingPassSSAOplus;
	static Shader			shaderSSAOplus;
	static Shader			shaderSSAOplusBlur;
	static bool				SSAOplusBlur;
	static bool				OnlySSAOplus;
	// HBAO
	static Shader			shaderGeometryPassHBAO;
	static Shader			shaderDepthHBAO;
	static Shader			shaderCalculateHBAO;
	static Shader			shaderBlurHBAO;
	static Shader			shaderBlur2HBAO;
	static bool				HBAOBlur;
	static bool				OnlyHBAO;

	// time related
	static float			deltaTime;
	static float			lastFrame;

	// window related 
	static GLFWwindow*		window;
	static bool				mouseMode;
	static int				screenWidth;
	static int				screenHeight;
	static int				currentScreenResolution;

	// GUI related
	static float			GUIWidth;
	static int				chosenAmbientOcclusion;
	static bool				displayTexture;
	static bool				makePrintscreen;
	static bool				PrintscreenMode;
	static std::string		PrintscreenName;

	// camera related
	static class Camera		myCamera;
	static float			nearP;
	static float			farP;
	static float			lastX;
	static float			lastY;
	static glm::mat4		projection;
	static glm::mat4		view;

	// light related
	static Light			light;
	static Material			material;

	// AO realted
	// resolutions
	// 1440x810 - standard
	static class SSAO		ssao;
	static class SSAOplus	ssaoPlus;
	static class HBAO		hbao;

	// 800x600
	static class SSAO		ssao_800x600;
	static class SSAOplus	ssaoPlus_800x600;
	static class HBAO		hbao_800x600;

	// 1920x1080 (full HD)
	static class SSAO		ssao_1920x1080;
	static class SSAOplus	ssaoPlus_1920x1080;
	static class HBAO		hbao_1920x1080;

	// 2560x1440 (QHD)
	static class SSAO		ssao_2560x1440;
	static class SSAOplus	ssaoPlus_2560x1440;
	static class HBAO		hbao_2560x1440;

	// 3840x2160 (4K)
	static class SSAO		ssao_3840x2160;
	static class SSAOplus	ssaoPlus_3840x2160;
	static class HBAO		hbao_3840x2160;

	// 360x800 (common mobile screen resolution)
	static class SSAO		ssao_360x800;
	static class SSAOplus	ssaoPlus_360x800;
	static class HBAO		hbao_360x800;

	// current AO
	static class SSAO		*CurrentSsao;
	static class SSAOplus	*CurrentSsaoPlus;
	static class HBAO		*CurrentHbao;

	// Analysis related
	static AnalysisDataLogger logger;
	static bool enableLogging;
	static bool enableScalabilityLogging;
	static std::vector<std::string> ScalabilityLoggingNames;
};