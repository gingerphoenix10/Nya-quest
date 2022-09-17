#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UI/HoverClickHelper.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/ModalView.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "UnityEngine/Shader.hpp"
#include "NyaFloatingUI.hpp"
#include "UnityEngine/Space.hpp"
#include "main.hpp"

using namespace Nya;

DEFINE_TYPE(Nya, HoverClickHelper);

namespace Nya {
    
    void HoverClickHelper::Awake(){
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

    void HoverClickHelper::Init(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle){
        vrPointer = pointer;
        handleTransform = handle;
        origHandleMat = handle->GetComponent<UnityEngine::MeshRenderer*>()->get_material();
    }

    void HoverClickHelper::Update(){
        if(UnityEngine::Physics::Raycast(vrPointer->get_vrController()->get_position(), vrPointer->get_vrController()->get_forward(), hit, 100)){
            if(static_cast<std::string>(hit.get_collider()->get_name()).substr(0, 12).compare("gridcollider") == 0 && !grabbingHandle){
                if (isHit && currentCollider && currentCollider != hit.get_collider()->get_transform()) {
                    panelUI->image->GetComponent<UnityEngine::UI::Image *>()->set_color(UnityEngine::Color::get_gray());
                    hintController->hoverHintPanel->Hide();
                    panelUI = nullptr;
                    isHit = false;
                }
            }
            else if (isHit){
                panelUI->image->set_color(UnityEngine::Color::get_gray());
                hintController->hoverHintPanel->Hide();
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
            if (isHit && !Main::NyaFloatingUI->settingsMenu->isShown()){
                panelUI->image->GetComponent<UnityEngine::UI::Image*>()->set_color(UnityEngine::Color::get_gray());
                hintController->hoverHintPanel->Hide();
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
            // Main::NyaFloatingUI->updateGridNotesInfo(panelUI->index);
            Main::NyaFloatingUI->settingsMenu->Show();
            hintController->hoverHintPanel->Hide();
            panelUI = nullptr;
            isHit = false;
            modalLocked = true;
        }
        if (!Main::NyaFloatingUI->settingsMenu->isShown() && !modalLocked && !notClickedModal) notClickedModal = true;
        if (justClosedModal) modalLocked = false;
        if (grabbingController) outOfRange = true;
    }

    void HoverClickHelper::LateUpdate(){
        if(!triggerPressed && !isHit && vrPointer->get_vrController()->get_triggerValue() > 0.9f && !(hoveringHandle || grabbingHandle)){
            // TODO: Fix modal hiding
            //    Main::NyaFloatingUI->settingsMenu->Hide();
            //    justClosedModal = true;
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
    //        if (grabbingHandle){
    //            auto* screenTransform = handleTransform->get_transform()->get_parent();
    ////            if ((float)(screenTransform->get_position().y) < 0.5) screenTransform->Translate(0.0f, (0.5 - screenTransform->get_position().y), 0.0f, UnityEngine::Space::World);
    //            getLogger().debug("Saved coords");
    //
    //        }
            auto* screenTransform = handleTransform->get_transform()->get_parent();
            Main::NyaFloatingUI->updateCoordinates(screenTransform);
            grabbingController = nullptr;
            triggerPressed = false;
            justClosedModal = false;
            outOfRange = false;
            grabbingHandle = false;
        } 
        if (isHit && Main::NyaFloatingUI->settingsMenu->isShown()) isHit = false;
        
    }

    void HoverClickHelper::resetBools(){
        justClosedModal = false;
        notClickedModal = true;
        triggerPressed = false;
        modalLocked = false;
        grabbingController = nullptr;
        outOfRange = false;
    }

    HoverClickHelper* addHoverClickHelper(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle, QuestUI::FloatingScreen* screen){
        auto* helper = screen->get_gameObject()->AddComponent<HoverClickHelper*>();
        helper->Init(pointer, handle);
        return helper;
    }


    void HoverClickHelper::LookAtCamera(){
        auto mainCamera = UnityEngine::Camera::get_main();
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        auto newRotation = UnityEngine::Quaternion::LookRotation(screenTransform->get_position() - mainCamera->get_transform()->get_position());
        screenTransform->set_rotation(newRotation);
        Main::NyaFloatingUI->updateCoordinates(screenTransform);
    }

    void HoverClickHelper::SetUpRight (){
        auto mainCamera = UnityEngine::Camera::get_main();
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        auto newRotation = UnityEngine::Quaternion::Euler(0.0, screenTransform->get_rotation().get_eulerAngles().y, 0.0);
        screenTransform->set_rotation(newRotation);
        Main::NyaFloatingUI->updateCoordinates(screenTransform);
    }

    void HoverClickHelper::SetPosition(UnityEngine::Vector3 position, UnityEngine::Quaternion rotation){
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();

        screenTransform->set_position(position);
        screenTransform->set_rotation(rotation);
    }
}
