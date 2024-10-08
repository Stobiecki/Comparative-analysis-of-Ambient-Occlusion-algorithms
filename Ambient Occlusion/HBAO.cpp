#include "HBAO.h"

#define __CHECK_FOR_ERRORS 	{GLenum errCode; if ((errCode = glGetError()) != GL_NO_ERROR) printf("Error (%d): %s in file %s at line %d !\n", errCode, glad_glGetString(errCode), __FILE__,  __LINE__);}


HBAO::HBAO() {}

void HBAO::Init(int _screenWidth, int _screenHeight)
{
    screenWidth = _screenWidth;
    screenHeight = _screenHeight;

    _InitRandom();
    _InitHBAOBuffer();
}

void HBAO::GeometryPass()
{
    __CHECK_FOR_ERRORS
    // 1. geometry pass: render scene's geometry/color data into gbuffer
    // -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "geometry");
    RenderData::projection = glm::perspective(glm::radians(RenderData::myCamera.Zoom), (float)screenWidth / (float)screenHeight, RenderData::nearP, RenderData::farP);
    RenderData::view = RenderData::myCamera.GetViewMatrix();

    RenderData::shaderGeometryPassHBAO.use();
    RenderData::shaderGeometryPassHBAO.setMat4("projection", RenderData::projection);
    RenderData::shaderGeometryPassHBAO.setMat4("view", RenderData::view);
}
void HBAO::LightingPass()
{
    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "light");
    //glm::vec3 lightPosView = glm::vec3(RenderData::myCamera.GetViewMatrix() * glm::vec4(glm::vec3(20.0, 50.0, -2.0), 1.0));
    const float linear = 0.09f;
    const float quadratic = 0.032f;
    //const float linear = 9.f;
    //const float quadratic = 32.f;

    RenderData::shaderGeometryPassHBAO.setVec3("light.Ambient", RenderData::light.Ambient);
    RenderData::shaderGeometryPassHBAO.setVec3("light.Diffuse", RenderData::light.Diffuse);
    RenderData::shaderGeometryPassHBAO.setVec3("light.Specular", RenderData::light.Specular);
    RenderData::shaderGeometryPassHBAO.setVec3("light.Attenuation", RenderData::light.Attenuation);
    RenderData::shaderGeometryPassHBAO.setVec3("light.Position", RenderData::light.Position);


    RenderData::shaderGeometryPassHBAO.setVec3("material.Ambient", RenderData::material.Ambient);
    RenderData::shaderGeometryPassHBAO.setVec3("material.Diffuse", RenderData::material.Diffuse);
    RenderData::shaderGeometryPassHBAO.setVec3("material.Specular", RenderData::material.Specular);
    RenderData::shaderGeometryPassHBAO.setFloat("material.Shininess", RenderData::material.Shininess);

    RenderData::shaderGeometryPassHBAO.setVec3("cameraPos", RenderData::myCamera.Position);
    __CHECK_FOR_ERRORS
    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "light");
}
void HBAO::PrepareDataHBAO()
{
    // 2. prepare data for HBAO
    // ------------------------
    __CHECK_FOR_ERRORS
    const float* P = glm::value_ptr(RenderData::projection);

    float projInfoPerspective[] = {
        2.0f / (P[4 * 0 + 0]),                  // (x) * (R - L)/N
        2.0f / (P[4 * 1 + 1]),                  // (y) * (T - B)/N
        -(1.0f - P[4 * 2 + 0]) / P[4 * 0 + 0],  // L/N
        -(1.0f + P[4 * 2 + 1]) / P[4 * 1 + 1],  // B/N
    };

    projInfo = glm::make_vec4(projInfoPerspective);

    float meters2viewspace = 1.0f;
    float R = radius * meters2viewspace;
    float projScale = float(screenHeight) / (tanf(RenderData::myCamera.Zoom * 0.5f) * 2.0f); // mo¿e byæ Ÿle

    // radius
    radius2 = R * R;
    NegInvRadius2 = -1.0f / radius2;
    RadiusToScreen = R * 0.5f * projScale;

    // ao
    powExponent = std::max(intensity, 0.0f);
    nDotVBias = std::min(std::max(0.0f, bias), 1.0f);
    aoMultiplier = 1.0f / (1.0f - nDotVBias);

    // resolution
    int quarterWidth = ((screenWidth + 3) / 4);
    int quarterHeight = ((screenHeight + 3) / 4);

    InvQuarterResolution = glm::vec2(1.0f / float(quarterWidth), 1.0f / float(quarterHeight));
    InvFullResolution = glm::vec2(1.0f / float(screenWidth), 1.0f / float(screenHeight));

    for (int i = 0; i < randomElementsHBAO; i++)
    {
        float2Offsets[i] = glm::vec4(float(i % 4) + 0.5f, float(i / 4) + 0.5f, 0.0f, 0.0f);
        jitters[i] = hbaoRandom[i];
    }
}
void HBAO::DrawLinearDepth()
{
    // 3. Linear Depth to remove noise
    // ------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, depthLinearFBO);
    RenderData::shaderDepthHBAO.use();
    RenderData::shaderDepthHBAO.setVec4(
        "clipInfo",
        glm::vec4(
            RenderData::nearP * RenderData::farP,
            RenderData::nearP - RenderData::farP,
            RenderData::farP,
            1.0f)
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneDepthStencilTexture); // inputTexture

    renderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

}
void HBAO::AOCalculation()
{
    if (RenderData::HBAOBlur || RenderData::OnlyHBAO)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, calculationFBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    }

    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "passed info to shader");
    RenderData::shaderCalculateHBAO.use();

    RenderData::shaderCalculateHBAO.setBool("aoBlurPresent", RenderData::HBAOBlur);
    RenderData::shaderCalculateHBAO.setVec4("projInfo", projInfo);
    RenderData::shaderCalculateHBAO.setVec2("InvFullResolution", InvFullResolution);
    RenderData::shaderCalculateHBAO.setFloat("NegInvR2", NegInvRadius2);
    RenderData::shaderCalculateHBAO.setFloat("PowExponent", powExponent);
    RenderData::shaderCalculateHBAO.setFloat("NDotVBias", nDotVBias);
    RenderData::shaderCalculateHBAO.setFloat("AOMultiplier", aoMultiplier);
    RenderData::shaderCalculateHBAO.setFloat("RadiusToScreen", RadiusToScreen);
    RenderData::shaderCalculateHBAO.setInt("AO_RANDOMTEX_SIZE", randomSizeHBAO);


    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
        GL_DEBUG_SEVERITY_NOTIFICATION, -1, "scene Depth linear attach");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneDepthlinear);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, randomViewTextureHBAO);

    renderQuad();
}

