
#include "NyaConfig.hpp"
#include "main.hpp"


namespace NyaAPI {
    // Function gets url for the current selected category
    std::string get_api_path();
    void get_path_from_api(std::string url,float timeoutInSeconds,std::function<void(bool success, std::string url)> finished);
}