#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UI/ModalHelper.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/ModalView.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "UnityEngine/Space.hpp"
#include "main.hpp"

DEFINE_TYPE(Nya, ModalHelper);

void Nya::ModalHelper::Awake(){
    isHit = false;
    hintController = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::HoverHintController*>());
    hint = nullptr;
}

void Nya::ModalHelper::Init(VRUIControls::VRPointer* pointer){
    vrPointer = pointer;
}

void Nya::ModalHelper::Update(){
    if(UnityEngine::Physics::Raycast(vrPointer->get_vrController()->get_position(), vrPointer->get_vrController()->get_forward(), hit, 100)){
        if(static_cast<std::string>(hit.get_collider()->get_name()).compare("modalcollider") == 0 && !Nya::Main::NyaFloatingUI->hoverClickHelper->grabbingHandle){
            if (!isHit){
                hint = hit.get_collider()->get_transform()->get_parent()->GetComponentInChildren<HMUI::HoverHint*>();
                if (Nya::Main::NyaFloatingUI->modal->dyn__isShown()){
                    isHit = true;
                    hintController->SetupAndShowHintPanel(hint);
                    hint->set_enabled(true);
                    hint->get_gameObject()->set_active(true);
                    hintController->dyn__hoverHintPanel()->get_transform()->SetParent(Nya::Main::NyaFloatingUI->modal->get_transform(), false);
                    hintController->dyn__hoverHintPanel()->get_transform()->set_localScale({0.6f, 0.6f, 0.0f});
                    hintController->dyn__hoverHintPanel()->get_transform()->set_position(hint->get_transform()->get_position());
                    hintController->dyn__hoverHintPanel()->get_transform()->Translate({0.0f, 0.21f, 0.0f}, UnityEngine::Space::Self);
                }
            }
        }
    }
}

void Nya::ModalHelper::LateUpdate(){
    if (isHit && (!hit.get_collider() || static_cast<std::string>(hit.get_collider()->get_name()).compare("modalcollider") != 0)){
        isHit = false;
        hintController->dyn__hoverHintPanel()->Hide();
        hint = nullptr;
    }
}

Nya::ModalHelper* Nya::addModalHelper(VRUIControls::VRPointer* pointer, QuestUI::FloatingScreen* screen){
    auto* helper = screen->get_gameObject()->AddComponent<Nya::ModalHelper*>();
    helper->Init(pointer);
    return helper;
}