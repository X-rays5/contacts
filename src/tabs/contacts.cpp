//
// Created by X-ray on 9/25/2021.
//

#include "contacts.hpp"

namespace contacts {
	namespace tabs {
		namespace contacts {
			namespace {
				struct PhoneNumber {
					PhoneNumber() = default;

					explicit PhoneNumber(json::rapidjson_object_t& json) {
						number = json::GetString(json, "number");
						type = json::GetString(json, "type");
					}

					const char* number{};
					const char* type{};
				};

				struct Contact {
					Contact() = default;

					explicit Contact(rapidjson::Document& json) {
						first_name = (char*)json::GetString(json, "first");
						last_name = (char*)json::GetString(json, "last");
						auto phone_number_arr = json::ArrayToVector(json, "phone_number");
						for (auto&& number : phone_number_arr) {
							phone_numbers.emplace_back(PhoneNumber(number));
						}
						auto email_arr = json::ArrayToVector(json, "email");
						for (auto&& email : email_arr) {
							emails.emplace_back(json::GetString(email, "email"));
						}
					}

					char* first_name{};
					char* last_name{};
					std::vector<PhoneNumber> phone_numbers{};
					std::vector<const char*> emails{};
				};

				std::vector<std::string> SortContacts(SortBy* sort_settings, simple_db::TableSession* db) {
					std::vector<std::string> sort;
					auto db_iter = db->GetIterator();
					for (db_iter->SeekToFirst(); db_iter->Valid(); db_iter->Next()) {
						sort.emplace_back(db_iter->value().ToString());
					}
					delete db_iter;
					std::sort(sort.begin(), sort.end(), [&sort_settings](std::string a, std::string b){
						rapidjson::Document json;
						json.Parse(a.c_str());
						if (sort_settings->current == SortBy::first) {
							a = json::GetString(json, "first");
						} else {
							a = json::GetString(json, "last");
						}
						json = rapidjson::Document();
						json.Parse(b.c_str());
						if (sort_settings->current == SortBy::first) {
							b = json::GetString(json, "first");
						} else {
							b = json::GetString(json, "last");
						}

						return a<b;
					});

					return sort;
				}

