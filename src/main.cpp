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

    SetImGuiStyle();
    ui->LoadFont("Roboto-Medium", "fonts/Roboto-Medium.ttf", 18.f);
    ui->LoadFont(contacts::tabs::settings::current_font.name, contacts::tabs::settings::current_font.path, 18.f);

    simple_db::DB db("contacts/db.simpledb");
    auto contacts = db["contacts"];
    auto settings = db["settings"];

    contacts::tabs::contacts::SortBy sort;
    ui->SetRenderCB([ui, &sort, &contacts, &settings]{
    	ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT + 24));
    	ImGui::SetNextWindowPos(ImVec2(0,-24));

    	auto roboto = ui->GetFont("Roboto-Light");
    	ImGui::PushFont(roboto);
    	if (ImGui::Begin("Contacts", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing)) {
    		ImGui::BeginTabBar("tabs");
    		if (ImGui::BeginTabItem("Contacts")) {
    			contacts::tabs::contacts::Render(&sort, &contacts);
    			ImGui::EndTabItem();
    		}
    		if (ImGui::BeginTabItem("Settings")) {
    			contacts::tabs::settings::Render(&settings);
    			ImGui::EndTabItem();
    		}
    		ImGui::EndTabBar();
    	}
    	ImGui::End();
    	ImGui::PopFont();
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
    		if (event.type == SDL_QUIT)
    			done = true;
    		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
    			done = true;
    	}

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