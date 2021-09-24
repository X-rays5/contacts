//
// Created by X-ray on 9/24/2021.
//

#pragma once

#ifndef CONTACTS_JSON_HPP
#define CONTACTS_JSON_HPP
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace contacts {
	namespace json {
		typedef rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> rapidjson_object_t;

		inline std::string stringify(rapidjson::Document& json) {
			rapidjson::StringBuffer strbuf;
			strbuf.Clear();

			rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
			json.Accept(writer);

			return strbuf.GetString();
		}

		inline const char* GetString(rapidjson::Document& doc, const char* key) {
			if (doc.IsObject())
				if (doc.HasMember(key))
					if (doc[key].IsString())
						return doc[key].GetString();

			return "NULL";
		}

		inline const char* GetString(rapidjson_object_t& doc, const char* key) {
			if (doc.HasMember(key))
				if (doc[key].IsString())
					return doc[key].GetString();

			return "NULL";
		}

		inline std::vector<rapidjson_object_t> ArrayToVector(rapidjson::Document& doc, const char* key) {
			std::vector<rapidjson_object_t> res;

			if (doc.IsObject()) {
				if (doc.HasMember(key)) {
					if (doc[key].IsArray()) {
						auto arr = doc[key].GetArray();

						for (auto&& member : arr) {
							res.emplace_back(member.GetObject());
						}
					}
				}
			}

			return res;
		}
	} // json
} // contacts
#endif //CONTACTS_JSON_HPP