void HBAO::DrawBlur()
{
    float meters2viewspace = 1.0f;

    RenderData::shaderBlurHBAO.use();
    RenderData::shaderBlurHBAO.setBool("aoBlurPresent", 0);
    RenderData::shaderBlurHBAO.setFloat("g_Sharpness", (blurSharpness / meters2viewspace));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sceneDepthlinear);

    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resultTextureHBAO);

    RenderData::shaderBlurHBAO.setVec2("g_InvResolutionDirection", glm::vec2(1.0f / float(screenWidth),0.0f));

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
        GL_DEBUG_SEVERITY_NOTIFICATION, -1, "blur 1 ->");

    renderQuad();

    // final output to main fbo
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO); //do zmiany na -> sceneFBO
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);

    RenderData::shaderBlur2HBAO.use();
    RenderData::shaderBlur2HBAO.setBool("aoBlurPresent", 1);
    RenderData::shaderBlur2HBAO.setFloat("g_Sharpness", (blurSharpness / meters2viewspace));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurTextureHBAO);

    RenderData::shaderBlur2HBAO.setVec2("g_InvResolutionDirection", glm::vec2(0.0f, 1.0f / float(screenHeight)));
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
        GL_DEBUG_SEVERITY_NOTIFICATION, -1, "blur 2 ->");
    renderQuad();
}

