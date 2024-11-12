/* Copyright 2023 Dynatrace LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <boost/json.hpp>
#include <string_view>
#include <chrono>

namespace boost::json {
	  template<typename T>
    void tag_invoke(json::value_from_tag, json::value& v, std::reference_wrapper<T> const& wrapped) {
        v = json::value_from(wrapped.get());
    }

	//TODO: Properly implement conversion
 	  void tag_invoke(json::value_from_tag, json::value& v, std::chrono::time_point<std::chrono::steady_clock> time) {
        v = json::value_from(1);
    }
} // namespace boost::json


namespace boost::json::ext {

void remove_empty_keys(boost::json::object& obj) {
    for (auto it = obj.begin(); it != obj.end(); ) {
        if (it->value().is_string() && it->value().get_string().empty()) {
            it = obj.erase(it); // Erase returns the next iterator
        } else {
            ++it; // Move to the next element
        }
    }
}

void remove_empty_keys(boost::json::value& val)
{
  if(val.is_array()){
    auto &arr = val.get_array();
    for(auto &e : arr){
      remove_empty_keys(e);
    }
  } else if(val.is_object()){
    remove_empty_keys(val.get_object());
  }
  
}



} // boost::json::ext

template<typename T>
boost::json::object toJson(std::string_view key, T convertibleValue) {
	boost::json::object outJson{};
	outJson[key] = boost::json::value_from(convertibleValue);
	return outJson; 
}
