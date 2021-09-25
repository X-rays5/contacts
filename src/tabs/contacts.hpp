//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_CONTACTS_HPP
#define CONTACTS_CONTACTS_HPP
#include <vector>
#include <algorithm>
// expects win32 path on windows but this program is cross platform
#ifdef WIN32
#undef WIN32
#include <simple_db/db.hpp>
#define WIN32
#else
#include <simple_db/db.hpp>
#endif
#include <rapidjson/document.h>
#include "../json.hpp"
#include "../ui/ui.hpp"

namespace contacts {
	namespace tabs {
		namespace contacts {
			struct SortBy {
				enum order : std::uint8_t {
					first,
					last,
					};

				const char* options[2] = {
					"First Name",
					"Last Name",
					};
				std::uint8_t current = NULL;
			};

			void Render(SortBy* sort, simple_db::TableSession* db);
		}
	}
}
#endif //CONTACTS_CONTACTS_HPP
