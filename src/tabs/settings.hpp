//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_SETTINGS_HPP
#define CONTACTS_SETTINGS_HPP
#include <vector>
#include <string>
#include <filesystem>
// expects win32 path on windows but this program is cross platform
#ifdef WIN32
#undef WIN32
#include <simple_db/db.hpp>
#define WIN32
#else
#include <simple_db/db.hpp>
#endif
#include <imgui.h>
#include "../ui/ui.hpp"

namespace contacts {
	namespace tabs {
		namespace settings {
			struct ActiveFont {
				char name[2048] = "Roboto-Medium";
			};

			void Render(ActiveFont* current_font, simple_db::TableSession* db, std::shared_ptr<::contacts::ui> ui);
		}
	}
}
#endif //CONTACTS_SETTINGS_HPP
