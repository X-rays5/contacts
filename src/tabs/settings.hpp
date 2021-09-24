//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_SETTINGS_HPP
#define CONTACTS_SETTINGS_HPP
namespace contacts {
	namespace tabs {
		namespace settings {
			struct ActiveFont {
				const char* name = "Roboto-Medium";
				const char* path = "fonts/Roboto-Medium.ttf";
			};
			ActiveFont current_font;

			void Render(simple_db::TableSession* db) {

			}
		}
	}
}
#endif //CONTACTS_SETTINGS_HPP
