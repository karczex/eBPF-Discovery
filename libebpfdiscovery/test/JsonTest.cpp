/*
 * Copyright 2023 Dynatrace LLC
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

#include "ebpfdiscovery/Json.h"
#include <gtest/gtest.h>

#include "service/Service.h"
#include <iostream>
#include <string>
#include <vector>

#include <boost/describe.hpp>

class Json : public testing::Test {};


//TODO: Apply changes in test

struct testClass {
	std::string str = "foo";
	std::string empty = "";
};

BOOST_DESCRIBE_STRUCT(testClass, (), (str, empty))

TEST_F(Json, filterEmptyKeys){
	
	std::vector<testClass> vtc(4, {"bar", ""});
	 auto json = boost::json::value_from(vtc);
	boost::json::ext::remove_empty_keys(json);
	
	std::stringstream buffer;
	buffer << json;	  
	
	const std::string expected{"[{\"str\":\"bar\"},{\"str\":\"bar\"},{\"str\":\"bar\"},{\"str\":\"bar\"}]"};

	EXPECT_EQ(buffer.str(), expected);
} 


TEST_F(Json, servicesToJson) {

	std::vector<std::reference_wrapper<service::Service>> internalServices;
	service::Service service1{.pid = 1, .endpoint = "/endpoint/1", .internalClientsNumber = 1, .externalClientsNumber = 2};
	service::Service service2{.pid = 2, .endpoint = "/endpoint/1", .internalClientsNumber = 1, .externalClientsNumber = 2};
	service::Service service3{.pid = 3, .endpoint = "/endpoint/2", .internalClientsNumber = 1, .externalClientsNumber = 2};

	service::Service service4{.pid = 4, .endpoint = "google.com/endpoint/3", .domain = "google.com", .scheme = "http", .internalClientsNumber = 1, .externalClientsNumber = 2};
	service::Service service5{.pid = 5, .endpoint = "dynatrace.com/endpoint/4", .domain = "dynatrace.com", .scheme = "https", .internalClientsNumber = 1, .externalClientsNumber = 2};

	internalServices.push_back(service1);
	internalServices.push_back(service2);
	internalServices.push_back(service3);
	internalServices.push_back(service4);
	internalServices.push_back(service5);

	boost::json::object outJson{};
	outJson["service"] = boost::json::value_from(internalServices);

	std::stringstream buffer;
	buffer << outJson;
	// clang-format off
	const std::string expected{"{\"service\":["
		"{\"pid\":1,\"endpoint\":\"/endpoint/1\",\"domain\":\"\",\"scheme\":\"\",\"internalClientsNumber\":1,\"externalClientsNumber\":2},"
		"{\"pid\":2,\"endpoint\":\"/endpoint/1\",\"domain\":\"\",\"scheme\":\"\",\"internalClientsNumber\":1,\"externalClientsNumber\":2},"
		"{\"pid\":3,\"endpoint\":\"/endpoint/2\",\"domain\":\"\",\"scheme\":\"\",\"internalClientsNumber\":1,\"externalClientsNumber\":2},"
		"{\"pid\":4,\"endpoint\":\"google.com/endpoint/3\",\"domain\":\"google.com\",\"scheme\":\"http\",\"internalClientsNumber\":1,\"externalClientsNumber\":2},"
		"{\"pid\":5,\"endpoint\":\"dynatrace.com/endpoint/4\",\"domain\":\"dynatrace.com\",\"scheme\":\"https\",\"internalClientsNumber\":1,\"externalClientsNumber\":2}]}"};
	// clang-format on
	EXPECT_EQ(buffer.str(), expected);
}
