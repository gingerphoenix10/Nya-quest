#pragma once

#include "HMUI/TextSegmentedControl.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "components/CustomTextSegmentedControlData.hpp"
#include "custom-types/shared/delegate.hpp"
#include "System/Action_2.hpp"
using namespace UnityEngine;


namespace Nya::Utils {
    NyaUI::CustomTextSegmentedControlData* CreateTextSegmentedControl(UnityEngine::Transform* parent, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, ArrayW<StringW> values, std::function<void(int)> onCellWithIdxClicked);
}