#pragma once
#include "NyaConfig.hpp"
#include "main.hpp"


#include <string>
#include <map>
#include <list>
#include <vector>

#include <iostream>

enum DataMode
{
    Unsupported,
    Json,
    Local
};

struct SourceData
{
    // SourceData(
    //     std::string i,
    //     DataMode j,
    //     std::list<std::string> k,
    //     std::list<std::string> l) {
    //     BaseEndpoint = i;
    //     Mode = j;
    //     SfwEndpoints = k;
    //     NsfwEndpoints = l;
    // };
    std::string BaseEndpoint;
    DataMode Mode;
    std::list<std::string> SfwEndpoints;
    std::list<std::string> NsfwEndpoints;
};


namespace NyaAPI {
    // Function gets url for the current selected category
    SourceData* get_data_source(std::string name);
    std::vector<StringW> get_source_list();
    void get_path_from_api(std::string url,float timeoutInSeconds,std::function<void(bool success, std::string url)> finished);
    // void NyaAPI::downloadImageFile();
    std::map<std::string, SourceData>* getEndpoints();
}