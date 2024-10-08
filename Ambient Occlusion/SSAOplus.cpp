#include "SSAOplus.h"

SSAOplus::SSAOplus(){}

void SSAOplus::Init(int _screenWidth, int _screenHeight)
{
    screenWidth = _screenWidth;
    screenHeight = _screenHeight;

    _InitG_Buffer();
    _InitSSAOBuffer();

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    _GenerateKernel(randomFloats, generator);
    _GenerateNoise(randomFloats, generator);
}

void SSAOplus::GeometryPass()
{
    // 1. geometry pass: render scene's geometry/color data into gbuffer
    // -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderData::projection = glm::perspective(glm::radians(RenderData::myCamera.Zoom), (float)screenWidth / (float)screenHeight, RenderData::nearP, RenderData::farP);
    RenderData::view = RenderData::myCamera.GetViewMatrix();
    
    RenderData::shaderGeometryPassSSAOplus.use();
    RenderData::shaderGeometryPassSSAOplus.setMat4("projection", RenderData::projection);
    RenderData::shaderGeometryPassSSAOplus.setMat4("view", RenderData::view);
}
void SSAOplus::RenderToTexture()
{
    // 2. generate SSAOplus texture
    // ------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    RenderData::shaderSSAOplus.use();
    glUniform3fv(glGetUniformLocation(RenderData::shaderSSAOplus.idProgram, "samples"), 64, glm::value_ptr(ssaoKernel[0]));

    RenderData::shaderSSAOplus.setInt("kernelSize", kernelSize);
    RenderData::shaderSSAOplus.setFloat("radius", radius);
    RenderData::shaderSSAOplus.setFloat("bias", bias);
    RenderData::shaderSSAOplus.setFloat("noiseSize", noiseSize);
    RenderData::shaderSSAOplus.setFloat("aoAmplification", aoAmplification);

    RenderData::shaderSSAOplus.setFloat("windowWidth", screenWidth);
    RenderData::shaderSSAOplus.setFloat("windowHight", screenHeight);

    RenderData::shaderSSAOplus.setMat4("projection", RenderData::projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    RenderData::shaderSSAOplus.setInt("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    RenderData::shaderSSAOplus.setInt("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    RenderData::shaderSSAOplus.setInt("texNoise", 2);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SSAOplus::BlurSSAO()
{
    // 3. blur SSAOplus texture to remove noise
    // ------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    RenderData::shaderSSAOplusBlur.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SSAOplus::LightiningPass()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderData::shaderLightingPassSSAOplus.use();
    // send light relevant uniforms
            //RenderData::shaderLightingPassSSAO.setVec3("light.Position", RenderData::light2.lightPosView);
            //RenderData::shaderLightingPassSSAO.setVec3("light.Color", RenderData::light2.color);
            //const float linear = 0.09f;
            //const float quadratic = 0.032f;
            //RenderData::shaderLightingPassSSAO.setFloat("light.Coeff", RenderData::light2.coeff);
            //RenderData::shaderLightingPassSSAO.setFloat("light.Linear", RenderData::light2.linear);
            //RenderData::shaderLightingPassSSAO.setFloat("light.Quadratic", RenderData::light2.quadratic);


    RenderData::shaderLightingPassSSAOplus.setVec3("light.Ambient", RenderData::light.Ambient);
    RenderData::shaderLightingPassSSAOplus.setVec3("light.Diffuse", RenderData::light.Diffuse);
    RenderData::shaderLightingPassSSAOplus.setVec3("light.Specular", RenderData::light.Specular);
    RenderData::shaderLightingPassSSAOplus.setVec3("light.Attenuation", RenderData::light.Attenuation);
    RenderData::shaderLightingPassSSAOplus.setVec3("light.Position", RenderData::light.Position);

    RenderData::shaderLightingPassSSAOplus.setVec3("material.Ambient", RenderData::material.Ambient);
    RenderData::shaderLightingPassSSAOplus.setVec3("material.Diffuse", RenderData::material.Diffuse);
    RenderData::shaderLightingPassSSAOplus.setVec3("material.Specular", RenderData::material.Specular);
    RenderData::shaderLightingPassSSAOplus.setFloat("material.Shininess", RenderData::material.Shininess);

    RenderData::shaderLightingPassSSAOplus.setVec3("cameraPos", RenderData::myCamera.Position);
    //cout << "\nRenderData::myCamera.Position: " 
    //    << RenderData::myCamera.Position.x << " "
    //    << RenderData::myCamera.Position.y << " "
    //    << RenderData::myCamera.Position.z;


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    RenderData::shaderLightingPassSSAOplus.setInt("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    RenderData::shaderLightingPassSSAOplus.setInt("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    RenderData::shaderLightingPassSSAOplus.setInt("gAlbedo", 2);
    glActiveTexture(GL_TEXTURE3); 
    glBindTexture(GL_TEXTURE_2D, gMatMP);
    RenderData::shaderLightingPassSSAOplus.setInt("gMatMP", 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gNormalForLight);
    RenderData::shaderLightingPassSSAOplus.setInt("gNormalForLight", 4);
    
    glActiveTexture(GL_TEXTURE5);// add extra SSAOplus texture to lighting pass
    if(RenderData::SSAOplusBlur)
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    else
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    RenderData::shaderLightingPassSSAOplus.setInt("ssao", 5);
    renderQuad();
}

void SSAOplus::OnlyAO()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderData::shaderNotRedAO.use();
    glActiveTexture(GL_TEXTURE0);
    if (RenderData::SSAOplusBlur)
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    else
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    renderQuad();
}

void SSAOplus::_InitG_Buffer()
{
    //std::cout << "Init G buffer\n";
	// generating and binding gBuffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // position color buffer (not binding)
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal color buffer (not binding)
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // color + specular color buffer (not binding)
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    // modelMat * position, color buffer (not binding)
    glGenTextures(1, &gMatMP);
    glBindTexture(GL_TEXTURE_2D, gMatMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMatMP, 0);

    // normalMatrix * transpose(inverse(mat3(model))), color buffer (not binding)
    glGenTextures(1, &gNormalForLight);
    glBindTexture(GL_TEXTURE_2D, gNormalForLight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gNormalForLight, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, attachments);

    // RBO - Render Buffer Object
    glGenRenderbuffers(1, &RBODepth);
    glBindRenderbuffer(GL_RENDERBUFFER, RBODepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBODepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    // unbinding FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOplus::_InitSSAOBuffer()
{
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    // SSAOplus color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAOplus Framebuffer not complete!" << std::endl;

    // blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAOplus Blur Framebuffer not complete!" << std::endl;

    // unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOplus::_GenerateKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator)
{
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerpFunc(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

void SSAOplus::_GenerateNoise(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator)
{
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}