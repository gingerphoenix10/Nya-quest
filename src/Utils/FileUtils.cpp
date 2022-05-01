#include "Utils/FileUtils.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
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
                if (
                    findCaseInsensitive(entry.path(), ".png") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".jpg") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".jpeg") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".webp") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".tiff") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".bmp") != std::string::npos ||
                    findCaseInsensitive(entry.path(), ".gif") != std::string::npos
                ) {
                    strings.push_back(entry.path());
                } 
               
            }
        }
        return strings;
    }

}