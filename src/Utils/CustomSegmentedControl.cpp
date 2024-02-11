#include "Utils/CustomSegmentedControl.hpp"

#include "main.hpp"
#include "string"
using namespace UnityEngine;

namespace Nya::Utils
{

    NyaUI::CustomTextSegmentedControlData *CreateTextSegmentedControl(UnityEngine::Transform *parent, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, ArrayW<StringW> values, std::function<void(int)> onCellWithIdxClicked)
    {
        static SafePtrUnity<HMUI::TextSegmentedControl> segmentedControlTemplate;
        if (!segmentedControlTemplate)
        {
            segmentedControlTemplate = Resources::FindObjectsOfTypeAll<HMUI::TextSegmentedControl *>()->First([](auto x)
                                                                                                       {
                // INFO: This selector could break in any new update. If you came here, try to modify the selection
                if (x->get_name() != "TextSegmentedControl") return false;
                auto parent = x->get_transform()->get_parent();
                if (!parent) return false;
                auto parentName = parent->get_name();
                // Oculus PC settings menu seems pog. Lets me fix my view
                return parentName == "GameplaySetupViewController" || parentName == "BaseGameplaySetupWrapper" || parentName == "OculusPCSettingsMenu"; });
        }

        auto segmentedControlObj = Object::Instantiate(segmentedControlTemplate->get_gameObject(), parent, false);
        segmentedControlObj->SetActive(false);
        static ConstString NyaUITextSegmentedControl("NyaUITextSegmentedControl");
        segmentedControlObj->set_name(NyaUITextSegmentedControl);
        auto rectTransform = reinterpret_cast<RectTransform *>(segmentedControlObj->get_transform());
        rectTransform->set_sizeDelta(sizeDelta);
        rectTransform->set_anchoredPosition(anchoredPosition);

        Object::DestroyImmediate(segmentedControlObj->GetComponent<HMUI::TextSegmentedControl *>());
        auto control = segmentedControlObj->AddComponent<HMUI::SegmentedControl *>();
        auto result = segmentedControlObj->AddComponent<NyaUI::CustomTextSegmentedControlData *>();

        result->firstCellPrefab = segmentedControlTemplate->_firstCellPrefab;
        result->lastCellPrefab = segmentedControlTemplate->_lastCellPrefab;
        result->middleCellPrefab = segmentedControlTemplate->_middleCellPrefab;
        result->singleCellPrefab = segmentedControlTemplate->_singleCellPrefab;

        result->segmentedControl = control;
        control->dataSource = result.cast<HMUI::SegmentedControl::IDataSource>();

        if (onCellWithIdxClicked)
        {
            using DelegateType = System::Action_2<HMUI::SegmentedControl *, int> *;
            std::function<void(HMUI::SegmentedControl *, int)> fun = [onCellWithIdxClicked](HMUI::SegmentedControl *cell, int idx)
            { onCellWithIdxClicked(idx); };
            auto delegate = custom_types::MakeDelegate<DelegateType>(fun);
            // auto delegate = MakeDelegate(DelegateType, fun);
            control->add_didSelectCellEvent(delegate);
        }

        int childCount = result->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            Object::DestroyImmediate(result->get_transform()->GetChild(0)->get_gameObject());
        }

        result->set_texts(values);

        segmentedControlObj->SetActive(true);
        return result;
    }

}