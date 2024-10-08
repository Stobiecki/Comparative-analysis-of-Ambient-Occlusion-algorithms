#pragma once
#pragma once
#include <glad/glad.h> 

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "UtilityFunctions.h"

class HBAO
{
public:
	GLuint gBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;

// textures ///////////

	GLuint sceneColorTexture;
	GLuint sceneDepthStencilTexture;
	GLuint sceneDepthlinear;

	GLuint randomTextureHBAO;
	GLuint randomViewTextureHBAO;

	GLuint resultTextureHBAO;
	GLuint blurTextureHBAO;

// FBO ////////////////

	GLuint sceneFBO;
	GLuint depthLinearFBO;
	GLuint calculationFBO;

///////////////////////

	const int randomElementsHBAO = 16;
	const int maxSamples = 8;
	const int randomSizeHBAO = 4;

	glm::vec4 hbaoRandom[128];

// data ///////////////

	float intensity = 1.5f;
	float bias = 0.1f;
	float radius = 2.0f;
	float blurSharpness = 40.0f;

// shader data ////////

	glm::vec4 projInfo;

	float radius2;
	float NegInvRadius2;
	float RadiusToScreen;

	float powExponent;
	float nDotVBias;
	float aoMultiplier;

	glm::vec2 InvQuarterResolution;
	glm::vec2 InvFullResolution;

	glm::vec4 float2Offsets[4*4];
	glm::vec4 jitters[4*4];

///////////////////////
	HBAO();
	void Init(int _screenWidth, int _screenHeight);
	void GeometryPass();
	void LightingPass();
	void PrepareDataHBAO();
	void DrawLinearDepth();
	void AOCalculation();
	void DrawBlur();
	void Clearing();

	void OnlyAO();

private:
	int screenWidth;
	int screenHeight;

	void _InitRandom();
	void _InitHBAOBuffer();
};