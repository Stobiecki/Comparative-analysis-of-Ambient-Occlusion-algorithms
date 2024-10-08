#pragma once
#include <glad/glad.h> 

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "UtilityFunctions.h"

class SSAO
{
public:
	GLuint gBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;
	GLuint gMatMP;
	GLuint gNormalForLight;

	GLuint RBODepth;

	GLuint ssaoFBO;
	GLuint ssaoBlurFBO;

	GLuint ssaoColorBuffer;
	GLuint ssaoColorBufferBlur;

	GLuint noiseTexture;

	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	int kernelSize = 64;
	float radius = 0.5f;
	float bias = 0.025f;
	float noiseSize = 4.0f;
	float aoAmplification = 1.0f;

	SSAO();
	void Init(int screenWidth, int screenHeight);
	void GeometryPass();
	void RenderToTexture();
	void BlurSSAO();
	void LightiningPass();
	void OnlyAO();

private:
	int screenWidth;
	int screenHeight;

	void _InitG_Buffer();
	void _InitSSAOBuffer();
	void _GenerateKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
	void _GenerateNoise(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
};