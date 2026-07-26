// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Engine/StreamableManager.h"
#include "GameplayTagContainer.h"
#include "PrimaryGameLayout.generated.h"

class UActivatableWidget;
class UCommonActivatableWidgetContainerBase;

/**
 * The state of an async load operation for the UI.
 */
enum class EAsyncWidgetLayerState : uint8
{
    Initialize,
    AfterPush,
    Canceled
};

/**
 * The primary game UI layout of your game. Created within ASeedHUD.
 * This widget class represents how to layout, push, and display all UI layers for a single player.
 *
 * Typically manages the following UI layers (defined in the concrete Widget Blueprint):
 * - Game: HUD elements
 * - GameMenu: Gameplay UI
 * - Menu: Settings and options
 * - Modal: Dialogs and popups
 *
 * Layer Behavior:
 * - Within a layer: Push/pop or push/deactivate operations. When new UI is pushed to a layer,
 *   it replaces the previous UI until deactivated (which pops it from the stack).
 * - Across layers: Each layer maintains a separate widget stack. When UI is added to a higher layer
 *   (e.g., Menu), all lower layers remain visible beneath it.
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class SEEDWORKUI_API UPrimaryGameLayout : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(
        FGameplayTag layerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UActivatableWidget> widgetClass)
    {
        return PushWidgetToLayerStackAsync(layerName, bSuspendInputUntilComplete, widgetClass, [](auto, auto) {});
    }

    TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(
        FGameplayTag layerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UActivatableWidget> widgetClass, TFunction<void(EAsyncWidgetLayerState, UActivatableWidget*)> stateFunc);

    UActivatableWidget* PushWidgetToLayerStack(
        FGameplayTag layerName, TSubclassOf<UActivatableWidget> widgetClass);

    UActivatableWidget* PushWidgetToLayerStack(
        FGameplayTag layerName, TSubclassOf<UActivatableWidget> widgetClass, TFunctionRef<void(UActivatableWidget&)> initInstanceFunc);

    // Find the widget if it exists on any of the layers and remove it from the layer.
    void FindAndRemoveWidgetFromLayer(UActivatableWidget* activatableWidget);

    // Get the layer widget for the given layer tag.
    UCommonActivatableWidgetContainerBase* GetLayerWidgetChecked(FGameplayTag layerName);

    static UPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* worldContextObject);

    UFUNCTION(BlueprintPure)
    static UPrimaryGameLayout* GetPrimaryGameLayout(APlayerController* playerController);

protected:
    /** Blueprint API: Register a layer that widgets can be pushed onto. */
    UFUNCTION(BlueprintCallable)
    void RegisterLayer(FGameplayTag layerTag, UCommonActivatableWidgetContainerBase* layerWidget);

private:
    // Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
    // for the duration of all of them.
    TArray<FName> SuspendInputTokens;

    // The registered layers for the primary layout.
    UPROPERTY(Transient)
    TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

    void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* widget, bool bIsTransitioning);
};
