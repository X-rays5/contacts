//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_UI_HPP
#define CONTACTS_UI_HPP
#include <string>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <utility>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

namespace contacts {
	class ui {
	public:
		using render_cb_t = std::function<void()>;

		ui(SDL_Window* window, SDL_GLContext* gl_context, const char* glsl_version);
		~ui();

		void Tick();
		void SdlEvent(SDL_Event* event) {
			ImGui_ImplSDL2_ProcessEvent(event);
		}

		void SetRenderCB(render_cb_t cb) {
			render_cb = std::move(cb);
		}

		std::unordered_map<std::string, ImFont*> GetFonts() {
			return fonts_;
		}

		bool LoadFont(const std::string& name, const std::string& path, float size);
		ImFont* GetFont(const std::string& name);

		inline static void Tooltip(const char* text, ...) {
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				va_list args;
				va_start(args, text);
				char buf[2048] = { 0 };
				vsprintf_s(buf, text, args);
				va_end(args);
				ImGui::Text("%s", buf);
				ImGui::EndTooltip();
			}
		}
	private:
		SDL_Window* window_;
		render_cb_t render_cb;
		std::unordered_map<std::string, ImFont*> fonts_;
	private:
	};
}
#endif //CONTACTS_UI_HPP