				void SortWidget(SortBy* sort) {
					ImGui::Text("Sort by: ");
					ImGui::SameLine();
					if (ImGui::BeginCombo("##sort combo", sort->options[sort->current])) {
						for (int i = 0; i < IM_ARRAYSIZE(sort->options); i++) {
							bool is_selected = (sort->options[sort->current] == sort->options[i]);
							if (ImGui::Selectable(sort->options[i], is_selected))
								sort->current = i;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}

				namespace new_contact {
					char first_name[2048];
					char last_name[2048];
					std::vector<std::string> phone_numbers;
					std::vector<std::string> phone_numbers_type;
					std::uint8_t phone_number_c = 1;
					char type[2048];
					char number[2048];
					std::vector<std::string> emails;
					std::uint8_t email_c = 1;
					char email[2048];
				}

				std::string NewContactToJsonString() {
					rapidjson::Document json;
					json.SetObject();

					json.AddMember("first", rapidjson::Value().SetString(std::string(new_contact::first_name).c_str(), json.GetAllocator()), json.GetAllocator());
					json.AddMember("last", rapidjson::Value().SetString(std::string(new_contact::last_name).c_str(), json.GetAllocator()), json.GetAllocator());

					rapidjson::Value phonenumbers(rapidjson::kArrayType);
					for (int i = 0; i < new_contact::phone_numbers.size(); i++) {
						rapidjson::Value entry;

						entry.SetObject();
						entry.AddMember("number", rapidjson::Value().SetString(new_contact::phone_numbers[i].c_str(), json.GetAllocator()), json.GetAllocator());
						entry.AddMember("type", rapidjson::Value().SetString(new_contact::phone_numbers_type[i].c_str(), json.GetAllocator()), json.GetAllocator());

						phonenumbers.PushBack(entry, json.GetAllocator());
					}
					rapidjson::Value emails;
					emails.SetArray();
					for (int i = 0; i < new_contact::emails.size(); i++) {
						rapidjson::Value entry;

						entry.SetObject();
						entry.AddMember("email", rapidjson::Value().SetString(new_contact::emails[i].c_str(), json.GetAllocator()), json.GetAllocator());

						emails.PushBack(entry, json.GetAllocator());
					}
					json.AddMember("phone_number", phonenumbers, json.GetAllocator());
					json.AddMember("email", emails, json.GetAllocator());

					return json::stringify(json);
				}

				// true means still adding
				// false mean done adding
				bool AddContactDialog(simple_db::TableSession* db) {
					bool busy = true;
					ImGui::Text("First Name: ");
					ImGui::SameLine();
					ImGui::InputText("##first name new", new_contact::first_name, IM_ARRAYSIZE(new_contact::first_name));
					ImGui::Text("Last Name: ");
					ImGui::SameLine();
					ImGui::InputText("##last name new", new_contact::last_name, IM_ARRAYSIZE(new_contact::last_name));

					if (ImGui::CollapsingHeader("Phone Numbers")) {
						for (int i = 0; i < new_contact::phone_number_c - 1; i++) {
							ImGui::Text("%s: %s", new_contact::phone_numbers_type[i].c_str(), new_contact::phone_numbers[i].c_str());
							ImGui::Separator();
						}

						ImGui::Text("Number:");
						ImGui::SameLine(0.f, -0.1f);
						ImGui::PushItemWidth(60.f);
						ImGui::InputText("##typeadd", new_contact::type, IM_ARRAYSIZE(new_contact::type));
						ui::Tooltip("Number type");
						ImGui::PopItemWidth();
						ImGui::SameLine(0.f, -0.1f);
						ImGui::InputText("##numberadd", new_contact::number, IM_ARRAYSIZE(new_contact::number));
						ui::Tooltip("Phone number");
						ImGui::Separator();
						if (ImGui::Button("Add phone number")) {
							new_contact::phone_number_c++;
							new_contact::phone_numbers_type.push_back(new_contact::type);
							new_contact::phone_numbers.push_back(new_contact::number);
							strcpy_s(new_contact::type, sizeof(new_contact::type), "mobile");
							memset(new_contact::number, 0, sizeof(new_contact::number));
						}
						ui::Tooltip("Add new phone number field");
						ImGui::SameLine();
						if (ImGui::Button("Remove last number")) {
							if (new_contact::phone_number_c > 1) {
								new_contact::phone_number_c--;
								strcpy_s(new_contact::type, sizeof(new_contact::type), new_contact::phone_numbers_type.back().c_str());
								strcpy_s(new_contact::number, sizeof(new_contact::number), new_contact::phone_numbers.back().c_str());
								new_contact::phone_numbers_type.pop_back();
								new_contact::phone_numbers.pop_back();
							}
						}
						ui::Tooltip("Remove last added number");
					}
					if (ImGui::CollapsingHeader("Emails")) {
						for (int i = 0; i < new_contact::email_c - 1; i++) {
							ImGui::Text("%s", new_contact::emails[i].c_str());
							ImGui::Separator();
						}

						ImGui::Text("email:");
						ImGui::SameLine(0.f, -0.1f);
						ImGui::InputText("##emailadd", new_contact::email, IM_ARRAYSIZE(new_contact::email));
						ImGui::Separator();
						if (ImGui::Button("Add email")) {
							new_contact::email_c++;
							new_contact::emails.push_back(new_contact::email);
							memset(new_contact::email, 0, sizeof(new_contact::email));
						}
						ui::Tooltip("Add new email field");
						ImGui::SameLine();
						if (ImGui::Button("Remove last email")) {
							if (new_contact::email_c > 1) {
								new_contact::email_c--;
								strcpy_s(new_contact::email, sizeof(new_contact::email), new_contact::emails.back().c_str());
								new_contact::emails.pop_back();
							}
						}
						ui::Tooltip("Remove last added email");
					}
					if (ImGui::Button("Cancel##adding contact")) {
						busy = false;
					}
					ui::Tooltip("Stop creating contact.\nAll data will be lost!");
					ImGui::SameLine();
					if (ImGui::Button("Add##add contact")) {
						// make sure everything is in the vector
						if (new_contact::number[0] != 0) {
							new_contact::phone_numbers_type.push_back(new_contact::type);
							new_contact::phone_numbers.push_back(new_contact::number);
						}
						if (new_contact::email[0] == 0) {
							new_contact::emails.push_back(new_contact::email);
						}
						busy = false;

						std::string key = new_contact::first_name;
						key.append("_");
						key.append(new_contact::last_name);
						printf("%s\n", NewContactToJsonString().c_str());
						db->Put(key, NewContactToJsonString());
					}
					ui::Tooltip("Create new contact.");
					return busy;
				}

				bool adding_contact = false;
				void AddContactWidget(simple_db::TableSession* db) {
					if (!adding_contact) {
						if (ImGui::Button("Add new contact")) {
							memset(new_contact::first_name, 0, sizeof(new_contact::first_name));
							memset(new_contact::last_name, 0, sizeof(new_contact::last_name));
							strcpy_s(new_contact::type, sizeof(new_contact::type), "mobile");
							memset(new_contact::number, 0, sizeof(new_contact::number));
							new_contact::phone_numbers_type.clear();
							new_contact::phone_numbers.clear();
							new_contact::phone_number_c = 1;
							new_contact::email_c = 1;
							new_contact::emails.clear();
							memset(new_contact::email, 0, sizeof(new_contact::email));
							adding_contact = true;
						}
					} else {
						adding_contact = AddContactDialog(db);
					}
				}

				std::int32_t current_contact = -1;
				void ContactHeader(Contact contact) {
					if (ImGui::Button("<----")) {
						current_contact = -1;
					}
					auto window = ImGui::GetWindowDrawList();
					ui::Tooltip("Go back to the contact list");

					int textsize = std::string(contact.first_name).size() + std::string(contact.last_name).size();
					float font_size = ImGui::GetFontSize() * textsize / 2;
					ImGui::SameLine(
						ImGui::GetWindowSize().x / 2 -
						font_size + (font_size / 2)
						);
					ImGui::Text("%s %s", contact.first_name, contact.last_name);
				}

				void ViewContact(Contact contact) {
					ContactHeader(contact);
					ImGui::Separator();
					if (contact.phone_numbers.size() > 1) {
						if (ImGui::CollapsingHeader("phone numbers")) {
							for (auto&& phone_number : contact.phone_numbers) {
								if (phone_number.type != std::string("") && phone_number.number != std::string(""))
									ImGui::Text("%s: %s", phone_number.type, phone_number.number);
							}
						}
						ui::Tooltip("See phone numbers");
					} else {
						ImGui::Text("%s: %s", contact.phone_numbers[0].type, contact.phone_numbers[0].number);
					}
					ImGui::Separator();
					if (contact.emails.size() > 1) {
						if (ImGui::CollapsingHeader("emails")) {
							for (auto&& email : contact.emails) {
								if (email != std::string(""))
									ImGui::Text("%s", email);
							}
							ImGui::Separator();
						}
						ui::Tooltip("See emails");
					} else {
						ImGui::Text("email: %s", contact.emails[0]);
					}
				}

				void ListContactsWidget(std::vector<std::string> contacts) {
					if (ImGui::BeginChild("##contactschild", ImVec2(0,0), true)) {
						if (current_contact == -1) {
							for (int i = 0; i < contacts.size(); i++) {
								rapidjson::Document json;
								json.Parse(contacts[i].c_str());
								Contact contact(json);
								ImGui::Text("%s %s", contact.first_name, contact.last_name);
								ImGui::SameLine(ImGui::GetWindowWidth()-110); // right align button
								std::string button_name = "View Contact";
								button_name.append("##" + std::to_string(i));
								ImGui::Button(button_name.c_str());
								if (ImGui::IsItemActive()) {
									printf("%d\n", i);
									current_contact = i;
								}
								ImGui::Separator();
							}
						} else {
							rapidjson::Document json;
							json.Parse(contacts[current_contact].c_str());
							ViewContact(Contact(json));
						}
						ImGui::EndChild();
					}
				}

			}

			void Render(SortBy* sort, simple_db::TableSession* db) {
				auto contacts = SortContacts(sort, db);

				AddContactWidget(db);
				ImGui::Separator();
				SortWidget(sort);
				ListContactsWidget(contacts);
			}
		} // contacts
	} // tabs
} // contacts