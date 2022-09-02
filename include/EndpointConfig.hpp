#pragma once
#include "main.hpp"
#include "API.hpp"

namespace EndpointConfig {
    void migrate(Configuration* config);
    void updateEndpointValue(Configuration* config, std::string name, bool nsfw, std::string value);
    std::string getEndpointValue(Configuration* config, std::string name, bool nsfw);
}
