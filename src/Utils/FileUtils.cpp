
#include "Utils/FileUtils.hpp"
#include "Utils/Utils.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "beatsaber-hook/shared/utils/typedefs.h"
namespace FileUtils {

    std::string FixIlegalName(std::string_view path) {
        static const std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()[]{}%&.:,;=!-_ ";
        std::string newPath;
        newPath.reserve(path.size());  
        std::copy_if(path.cbegin(), path.cend(), std::back_inserter(newPath), 
            [] (char c) { 
                return allowedChars.find(c) != std::string::npos;
            }
        );
        return newPath;
    }

     std::vector<std::string> getAllFoldersInFolder(const std::string& path) {
        std::vector<std::string> strings;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                strings.push_back(entry.path());
            }
        }
        return strings;
    }


    /*
    * Find Case Insensitive Sub String in a given substring
    */
    size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0)
    {
        // Convert complete given String to lower case
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        // Convert complete given Sub String to lower case
        std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
        // Find sub string in given string
        return data.find(toSearch, pos);
    }

    // Gets all supported files
    std::vector<std::string> getAllFilesInFolder(const std::string& path){
         std::vector<std::string> strings;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                StringW path = StringW(entry.path().c_str());
                if (Nya::Utils::IsImage(path)) {
                    strings.push_back(entry.path());
                }            
            }
        }
        return strings;
    }

}