#pragma once
#include <string>

namespace FileUtils {

    std::string FixIlegalName(std::string_view path);
    std::vector<std::string> getAllFoldersInFolder(const std::string& path);
    std::vector<std::string> getAllFilesInFolder(const std::string& path);
}