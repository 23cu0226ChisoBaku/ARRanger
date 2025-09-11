#include "ARRangerEditorModule.h"
#include "PropertyEditorModule.h"
#include "CustomDetails/RangeDetectorCustomization.h"

#include "CustomDetails/RangeDetectorTargetCustomization.h"

#include "RangeDetector/GameFramework/RangeDetectorComponent.h"
#include "CustomComponentVisualizer/ARRangeDetectorComponentVisualizer.h"

#include "UnrealEdGlobals.h"        // Access GUnrealEd
#include "Editor/UnrealEdEngine.h"

#include "IAnimationEditorModule.h"                   
#include "EditorViewportClient.h"
#include "AnimationEditorViewportClient.h"
#include "IPersonaPreviewScene.h"

IMPLEMENT_GAME_MODULE(FARRangerEditorModule, ARRangerEditor);

#define LOCTEXT_NAMESPACE "FARRangerEditorModule"

#define REGISTER_CUSTOM_COMPONENT_VISUALIZER(ActorComponentType, ComponentVisualizerSharedPtr) \
  this->RegisterCustomComponentVisualizer(ActorComponentType::StaticClass()->GetFName(), ComponentVisualizerSharedPtr)

namespace
{
  static const FName PROPERTY_EDITOR_NAME{"PropertyEditor"};
}

class FViewportClientExtension_RangeDetector : public FEditorViewportClient
{
  using Super = FEditorViewportClient;
  public:
    FViewportClientExtension_RangeDetector(FPreviewScene* InPreviewScene, const TSharedPtr<IPersonaPreviewScene>& InPersonaPreviewScene, const TSharedRef<SEditorViewport>& InEditorViewportWidget)
      : Super(nullptr, InPreviewScene, InEditorViewportWidget)
      , m_personaPreviewScene{InPersonaPreviewScene}
    {}

    virtual void Draw(const FSceneView* InView, FPrimitiveDrawInterface* PDI) override
    {
      Super::Draw(InView, PDI);

      if (m_personaPreviewScene.IsValid())
      {
        if (UDebugSkelMeshComponent* MeshComp = m_personaPreviewScene->GetPreviewMeshComponent())
        {
          TArray<USceneComponent*> children{};
          MeshComp->GetChildrenComponents(false, children);
          
          for (USceneComponent* child : children)
          {
            if (URangeDetectorComponent* RDC = ::Cast<URangeDetectorComponent>(child))
            {
              RDC->ED_DrawComponentVisualizer(PDI);
            }
          }
        }
      }
    }

  private:
    TSharedPtr<IPersonaPreviewScene> m_personaPreviewScene;
};

void FARRangerEditorModule::StartupModule()
{
  RegisterComponentVisualizers();

  RegisterPropertyTypeCustomizations();

  FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
  propertyModule.NotifyCustomizationModuleChanged();

  ExtendAnimEditor();

}

void FARRangerEditorModule::ShutdownModule()
{
  if (FModuleManager::Get().IsModuleLoaded(PROPERTY_EDITOR_NAME))
  {
    FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
    for (const auto& propertyTypeName : m_registeredPropertyTypes)
    {
      propertyModule.UnregisterCustomPropertyTypeLayout(propertyTypeName);
    }

    propertyModule.NotifyCustomizationModuleChanged();
  }


}

void FARRangerEditorModule::RegisterPropertyTypeCustomizations()
{
  RegisterCustomPropertyTypeLayout("DetectorTarget", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRangeDetectorTargetCustomization::MakeInstance));
}

void FARRangerEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
  check(PropertyTypeName != NAME_None);

  m_registeredPropertyTypes.Add(PropertyTypeName);
  FPropertyEditorModule& propertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PROPERTY_EDITOR_NAME);
  propertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

void FARRangerEditorModule::RegisterComponentVisualizers()
{
  REGISTER_CUSTOM_COMPONENT_VISUALIZER(URangeDetectorComponent, FARRangeDetectorComponentVisualizer::MakeInstance());
}

void FARRangerEditorModule::RegisterCustomComponentVisualizer(FName ClassName, TSharedPtr<FComponentVisualizer> VisualizerPtr)
{
  check(VisualizerPtr.IsValid());
  check(ClassName != NAME_None);

  if (GUnrealEd != nullptr)
  {
    m_registeredComponentVisualizerTypes.Add(ClassName);
    GUnrealEd->RegisterComponentVisualizer(ClassName, VisualizerPtr);
    VisualizerPtr->OnRegister();
  }
}

void FARRangerEditorModule::UnregisterComponentVisualizers()
{
  if (GUnrealEd != nullptr)
  {
    for (const FName& registeredName : m_registeredComponentVisualizerTypes)
    {
      GUnrealEd->UnregisterComponentVisualizer(registeredName);
    }
  }
}

void FARRangerEditorModule::ExtendAnimEditor()
{

}

#undef LOCTEXT_NAMESPACE
#undef REGISTER_CUSTOM_COMPONENT_VISUALIZER