void HBAO::Clearing()
{
    __CHECK_FOR_ERRORS
    // cleaning
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_SAMPLE_MASK);
    glSampleMaski(0, ~0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void HBAO::OnlyAO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    RenderData::shaderNotRedAO.use();
    glActiveTexture(GL_TEXTURE0);
    if(RenderData::HBAOBlur)
        glBindTexture(GL_TEXTURE_2D, blurTextureHBAO);
    else
        glBindTexture(GL_TEXTURE_2D, resultTextureHBAO);

    renderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void HBAO::_InitRandom()
{
    std::mt19937 rmt;

    float numDir = 8;  // keep in sync to glsl

    signed short hbaoRandomShort[512];

    for (int i = 0; i < randomElementsHBAO * maxSamples; i++)
    {
        float Rand1 = static_cast<float>(rmt()) / 4294967296.0f;
        float Rand2 = static_cast<float>(rmt()) / 4294967296.0f;

        // Use random rotation angles in [0,2PI/NUM_DIRECTIONS)
        float Angle = glm::two_pi<float>() * Rand1 / numDir;
        hbaoRandom[i].x = cosf(Angle);
        hbaoRandom[i].y = sinf(Angle);
        hbaoRandom[i].z = Rand2;
        hbaoRandom[i].w = 0;
#define SCALE ((1 << 15))
        hbaoRandomShort[i * 4 + 0] = (signed short)(SCALE * hbaoRandom[i].x);
        hbaoRandomShort[i * 4 + 1] = (signed short)(SCALE * hbaoRandom[i].y);
        hbaoRandomShort[i * 4 + 2] = (signed short)(SCALE * hbaoRandom[i].z);
        hbaoRandomShort[i * 4 + 3] = (signed short)(SCALE * hbaoRandom[i].w);
#undef SCALE
    }

    glGenTextures(1, &randomTextureHBAO);
    glBindTexture(GL_TEXTURE_2D_ARRAY, randomTextureHBAO);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA16_SNORM, randomSizeHBAO, randomSizeHBAO, maxSamples);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, randomSizeHBAO, randomSizeHBAO, maxSamples, GL_RGBA, GL_SHORT, hbaoRandomShort);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

 
    glGenTextures(1, &randomViewTextureHBAO);
    glTextureView(randomViewTextureHBAO, GL_TEXTURE_2D, randomTextureHBAO, GL_RGBA16_SNORM, 0, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, randomViewTextureHBAO);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    return;
}

void HBAO::_InitHBAOBuffer()
{
    glGenTextures(1, &sceneColorTexture);
    glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, screenWidth, screenHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &sceneDepthStencilTexture);
    glBindTexture(GL_TEXTURE_2D, sceneDepthStencilTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneColorTexture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, sceneDepthStencilTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenTextures(1, &sceneDepthlinear);
    glBindTexture(GL_TEXTURE_2D, sceneDepthlinear);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, screenWidth, screenHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &depthLinearFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthLinearFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneDepthlinear, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLenum formatAO = GL_RG16F;
    GLint  swizzle[4] = { GL_RED, GL_GREEN, GL_ZERO, GL_ZERO };

    glGenTextures(1, &resultTextureHBAO);
    glBindTexture(GL_TEXTURE_2D, resultTextureHBAO);
    glTexStorage2D(GL_TEXTURE_2D, 1, formatAO, screenWidth, screenHeight);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &blurTextureHBAO);
    glBindTexture(GL_TEXTURE_2D, blurTextureHBAO);
    glTexStorage2D(GL_TEXTURE_2D, 1, formatAO, screenWidth, screenHeight);
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &calculationFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, calculationFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTextureHBAO, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, blurTextureHBAO, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
