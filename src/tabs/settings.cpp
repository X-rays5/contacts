//
// Created by X-ray on 9/25/2021.
//

#include "settings.hpp"

namespace contacts {
	namespace tabs {
		namespace settings {
			namespace {
				struct OptionCategory {
					enum class order : std::uint8_t {
						font,
						};

					const char* options[1] = {
						"Font",
						};
					std::uint8_t current = NULL;
				};
				OptionCategory option_category;

				void FontSettingsWidget(ActiveFont* current_font, simple_db::TableSession* db, std::shared_ptr<contacts::ui> ui) {
					auto fonts = ui->GetFonts();
					if (fonts.size() > 0) {
						if (ImGui::BeginCombo("##fonts combo", current_font->name)) {
							ImGui::PopFont();
							for (auto&& font : fonts) {
								bool is_selected = (current_font->name == font.first);
								ImGui::PushFont(font.second);
								if (ImGui::Selectable(font.first.c_str(), is_selected)) {
									db->Put("font-name",  font.first);
									strcpy_s(current_font->name, sizeof(current_font->name), font.first.data());
								}
								ImGui::PopFont();
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::PushFont(ui->GetFont(current_font->name));
							ImGui::EndCombo();
						}
						ImGui::Text("Restart app to reload custom fonts");
						ImGui::Separator();
					} else {
						ImGui::Text("No custom fonts loaded\nRestart app to reload custom fonts");
						ImGui::Separator();
					}
				}

				void SelectCategoryWidget() {
					if (ImGui::BeginCombo("##category combo", option_category.options[option_category.current])) {
						for (int i = 0; i < IM_ARRAYSIZE(option_category.options); i++) {
							bool is_selected = (option_category.options[option_category.current] == option_category.options[i]);
							if (ImGui::Selectable(option_category.options[i], is_selected))
								option_category.current = i;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
			} // unnamed


			void Render(ActiveFont* current_font, simple_db::TableSession* db, std::shared_ptr<contacts::ui> ui) {
				SelectCategoryWidget();
				if (ImGui::BeginChild("##categorychild", ImVec2(0,0), true)) {
					switch(static_cast<OptionCategory::order>(option_category.current)) {
						case OptionCategory::order::font:
							FontSettingsWidget(current_font, db, std::move(ui));
							break;
					}
					ImGui::EndChild();
				}
			}
		} // settings
	} // tabs
} // contacts