// Copyright (c) Mistfly Games. All Rights Reserved.

#include "PrimaryGameLayout.h"
#include "ActivatableWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SeedHUD.h"
#include "UIUtilityLibrary.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

TSharedPtr<FStreamableHandle> UPrimaryGameLayout::PushWidgetToLayerStackAsync(FGameplayTag layerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UActivatableWidget> widgetClass, TFunction<void(EAsyncWidgetLayerState, UActivatableWidget*)> stateFunc)
{
    static FName NAME_PushingWidgetToLayer("PushingWidgetToLayer");
    const FName SuspendInputToken = bSuspendInputUntilComplete
        ? UUIUtilityLibrary::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer)
        : NAME_None;

    auto onLoadComplete = FStreamableDelegate::CreateWeakLambda(this, [this, layerName, widgetClass, stateFunc, SuspendInputToken]()
    {
        UUIUtilityLibrary::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

        auto widget = PushWidgetToLayerStack(layerName, widgetClass.Get(), [stateFunc](UActivatableWidget& widgetToInit)
        {
            stateFunc(EAsyncWidgetLayerState::Initialize, &widgetToInit);
        });

        stateFunc(EAsyncWidgetLayerState::AfterPush, widget);
    });

    auto streamingHandle = UAssetManager::Get().GetStreamableManager()
        .RequestAsyncLoad(widgetClass.ToSoftObjectPath(), MoveTemp(onLoadComplete));

    // Setup a cancel delegate so that we can resume input if this handler is canceled.
    streamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
        [this, stateFunc, SuspendInputToken]()
        {
            UUIUtilityLibrary::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
            stateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
        })
    );

    return streamingHandle;
}

UActivatableWidget* UPrimaryGameLayout::PushWidgetToLayerStack(FGameplayTag layerName, TSubclassOf<UActivatableWidget> widgetClass)
{
    return PushWidgetToLayerStack(layerName, widgetClass, [](UActivatableWidget&) {});
}

UActivatableWidget* UPrimaryGameLayout::PushWidgetToLayerStack(FGameplayTag layerName, TSubclassOf<UActivatableWidget> widgetClass, TFunctionRef<void(UActivatableWidget&)> initInstanceFunc)
{
    auto layer = GetLayerWidgetChecked(layerName);

    return layer->AddWidget(widgetClass, initInstanceFunc);
}

void UPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UActivatableWidget* activatableWidget)
{
    if (!IsValid(activatableWidget))
    {
        return;
    }

    // We're not sure what layer the widget is on so go searching.
    for (const auto& [_, layer] : Layers)
    {
        layer->RemoveWidget(*activatableWidget);
    }
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidgetChecked(FGameplayTag layerName)
{
    auto layer = Layers.FindRef(layerName);
    check(layer);
    return layer;
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* worldContextObject)
{
    auto pc = UGameplayStatics::GetGameInstance(worldContextObject)->GetPrimaryPlayerController(false);

    return GetPrimaryGameLayout(pc);
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(APlayerController* playerController)
{
    if (!playerController)
    {
        return nullptr;
    }

    if (auto hud = playerController->GetHUD<ASeedHUD>())
    {
        return hud->GetPrimaryGameLayout();
    }

    return nullptr;
}

void UPrimaryGameLayout::RegisterLayer(FGameplayTag layerTag, UCommonActivatableWidgetContainerBase* layerWidget)
{
    if (!IsDesignTime())
    {
        layerWidget->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnWidgetStackTransitioning);
        // TODO: Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
        //       transition effect will cause focus to not transition properly to the new widgets when using
        //       gamepad always.
        layerWidget->SetTransitionDuration(0.0);
        Layers.Add(layerTag, layerWidget);
    }
}

void UPrimaryGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* widget, bool bIsTransitioning)
{
    if (bIsTransitioning)
    {
        static const FName NAME_GlobalStackTransition("GlobalStackTransition");
        const FName suspendToken = UUIUtilityLibrary::SuspendInputForPlayer(GetOwningPlayer(), NAME_GlobalStackTransition);
        SuspendInputTokens.Add(suspendToken);
    }
    else
    {
        if (ensure(SuspendInputTokens.Num() > 0))
        {
            const FName suspendToken = SuspendInputTokens.Pop();
            UUIUtilityLibrary::ResumeInputForPlayer(GetOwningPlayer(), suspendToken);
        }
    }
}
