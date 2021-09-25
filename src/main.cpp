#include <cstdio>
#include <vector>
#include <memory>
#include <thread>
#include <SDL.h>
#include "opengl.hpp"
#include "ui/ui.hpp"
#include "tabs/contacts.hpp"
#include "tabs/settings.hpp"

#define WINDOW_WIDTH 380
#define WINDOW_HEIGHT 600

void SetImGuiStyle() {
	auto style = ImGui::GetStyle();
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.WindowTitleAlign = ImVec2(0.5, 0.5);
	style.ChildBorderSize = 1.f;
	style.ChildRounding = 0.2f;
	//ImGui::StyleColorsClassic();
}

std::vector<std::filesystem::path> FindFonts(const std::string& path) {
	std::vector<std::filesystem::path> res;
	if (std::filesystem::is_directory(path)) {
		std::filesystem::directory_iterator dirIt{path};
		for (auto&& dirEntry : dirIt)
		{
			auto entry_path = dirEntry.path();
			if (entry_path.has_filename())
			{
				if (dirEntry.is_regular_file())
					if (entry_path.extension().string() == ".ttf")
						res.emplace_back(entry_path);
			}
		}
	} else {
		std::filesystem::create_directories(path);
	}
	return res;
}

// Main code
int main(int, char**)
{
	#ifdef WIN32
	#ifdef NDEBUG
		FreeConsole();
	#endif
	#endif
    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    const char* glsl_version = contacts::opengl::SetOpenGLAttributes();

    // Create window with graphics context
    SDL_Window* window = SDL_CreateWindow("Contacts", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
	    printf("Failed to create window\n");
	    SDL_Quit();
	    return EXIT_FAILURE;
    }

    SDL_SetWindowMaximumSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetWindowMinimumSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetWindowResizable(window, SDL_FALSE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    if (!contacts::opengl::SetVsync(contacts::opengl::Kvsync_state::adaptive)) {
    	contacts::opengl::SetVsync(contacts::opengl::Kvsync_state::on);
    }

    std::shared_ptr<contacts::ui> ui;
    ui = std::make_shared<contacts::ui>(window, &gl_context, glsl_version);

    simple_db::DB db("contacts/db.simpledb");
    auto contacts = db["contacts"];
    auto settings = db["settings"];
    contacts::tabs::settings::ActiveFont current_font;
    strcpy_s(current_font.name, sizeof(current_font.name), settings.Get("font-name").value.c_str());

    SetImGuiStyle();
    std::string path = std::filesystem::current_path().string();
					#ifdef WIN32
    path.append("\\fonts");
					#elif
    path.append("/fonts");
					#endif
    auto fonts = FindFonts(path);
    for (auto&& font : fonts)
    	ui->LoadFont(font.filename().string(), font.string(), 18.f);
    auto io = ImGui::GetIO();
    while(!io.Fonts->IsBuilt()) {
    	io.Fonts->Build();
    }

    contacts::tabs::contacts::SortBy sort;
    bool render_ui = true;
    ui->SetRenderCB([ui, &sort, &contacts, &settings, &current_font, &render_ui]{
    	if (render_ui) {
    		ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT + 24));
    		ImGui::SetNextWindowPos(ImVec2(0,-24));

    		ImGui::PushFont(ui->GetFont(current_font.name));
    		if (ImGui::Begin("Contacts", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing)) {
    			ImGui::BeginTabBar("tabs");
    			if (ImGui::BeginTabItem("Contacts")) {
    				contacts::tabs::contacts::Render(&sort, &contacts);
    				ImGui::EndTabItem();
    			}
    			if (ImGui::BeginTabItem("Settings")) {
    				contacts::tabs::settings::Render(&current_font, &settings, ui);
    				ImGui::EndTabItem();
    			}
    			ImGui::EndTabBar();
    		}
    		ImGui::End();
    		ImGui::PopFont();
    	}
    });
    // Main loop
    bool done = false;
    while (!done)
    {
    	std::uint64_t start = SDL_GetPerformanceCounter();

    	SDL_Event event;
    	while (SDL_PollEvent(&event))
    	{
    		ui->SdlEvent(&event);
    		switch(event.type) {
    			case SDL_QUIT:
    				done = true;
    				break;
    			case SDL_WINDOWEVENT:
				    switch(event.window.event) {
				    	case SDL_WINDOWEVENT_CLOSE:
				    		if (event.window.windowID == SDL_GetWindowID(window))
				    			done = true;
				    		break;
				    	// fix imgui crashing on minimize
				    	case SDL_WINDOWEVENT_FOCUS_GAINED:
				    		render_ui = true;
						    break;
				    	case SDL_WINDOWEVENT_MINIMIZED:
				    		render_ui = false;
						    break;
				    }
				    break;
    		}
    ; ; 	}

    	ui->Tick();

    	// cap fps at 30 to minimize gpu usage
    	std::uint64_t end = SDL_GetPerformanceCounter();
    	float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    	std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(16.666f - elapsedMS) * 2));
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}