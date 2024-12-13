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
#include <unordered_map>

namespace boost::json {
	  template<typename T>
    void tag_invoke(json::value_from_tag, json::value& v, std::reference_wrapper<T> const& wrapped) {
        v = json::value_from(wrapped.get());
    }
} // namespace boost::json


namespace boost::json::ext {

void
pretty_print( std::ostream& os, boost::json::value const& jv)
{
    switch(jv.kind())
    {
    case json::kind::object:
    {
        auto const& obj = jv.get_object();
        if(! obj.empty())
        {
            os << "{";
            for(auto it = obj.begin(); it != obj.end(); )
            { 
                auto next = it++;
                auto val = it->value();
                if( val.is_null() || (val.is_string() && (val.get_string().size() == 0))) {
                  ++it;
                  continue;
                } 
                os << json::serialize(it->key()) << ":";
                pretty_print(os, val);
                if(++it ==obj.end()){
                    continue;
                }
                os << ",";
            }
            os << "}";
        }
        break;
    }

    case json::kind::array:
    {
        auto const& arr = jv.get_array();
        if(! arr.empty())
        {
            os << "[";
            auto it = arr.begin();
            for(auto it = arr.begin(); it != arr.end();)
            {
                pretty_print(os, *it);
                if(++it != arr.end()){
                    os << ",";
                }
            }
        }
        os << "]";
        break;
    }
    default:
        os << jv;
    }

}
} //boost::json::ext

template<typename T>
boost::json::object toJson(std::string_view key, T convertibleValue) {
	boost::json::object outJson{};
	outJson[key] = boost::json::value_from(convertibleValue);
	return outJson; 
}
