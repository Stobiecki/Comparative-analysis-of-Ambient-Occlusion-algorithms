#include "GUI.h"

void GUI::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void GUI::DockspaceInit()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool ImGuiDockingWindowsInitFlag = true;
	ImGuiID dockspaceID = ImGui::GetID("MyDockspace");

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 viewportPos = ImVec2(viewport->Pos);
	ImVec2 viewportSize = ImVec2(viewport->Size);
	ImVec2 copyViewportSize = ImVec2(viewportSize.x / 4, viewportSize.y);
	ImVec2 copyViewportPos = ImVec2(viewportSize.x-copyViewportSize.x, viewportPos.y);

	std::cout << "size x: "<< viewportSize.x<<", y: "<< viewportSize.y<<std::endl;
	std::cout << "pos x: "<< viewportPos.x<<", y: "<< viewportPos.y<<std::endl;

	ImGui::SetNextWindowPos(copyViewportPos);
	ImGui::SetNextWindowSize(copyViewportSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.8f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, -1.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags dock_window_flags = 1;
	dock_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	dock_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("MyDockspace", NULL, dock_window_flags);

	if (ImGui::DockBuilderGetNode(dockspaceID) == NULL || ImGuiDockingWindowsInitFlag == true)
	{
		ImGuiDockingWindowsInitFlag = false;

		ImGuiID dock_main_id = dockspaceID;

		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dock_main_id, viewport->Size);

		auto control_panel_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.59f, nullptr, &dock_main_id);

		ImGui::DockBuilderDockWindow("##ControlPanel", control_panel_id);
		ImGui::DockBuilderDockWindow("##DiagnosticWindow", dock_main_id);


		ImGui::DockBuilderGetNode(control_panel_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResizeX  // | ImGuiDockNodeFlags_NoResizeX 
			| ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;

		ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar // | ImGuiDockNodeFlags_NoResizeX 
			| ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;


		ImGui::DockBuilderFinish(dockspaceID);
	}

	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::Draw()
{
	// Dockspace Window
	ImGuiID dockspaceID = ImGui::GetID("MyDockspace");
	static ImGuiWindowFlags dock_window_flags = 1;
	dock_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	dock_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 viewportPos = ImVec2(viewport->Pos);
	ImVec2 viewportSize = ImVec2(viewport->Size);
	ImVec2 copyViewportSize = ImVec2(RenderData::GUIWidth, viewportSize.y);
	ImVec2 copyViewportPos = ImVec2(viewportSize.x - copyViewportSize.x, viewportPos.y);

	ImGui::SetNextWindowPos(copyViewportPos);
	ImGui::SetNextWindowSize(copyViewportSize);

	ImGui::Begin("MyDockspace", NULL, dock_window_flags);
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
	ImGui::End();

	// Diagnostic Window
	static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize 
		| ImGuiWindowFlags_NoResize;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.f);
	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("##DiagnosticWindow", 0, windowFlags);
	ImGui::PopStyleVar();
	ImGui::TextUnformatted("Choose Ambient Occlusion");
	const char* ambientOccclusionNameList[] = { "SSAO", "SSAO+", "HBAO"};
	ImGui::Combo("AO type", &RenderData::chosenAmbientOcclusion, ambientOccclusionNameList, IM_ARRAYSIZE(ambientOccclusionNameList));
	ImGui::Separator();
	ImGui::Text((std::string("Camera postition\n\nx: ")
		+ std::to_string(RenderData::myCamera.Position.x)
		+ "\ny: "
		+ std::to_string(RenderData::myCamera.Position.y)
		+ "\nz: "
		+ std::to_string(RenderData::myCamera.Position.z)
		).c_str());	
	ImGui::Separator();
	static char screenshotName[128] = "screen.png";
	ImGui::TextUnformatted("Screenshot name\n\n");
	if (ImGui::InputText("##Screenshot name", screenshotName, IM_ARRAYSIZE(screenshotName)))
	{
		RenderData::PrintscreenName = string(screenshotName);
	} ImGui::SameLine();
	HelpMarker("Press \"P\" to take a screenshot");
	ImGui::Checkbox("Enable printscreens", &RenderData::PrintscreenMode);
	ImGui::Separator();
	ImGui::TextUnformatted("Screen resolution\n\n");
	if (ImGui::BeginTable("##Resolutions", 3, ImGuiTableFlags_None))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::RadioButton("360x800", &RenderData::currentScreenResolution, Res360x800);
		ImGui::TableSetColumnIndex(1);
		ImGui::RadioButton("800x600", &RenderData::currentScreenResolution, Res800x600);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::RadioButton("1440x810", &RenderData::currentScreenResolution, Res1440x810);
		ImGui::TableSetColumnIndex(1);
		ImGui::RadioButton("1920x1080", &RenderData::currentScreenResolution, Res1920x1080);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::RadioButton("2560x1440", &RenderData::currentScreenResolution, Res2560x1440);
		ImGui::TableSetColumnIndex(1);
		ImGui::RadioButton("3840x2160", &RenderData::currentScreenResolution, Res3840x2160);
		ImGui::EndTable();
	}

	ImGui::End();

	// Control Panel
	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("##ControlPanel", 0, windowFlags);
	ImGui::TextUnformatted(ambientOccclusionNameList[RenderData::chosenAmbientOcclusion]);
	static void* texture = (void*)RenderData::CurrentSsao->noiseTexture;
	//SSAO
	if (RenderData::chosenAmbientOcclusion == SSAO_)
	{
		ImGui::Checkbox("AO only", &RenderData::OnlySSAO);
		ImGui::Checkbox("Blur", &RenderData::SSAOBlur);
		ImGui::Separator();
		ImGui::TextUnformatted("AO settings\n\n");
		ImGui::PushItemWidth(180.f);
		if (ImGui::SliderInt("Kernel Size", &RenderData::CurrentSsao->kernelSize, 1, 256))
		{
			std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
			std::default_random_engine generator;

			RenderData::CurrentSsao->ssaoKernel.clear();
			//cout << "size: " << RenderData::CurrentSsao->ssaoKernel.size();
			for (unsigned int i = 0; i < RenderData::CurrentSsao->kernelSize; ++i)
			{
				glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
				sample = glm::normalize(sample);
				sample *= randomFloats(generator);
				float scale = float(i) / 64.0f;

				// scale samples s.t. they're more aligned to center of kernel
				scale = lerpFunc(0.1f, 1.0f, scale * scale);
				sample *= scale;
				RenderData::CurrentSsao->ssaoKernel.push_back(sample);
			}
		}
		ImGui::SliderFloat("Radius", &RenderData::CurrentSsao->radius, 0.001f, 20.f);
		ImGui::SliderFloat("Bias", &RenderData::CurrentSsao->bias, 0.001f, 2.f);
		if (ImGui::InputFloat("Noise size", &RenderData::CurrentSsao->noiseSize, 1.0f, 5.0f, "%.1f"))
		{
			if (RenderData::CurrentSsao->noiseSize < 1)
				RenderData::CurrentSsao->noiseSize = 1;
			else if (RenderData::CurrentSsao->noiseSize > 64)
				RenderData::CurrentSsao->noiseSize = 64;
		}
		ImGui::SliderFloat("Ambient Amp.", &RenderData::CurrentSsao->aoAmplification, 0.01f, 4.0f);
		ImGui::PopItemWidth();
		ImGui::Checkbox("Inverted normals", &RenderData::invertedNormals);
		ImGui::Checkbox("##display SSAO textures", &RenderData::displayTexture); ImGui::SameLine();
		static int chosenSSAOTexture = 9;
		const char* SSAOTextureName[] = { "gBuffer", "gPosition", "gNormal", "gAlbedo", "RBODepth", "ssaoFBO", "ssaoBlurFBO", "ssaoColorBuffer", "ssaoColorBufferBlur", "noiseTexture" };
		if (ImGui::Combo("##display SSAO textures list", &chosenSSAOTexture, SSAOTextureName, IM_ARRAYSIZE(SSAOTextureName)))
		{
			cout << "\nchosenSSAOTexture: " << chosenSSAOTexture;
			switch (chosenSSAOTexture)
			{
			case 0:
				texture = (void*)RenderData::CurrentSsao->gBuffer;
				break;
			case 1:
				texture = (void*)RenderData::CurrentSsao->gPosition;
				break;
			case 2:
				texture = (void*)RenderData::CurrentSsao->gNormal;
				break;
			case 3:
				texture = (void*)RenderData::CurrentSsao->gAlbedo;
				break;
			case 4:
				texture = (void*)RenderData::CurrentSsao->RBODepth;
				break;
			case 5:
				texture = (void*)RenderData::CurrentSsao->gMatMP;
				break;
			case 6:
				texture = (void*)RenderData::CurrentSsao->gNormalForLight;
				break;
			case 7:
				texture = (void*)RenderData::CurrentSsao->ssaoFBO;
				break;
			case 8:
				texture = (void*)RenderData::CurrentSsao->ssaoBlurFBO;
				break;
			case 9:
				texture = (void*)RenderData::CurrentSsao->ssaoColorBuffer;
				break;
			case 10:
				texture = (void*)RenderData::CurrentSsao->ssaoColorBufferBlur;
				break;
			case 11:
				texture = (void*)RenderData::CurrentSsao->noiseTexture;
				break;
			default:
				break;
			}
		}
	}
	//SSAO+
	else if(RenderData::chosenAmbientOcclusion == SSAOplus_)
	{
		ImGui::Checkbox("AO only", &RenderData::OnlySSAOplus);
		ImGui::Checkbox("Blur", &RenderData::SSAOplusBlur);
		ImGui::Separator();
		ImGui::TextUnformatted("AO settings\n\n");
		ImGui::PushItemWidth(180.f);
		if (ImGui::SliderInt("Kernel Size", &RenderData::CurrentSsaoPlus->kernelSize, 1, 256))
		{
			std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
			std::default_random_engine generator;

			RenderData::CurrentSsaoPlus->ssaoKernel.clear();
			cout << "size: " << RenderData::CurrentSsaoPlus->ssaoKernel.size();
			for (unsigned int i = 0; i < RenderData::CurrentSsaoPlus->kernelSize; ++i)
			{
				glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
				sample = glm::normalize(sample);
				sample *= randomFloats(generator);
				float scale = float(i) / 64.0f;

				// scale samples s.t. they're more aligned to center of kernel
				scale = lerpFunc(0.1f, 1.0f, scale * scale);
				sample *= scale;
				RenderData::CurrentSsaoPlus->ssaoKernel.push_back(sample);
			}
		}
		ImGui::SliderFloat("Radius", &RenderData::CurrentSsaoPlus->radius, 0.001f, 20.f);
		ImGui::SliderFloat("Bias", &RenderData::CurrentSsaoPlus->bias, 0.001f, 2.f);
		if (ImGui::InputFloat("Noise size", &RenderData::CurrentSsaoPlus->noiseSize, 1.0f, 5.0f, "%.1f"))
		{
			if (RenderData::CurrentSsaoPlus->noiseSize < 1)
				RenderData::CurrentSsaoPlus->noiseSize = 1;
			else if (RenderData::CurrentSsaoPlus->noiseSize > 64)
				RenderData::CurrentSsaoPlus->noiseSize = 64;
		}
		ImGui::SliderFloat("Ambient Amp.", &RenderData::CurrentSsaoPlus->aoAmplification, 0.01f, 4.0f);
		ImGui::PopItemWidth();
		ImGui::Checkbox("Inverted normals", &RenderData::invertedNormals);
		ImGui::Checkbox("##display SSAOplus textures", &RenderData::displayTexture); ImGui::SameLine();
		static int chosenSSAOTexture = 9;
		const char* SSAOTextureName[] = { "gBuffer", "gPosition", "gNormal", "gAlbedo", "gMatMP", "gNormalForLight", "RBODepth", "ssaoFBO", "ssaoBlurFBO", "ssaoColorBuffer", "ssaoColorBufferBlur", "noiseTexture"};
		if (ImGui::Combo("##display SSAOplus textures list", &chosenSSAOTexture, SSAOTextureName, IM_ARRAYSIZE(SSAOTextureName)))
		{
			cout << "\nchosenSSAOTexture: " << chosenSSAOTexture;
			switch (chosenSSAOTexture)
			{
			case 0:
				texture = (void*)RenderData::CurrentSsaoPlus->gBuffer;
				break;
			case 1:
				texture = (void*)RenderData::CurrentSsaoPlus->gPosition;
				break;
			case 2:
				texture = (void*)RenderData::CurrentSsaoPlus->gNormal;
				break;
			case 3:
				texture = (void*)RenderData::CurrentSsaoPlus->gAlbedo;
				break;
			case 4:
				texture = (void*)RenderData::CurrentSsaoPlus->gMatMP;
				break;
			case 5:
				texture = (void*)RenderData::CurrentSsaoPlus->gNormalForLight;
				break;
			case 6:
				texture = (void*)RenderData::CurrentSsaoPlus->RBODepth;
				break;
			case 7:
				texture = (void*)RenderData::CurrentSsaoPlus->ssaoFBO;
				break;
			case 8:
				texture = (void*)RenderData::CurrentSsaoPlus->ssaoBlurFBO;
				break;
			case 9:
				texture = (void*)RenderData::CurrentSsaoPlus->ssaoColorBuffer;
				break;
			case 10:
				texture = (void*)RenderData::CurrentSsaoPlus->ssaoColorBufferBlur;
				break;
			case 11:
				texture = (void*)RenderData::CurrentSsaoPlus->noiseTexture;
				break;
			default:
				break;
			}
		}
	}
	else if (RenderData::chosenAmbientOcclusion == HBAO_)
	{
		ImGui::Checkbox("AO only", &RenderData::OnlyHBAO);
		ImGui::Checkbox("Blur", &RenderData::HBAOBlur);
		ImGui::Separator();
		ImGui::TextUnformatted("AO settings\n\n");
		ImGui::Checkbox("##display HBAO textures", &RenderData::displayTexture); ImGui::SameLine();
		static int chosenHBAOTexture = 0;
		const char* HBAOTextureName[] = { "sceneColorTexture", "sceneDepthStencilTexture", "sceneDepthlinear", "randomTextureHBAO", "randomViewTextureHBAO", "resultTextureHBAO", "blurTextureHBAO", "sceneFBO", "depthLinearFBO", "calculationFBO" };
		if (ImGui::Combo("##display HBAO textures list", &chosenHBAOTexture, HBAOTextureName, IM_ARRAYSIZE(HBAOTextureName)))
		{
			cout << "\nchosen HBAO Texture: " << chosenHBAOTexture;
			switch (chosenHBAOTexture)
			{
			case 0:
				texture = (void*)RenderData::CurrentHbao->sceneColorTexture;
				break;
			case 1:
				texture = (void*)RenderData::CurrentHbao->sceneDepthStencilTexture;
				break;
			case 2:
				texture = (void*)RenderData::CurrentHbao->sceneDepthlinear;
				break;
			case 3:
				texture = (void*)RenderData::CurrentHbao->randomTextureHBAO;
				break;
			case 4:
				texture = (void*)RenderData::CurrentHbao->randomViewTextureHBAO;
				break;
			case 5:
				texture = (void*)RenderData::CurrentHbao->resultTextureHBAO;
				break;
			case 6:
				texture = (void*)RenderData::CurrentHbao->blurTextureHBAO;
				break;
			case 7:
				texture = (void*)RenderData::CurrentHbao->sceneFBO;
				break;
			case 8:
				texture = (void*)RenderData::CurrentHbao->depthLinearFBO;
				break;
			case 9:
				texture = (void*)RenderData::CurrentHbao->calculationFBO;
				break;
			default:
				break;
			}
		}
		ImGui::PushItemWidth(180.f);
		ImGui::SliderFloat("Intensity", &RenderData::CurrentHbao->intensity, 0.0f, 8.f);
		ImGui::SliderFloat("Radius", &RenderData::CurrentHbao->radius, 0.0f, 4.f);
		ImGui::SliderFloat("Bias", &RenderData::CurrentHbao->bias, 0.0f, 1.f);
		ImGui::SliderFloat("Blur Sharpness", &RenderData::CurrentHbao->blurSharpness, 0.f, 128.f);
		ImGui::PopItemWidth();

	}
	// Light //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ImGui::Separator();
	ImGui::Text("Light\n\n");
	ImGui::Text("Ambient    "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Light Ambient x", &RenderData::light.Ambient.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Ambient y", &RenderData::light.Ambient.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Ambient z", &RenderData::light.Ambient.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Diffuse    "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Light Diffuse x", &RenderData::light.Diffuse.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Diffuse y", &RenderData::light.Diffuse.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Diffuse z", &RenderData::light.Diffuse.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Specular   "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Light Specular x", &RenderData::light.Specular.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Specular y", &RenderData::light.Specular.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Specular z", &RenderData::light.Specular.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Attenuation"); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Light Attenuation x", &RenderData::light.Attenuation.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Attenuation y", &RenderData::light.Attenuation.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Attenuation z", &RenderData::light.Attenuation.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Position   "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Light Position x", &RenderData::light.Position.x, -70.f, 60.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Position y", &RenderData::light.Position.y, -10.f, 100.f); ImGui::SameLine();
	ImGui::SliderFloat("##Light Position z", &RenderData::light.Position.z, -4.f, 4.f);
	ImGui::PopItemWidth();

	// Material //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ImGui::Separator();
	ImGui::Text("Material\n\n");
	ImGui::Text("Ambient    "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Material Ambient x", &RenderData::material.Ambient.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Ambient y", &RenderData::material.Ambient.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Ambient z", &RenderData::material.Ambient.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Diffuse    "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Material Diffuse x", &RenderData::material.Diffuse.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Diffuse y", &RenderData::material.Diffuse.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Diffuse z", &RenderData::material.Diffuse.z, 0.0f, 1.f);
	ImGui::PopItemWidth();

	ImGui::Text("Specular   "); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::SliderFloat("##Material Specular x", &RenderData::material.Specular.x, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Specular y", &RenderData::material.Specular.y, 0.0f, 1.f); ImGui::SameLine();
	ImGui::SliderFloat("##Material Specular z", &RenderData::material.Specular.z, 0.0f, 1.f);
	ImGui::PopItemWidth();
	ImGui::TextUnformatted("Shininess  ");  ImGui::SameLine();
	ImGui::PushItemWidth(196.f);
	ImGui::SliderFloat("##Shininess", &RenderData::material.Shininess, 0.f, 128.f);
	ImGui::PopItemWidth();

	// Logging data /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ImGui::Separator();
	ImGui::Text("Analytics\n\n");
	ImGui::Text("Number of iterations for tests");
	ImGui::InputInt("##Number of iterations", &RenderData::logger.loggedDataLimit);
	ImGui::TextUnformatted("\nRendering time");
	if (ImGui::Button("Log data")) 
		RenderData::enableLogging = true;

	ImGui::Text("Scalability Tests");
	if (ImGui::Button("Log scalability data"))
		RenderData::enableScalabilityLogging = true;

	ImGui::End();

	// display chosen texture 
	if(RenderData::displayTexture)
	{
		ImGui::Begin("Global light shadow map");
		ImGui::BeginChild("Global light shadow map Child");

		// we access the ImGui window size
		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;

		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddImage(
			texture,
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x + window_width, pos.y + window_height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::EndChild();
		ImGui::End();
	}


	//debug window
	//ImGui::ShowDemoWindow();
}