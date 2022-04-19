#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UI/HoverClickHelper.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/ModalView.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "UnityEngine/Shader.hpp"
#include "NyaFloatingUI.hpp"
#include "UnityEngine/Space.hpp"
#include "main.hpp"

DEFINE_TYPE(Nya, HoverClickHelper);

void Nya::HoverClickHelper::Awake(){
    isHit = false;
    hintController = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::HoverHintController*>());
    panelUI = nullptr;
    notClickedModal = true;
    outOfRange = false;
    currentCollider = nullptr;
    grabbingHandle = false;
    hoveringHandle = false;
    hoverHandleMat = UnityEngine::Material::New_ctor(UnityEngine::Shader::Find("Hidden/Internal-DepthNormalsTexture"));
}

void Nya::HoverClickHelper::Init(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle){
    vrPointer = pointer;
    handleTransform = handle;
    origHandleMat = handle->GetComponent<UnityEngine::MeshRenderer*>()->get_material();
}

void Nya::HoverClickHelper::Update(){
    if(UnityEngine::Physics::Raycast(vrPointer->get_vrController()->get_position(), vrPointer->get_vrController()->get_forward(), hit, 100)){
        if(static_cast<std::string>(hit.get_collider()->get_name()).substr(0, 12).compare("gridcollider") == 0 && !grabbingHandle){
            if (isHit && currentCollider && currentCollider != hit.get_collider()->get_transform()){
                panelUI->image->GetComponent<UnityEngine::UI::Image*>()->set_color(UnityEngine::Color::get_gray());
                hintController->dyn__hoverHintPanel()->Hide();
                panelUI = nullptr;
                isHit = false;
            }
            if (!isHit){
//                 panelUI = Nya::Main::NyaFloatingUI->panelImages[std::stoi(static_cast<std::string>(hit.get_collider()->get_name()).substr(13, 2))];
//                 currentCollider = hit.get_collider()->get_transform();
//                 if (!Nya::Main::NyaFloatingUI->settingsModal->dyn__isShown()){
//                     isHit = true;
//                     panelUI->image->set_color(UnityEngine::Color(0.70f, 0.70f, 0.70f, 1.0f));
//                     hintController->SetupAndShowHintPanel(panelUI->hoverHint);
//                     panelUI->hoverHint->set_enabled(true);
//                     panelUI->hoverHint->get_gameObject()->set_active(true);
//                     hintController->dyn__hoverHintPanel()->get_transform()->set_localScale({0.6f, 0.6f, 0.0f});
//                     hintController->dyn__hoverHintPanel()->get_transform()->set_position(panelUI->hoverHint->get_transform()->get_position());
//                     hintController->dyn__hoverHintPanel()->get_transform()->Translate({0.0f, 0.22f, 0.0f}, UnityEngine::Space::Self);
//                 }
//                 else panelUI = nullptr;
            }
        }
        else if (isHit){
            panelUI->image->set_color(UnityEngine::Color::get_gray());
            hintController->dyn__hoverHintPanel()->Hide();
            panelUI = nullptr;
            isHit = false;
        }
        if(hit.get_collider()->get_transform() == handleTransform->get_transform()){
            if(!hoveringHandle){
                hoveringHandle = true;
                handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(hoverHandleMat);
            }
        }
        else if(hoveringHandle && !grabbingHandle){
            hoveringHandle = false;
            handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(origHandleMat);
        }
    }
    else {
        if (isHit && !Nya::Main::NyaFloatingUI->settingsModal->dyn__isShown()){
            panelUI->image->GetComponent<UnityEngine::UI::Image*>()->set_color(UnityEngine::Color::get_gray());
            hintController->dyn__hoverHintPanel()->Hide();
            panelUI = nullptr;
            isHit = false;
        }
        if(hoveringHandle && !grabbingHandle){
            hoveringHandle = false;
            handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(origHandleMat);
        }
    }
    if (isHit && grabbingController && !modalLocked && notClickedModal && !justClosedModal && !outOfRange){
        grabbingController = vrPointer->get_vrController();
        triggerPressed = true;
        notClickedModal = false;
        panelUI->image->set_color(UnityEngine::Color::get_gray());
        // Nya::Main::NyaFloatingUI->updateGridNotesInfo(panelUI->index);
        Nya::Main::NyaFloatingUI->settingsModal->Show(true, true, nullptr);
        hintController->dyn__hoverHintPanel()->Hide();
        panelUI = nullptr;
        isHit = false;
        modalLocked = true;
    }
    if (!Nya::Main::NyaFloatingUI->settingsModal->dyn__isShown() && !modalLocked && !notClickedModal) notClickedModal = true;
    if (justClosedModal) modalLocked = false;
    if (grabbingController) outOfRange = true;
}

void Nya::HoverClickHelper::LateUpdate(){
    if(!triggerPressed && !isHit && vrPointer->get_vrController()->get_triggerValue() > 0.9f && !(hoveringHandle || grabbingHandle)){
        Nya::Main::NyaFloatingUI->settingsModal->Hide(true, nullptr);
        justClosedModal = true;
    }
    if (vrPointer->get_vrController()->get_triggerValue() > 0.9f && !triggerPressed){
        grabbingController = vrPointer->get_vrController();
        triggerPressed = true;
        if ((hit.get_collider() && hit.get_collider()->get_transform() == handleTransform->get_transform())){
            handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(hoverHandleMat);
            grabbingHandle = true;
            hoveringHandle = true;
        }
    }
    if (triggerPressed && grabbingController->get_triggerValue() < 0.1f){
        if (grabbingHandle){
            auto* screenTransform = handleTransform->get_transform()->get_parent();
            if ((float)(screenTransform->get_position().y) < 0.5) screenTransform->Translate(0.0f, (0.5 - screenTransform->get_position().y), 0.0f, UnityEngine::Space::World);
            Nya::Main::NyaFloatingUI->updateCoordinates(screenTransform);
        }
        grabbingController = nullptr;
        triggerPressed = false;
        justClosedModal = false;
        outOfRange = false;
        grabbingHandle = false;
    } 
    if (isHit && Nya::Main::NyaFloatingUI->settingsModal->dyn__isShown()) isHit = false;
    
}

void Nya::HoverClickHelper::resetBools(){
    justClosedModal = false;
    notClickedModal = true;
    triggerPressed = false;
    modalLocked = false;
    grabbingController = nullptr;
    outOfRange = false;
}

Nya::HoverClickHelper* Nya::addHoverClickHelper(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle, QuestUI::FloatingScreen* screen){
    auto* helper = screen->get_gameObject()->AddComponent<Nya::HoverClickHelper*>();
    helper->Init(pointer, handle);
    return helper;
}