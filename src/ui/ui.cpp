//
// Created by X-ray on 9/24/2021.
//

#include "ui.hpp"

namespace contacts {
	ui::ui(SDL_Window* window, SDL_GLContext* gl_context, const char* glsl_version) : window_(window) {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window_, gl_context);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	ui::~ui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ui::Tick() {
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		std::invoke(render_cb);

		ImGui::Render();
		auto io = ImGui::GetIO();
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
		glClearColor(0,0,0, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window_);
	}

	bool ui::LoadFont(const std::string& name, const std::string& path, float size) {
		if (std::filesystem::exists(path)) {
			auto io = ImGui::GetIO();
			ImFont* font = io.Fonts->AddFontFromFileTTF(path.c_str(), size);
			if (font) {
				fonts_[name] = font;
				return true;
			}
		}
		return false; // failed to load font
	}

	ImFont* ui::GetFont(const std::string& name) {
			auto entry = fonts_.find(name);

			if (entry != fonts_.end()) {
				return entry->second;
			}
			return nullptr;
	}
}