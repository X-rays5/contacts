//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_OPENGL_HPP
#define CONTACTS_OPENGL_HPP
#include <cstdint>

namespace contacts {
	namespace opengl {
		const char* SetOpenGLAttributes() {
			// Decide GL+GLSL versions
		#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100
			const char* glsl_version = "#version 100";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		#elif defined(__APPLE__)
			// GL 3.2 Core + GLSL 150
			const char* glsl_version = "#version 150";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		#else
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		#endif

			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

			return glsl_version;
		}

		enum class Kvsync_state : std::int8_t {
			off = 0,
			on = 1,
			adaptive = -1,
		};
		inline bool SetVsync(Kvsync_state state) {
			if (SDL_GL_SetSwapInterval(static_cast<int>(state)) == -1)
				return false;
			return true;
		}
	} // opengl
} // contacts
#endif //CONTACTS_OPENGL_HPP
