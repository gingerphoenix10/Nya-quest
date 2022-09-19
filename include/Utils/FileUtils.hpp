#pragma once
#include <string>
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace FileUtils {
    bool deleteFile(StringW path);
    void moveFile(StringW oldPath,StringW newPath);
    StringW GetFileFormat(StringW path);

    std::string FixIlegalName(std::string_view path);
    std::vector<std::string> getAllFoldersInFolder(const std::string& path);
    std::vector<std::string> getAllFilesInFolder(const std::string& path);
}