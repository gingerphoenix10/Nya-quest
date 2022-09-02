#pragma once
#include <string>

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "main.hpp"


namespace Nya::Utils {
    List<StringW>* vectorToList(std::vector<StringW> values);
    std::vector<StringW> listWToVector(List<StringW>* values);
    int findStrIndexInList(List<StringW>* values, StringW string );
    // Kill me
    int findStrIndexInListC(std::list<std::string> values, StringW string );
    int random(int min, int max);
    ListWrapper<StringW> listStringToStringW(std::list<std::string> values);
}

