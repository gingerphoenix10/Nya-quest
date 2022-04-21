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

    std::vector<std::string> getAllFilesInFolder(const std::string& path){
         std::vector<std::string> strings;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                strings.push_back(entry.path());
            }
        }
        return strings;
    }

}