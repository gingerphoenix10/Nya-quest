#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Resources.hpp"
#include "UI/PanelUI.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/HoverHintController.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "VRUIControls/VRPointer.hpp"

DECLARE_CLASS_CODEGEN(Nya, HoverClickHelper, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_FIELD(UnityEngine::Vector3 , targetPosition);
    DECLARE_INSTANCE_FIELD(UnityEngine::Quaternion , targetRotation);
    DECLARE_INSTANCE_FIELD(VRUIControls::VRPointer*, vrPointer);
    DECLARE_INSTANCE_FIELD(UnityEngine::RaycastHit, hit);
    DECLARE_INSTANCE_FIELD(HMUI::HoverHintController*, hintController);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, handleTransform);
    DECLARE_INSTANCE_FIELD(bool, isHit);
    DECLARE_INSTANCE_FIELD(bool, justClosedModal);
    DECLARE_INSTANCE_FIELD(bool, notClickedModal);
    DECLARE_INSTANCE_FIELD(bool, outOfRange);
    DECLARE_INSTANCE_FIELD(bool, grabbingHandle);
    DECLARE_INSTANCE_FIELD(bool, hoveringHandle);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, origHandleMat);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, hoverHandleMat);
    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, currentCollider);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::VRController*, grabbingController);
    DECLARE_INSTANCE_FIELD(bool, triggerPressed);
    DECLARE_INSTANCE_FIELD(bool, modalLocked);

    DECLARE_INSTANCE_METHOD(void, Init, VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, LateUpdate);
    DECLARE_INSTANCE_METHOD(void, resetBools);

    DECLARE_INSTANCE_METHOD(void, LookAtCamera);
    DECLARE_INSTANCE_METHOD(void, SetUpRight);
    DECLARE_INSTANCE_METHOD(void, SetPosition, UnityEngine::Vector3 position,UnityEngine::Quaternion rotation, bool lerp = true);
    

    public:
        Nya::PanelUI* panelUI;
);

namespace Nya{
    Nya::HoverClickHelper* addHoverClickHelper(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle, QuestUI::FloatingScreen* screen);
}