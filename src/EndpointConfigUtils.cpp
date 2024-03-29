#include "EndpointConfigUtils.hpp"
#include "logging.hpp"

using namespace Nya;

namespace EndpointConfigUtils {

    // Gets endpoint from the config
    std::string getEndpointValue(std::string name, bool nsfw) {
        auto& config = getNyaConfig();
        
        auto endpointsConfig = config.endpoints.GetValue();

        // Early return if no config is found
        if (!endpointsConfig.contains(name)) {
            auto source_data = NyaAPI::get_data_source(name);
            std::vector<NyaAPI::EndpointCategory>* endpoint_data = nsfw ? &source_data->NsfwEndpoints : &source_data->SfwEndpoints;
            return endpoint_data->size() > 0 ? endpoint_data->front().url: "";
        } 
        
        // Get the endpoint config
        EndpointConfig cnf = endpointsConfig[name];
            
        // Check if it's sfw or nsfw and get appropriate member
        std::optional<std::string> member = nsfw ? cnf.nsfw : cnf.sfw;

        // If the value is present, return it, otherwise return the first endpoint
        if (member.has_value()) {
            return member.value();
        } 

        // Otherwise return the first endpoint
        auto source_data = NyaAPI::get_data_source(name);

        std::vector<NyaAPI::EndpointCategory>* endpoint_data = nsfw ? &source_data->NsfwEndpoints : &source_data->SfwEndpoints;
        return endpoint_data->size() > 0 ? endpoint_data->front().url: "";
    }

    /// @brief Updates API config value
    /// @param name name of the selected API
    /// @param nsfw is nsfw or no
    /// @param endpointUrl endpoint category url chunk
    void updateEndpointValue(std::string name, bool nsfw, std::string endpointUrl) {
        auto& config = getNyaConfig();
        
        auto endpointsConfig = config.endpoints.GetValue();

        // Value to store the endpoint config
        EndpointConfig cnf = EndpointConfig(std::nullopt,std::nullopt);

        // If the source setting key does not exist, create it
        if (endpointsConfig.contains(name)) {
            // Get existing config
            cnf = endpointsConfig[name];
        }

        if (nsfw) {
            cnf.nsfw = endpointUrl;
        } else {
            cnf.sfw = endpointUrl;
        }

        // Save the endpoint to appropriate slot
        endpointsConfig[name] = cnf;

        // Save the config
        config.endpoints.SetValue(endpointsConfig);
    }

    void ResetPositions() {
        INFO("Resetting configs");
        NyaConfig_t & config = getNyaConfig();
        // Positions
        config.pausePosition.SetValue(config.pausePosition.GetDefaultValue());
        config.menuPosition.SetValue(config.menuPosition.GetDefaultValue());

        // Rotations
        config.pauseRotation.SetValue(config.pauseRotation.GetDefaultValue());
        config.menuRotation.SetValue(config.menuRotation.GetDefaultValue());

    }
}