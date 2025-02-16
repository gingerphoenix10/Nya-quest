#pragma once
#include <string>
namespace EndpointConfigUtils {
    void ResetPositions();
    void updateEndpointValue(std::string name, bool nsfw, std::string value);
    std::string getEndpointValue(std::string name, bool nsfw);
}
