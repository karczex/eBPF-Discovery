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

class JsonTest : public testing::Test {};

//TODO: Apply changes in test

struct testClass {
	std::string str = "foo";
	std::string empty = "";
};

BOOST_DESCRIBE_STRUCT(testClass, (), (str, empty))

TEST_F(JsonTest, removeEmptyKeys){
	
	std::vector<testClass> vtc(4, {"bar", ""});
	boost::json::object json{{"key", boost::json::value_from(vtc)}};

	boost::json::ext::remove_empty_keys(json);
	
	std::stringstream buffer;
	buffer << json;	  
	
	const std::string expected{"\"key\":[{\"str\":\"bar\"},{\"str\":\"bar\"},{\"str\":\"bar\"},{\"str\":\"bar\"}]"};

	EXPECT_EQ(buffer.str(), expected);
} 


TEST_F(JsonTest, servicesToJson) {

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

	boost::json::object outJson{{"service", boost::json::value_from(internalServices)}};
	boost::json::ext::remove_empty_keys(outJson);

	std::stringstream buffer;
	buffer << outJson;
	// clang-format off
	const std::string expected{"{\"service\":["
		"{\"pid\":1,\"endpoint\":\"/endpoint/1\",\"externalClientsNumber\":2,\"internalClientsNumber\":1},"
		"{\"pid\":2,\"endpoint\":\"/endpoint/1\",\"externalClientsNumber\":2,\"internalClientsNumber\":1},"
		"{\"pid\":3,\"endpoint\":\"/endpoint/2\",\"externalClientsNumber\":2,\"internalClientsNumber\":1},"
		"{\"pid\":4,\"endpoint\":\"google.com/endpoint/3\",\"domain\":\"google.com\",\"scheme\":\"http\",\"internalClientsNumber\":1,\"externalClientsNumber\":2},"
		"{\"pid\":5,\"endpoint\":\"dynatrace.com/endpoint/4\",\"domain\":\"dynatrace.com\",\"scheme\":\"https\",\"internalClientsNumber\":1,\"externalClientsNumber\":2}]}"};
	// clang-format on
	EXPECT_EQ(buffer.str(), expected);
}


TEST_F(JsonTest, servicesToJsonNetworkCounters) {
	std::unordered_map<uint32_t, std::chrono::time_point<std::chrono::steady_clock>> detectedExternalIPv416Networks = {
			{0xAC8F, std::chrono::steady_clock::now()},
			{0xACC7, std::chrono::steady_clock::now()}
	};
	std::unordered_map<uint32_t, std::chrono::time_point<std::chrono::steady_clock>> detectedExternalIPv424Networks = {
			{0xAC8F04, std::chrono::steady_clock::now()},
			{0xAC8F06, std::chrono::steady_clock::now()},
			{0xACC72D, std::chrono::steady_clock::now()}
	};
	std::unordered_map<std::array<uint8_t, service::ipv6NetworkPrefixBytesLen>, std::chrono::time_point<std::chrono::steady_clock>, service::ArrayHasher> detectedExternalIPv6Networks = {
			{{0x20, 0x01, 0x48, 0x60, 0x48, 0x60}, std::chrono::steady_clock::now()},
			{{0x12, 0x34, 0x23, 0x45, 0x34, 0x56}, std::chrono::steady_clock::now()}
	};

	std::vector<std::reference_wrapper<service::Service>> internalServices;
	service::Service service1{.pid = 1, .endpoint = "/endpoint/1", .internalClientsNumber = 1, .externalClientsNumber = 3, .detectedExternalIPv4_16Networks = detectedExternalIPv416Networks, .detectedExternalIPv4_24Networks = detectedExternalIPv424Networks};
	service::Service service2{.pid = 2, .endpoint = "/endpoint/1", .internalClientsNumber = 1, .externalClientsNumber = 3, .detectedExternalIPv4_16Networks = detectedExternalIPv416Networks, .detectedExternalIPv4_24Networks = detectedExternalIPv424Networks};
	service::Service service3{.pid = 3, .endpoint = "/endpoint/2", .internalClientsNumber = 1, .externalClientsNumber = 3, .detectedExternalIPv4_16Networks = detectedExternalIPv416Networks, .detectedExternalIPv4_24Networks = detectedExternalIPv424Networks};

	service::Service service4{.pid = 4, .endpoint = "google.com/endpoint/3", .domain = "google.com", .scheme = "http", .internalClientsNumber = 1, .externalClientsNumber = 2, .detectedExternalIPv6Networks = detectedExternalIPv6Networks};
	service::Service service5{.pid = 5, .endpoint = "dynatrace.com/endpoint/4", .domain = "dynatrace.com", .scheme = "https", .internalClientsNumber = 1, .externalClientsNumber = 2, .detectedExternalIPv6Networks = detectedExternalIPv6Networks};

	internalServices.emplace_back(service1);
	internalServices.emplace_back(service2);
	internalServices.emplace_back(service3);
	internalServices.emplace_back(service4);
	internalServices.emplace_back(service5);

	boost::json::object outJson{{"service", boost::json::value_from(internalServices)}};
	boost::json::ext::remove_empty_keys(outJson);
	
	std::stringstream result;
	result << outJson;

	const std::string expected{"{\"service\":[{\"pid\":1,\"endpoint\":\"/endpoint/"
							   "1\",\"internalClientsNumber\":1,\"externalClientsNumber\":3,\"externalIPv4_16ClientNets\":2,\"externalIPv4_24ClientNets\":3},{\"pid\":2,\"endpoint\":\"/endpoint/"
							   "1\",\"internalClientsNumber\":1,\"externalClientsNumber\":3,\"externalIPv4_16ClientNets\":2,\"externalIPv4_24ClientNets\":3},{\"pid\":3,\"endpoint\":\"/endpoint/"
							   "2\",\"internalClientsNumber\":1,\"externalClientsNumber\":3,\"externalIPv4_16ClientNets\":2,\"externalIPv4_24ClientNets\":3},{\"pid\":4,\"endpoint\":\"google.com/"
							   "endpoint/3\",\"domain\":\"google.com\",\"scheme\":\"http\",\"internalClientsNumber\":1,\"externalClientsNumber\":2,\"externalIPv6ClientsNets\":2},{\"pid\":5,\"endpoint\":\"dynatrace.com/"
							   "endpoint/4\",\"domain\":\"dynatrace.com\",\"scheme\":\"https\",\"internalClientsNumber\":1,\"externalClientsNumber\":2,\"externalIPv6ClientsNets\":2}]}"};
	EXPECT_EQ(result.str(), expected);
}
