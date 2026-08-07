// Copyright (c) Mistfly Games. All Rights Reserved.

#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "ActivatableWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PrimaryGameLayout.h"

void UAsyncAction_PushContentToLayerForPlayer::Activate()
{
    if (auto rootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(OwningPlayerPtr.Get()))
    {
        TWeakObjectPtr<UAsyncAction_PushContentToLayerForPlayer> weakThis = this;

        auto streamingHandle = rootLayout->PushWidgetToLayerStackAsync(LayerName, bSuspendInputUntilComplete, WidgetClass, [weakThis](EAsyncWidgetLayerState state, UActivatableWidget* widget)
        {
            if (weakThis.IsValid())
            {
                switch (state)
                {
                case EAsyncWidgetLayerState::Initialize:
                    weakThis->BeforePush.Broadcast(widget);
                    break;

                case EAsyncWidgetLayerState::AfterPush:
                    weakThis->AfterPush.Broadcast(widget);
                    weakThis->SetReadyToDestroy();
                    break;

                case EAsyncWidgetLayerState::Canceled:
                    weakThis->SetReadyToDestroy();
                    break;
                }
            }
        });

        // The state callback can run before the push returns (for example when the widget class is
        // already loaded), which destroys this action. Only take ownership of the handle if we
        // survived that, otherwise we'd resurrect a request on an object that's already garbage.
        if (!bIsReadyToDestroy)
        {
            StreamingHandle = MoveTemp(streamingHandle);
        }
    }
    else
    {
        SetReadyToDestroy();
    }
}

void UAsyncAction_PushContentToLayerForPlayer::Cancel()
{
    if (StreamingHandle.IsValid())
    {
        StreamingHandle->CancelHandle();
        StreamingHandle.Reset();
    }

    Super::Cancel();
}

UAsyncAction_PushContentToLayerForPlayer* UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(
    APlayerController* owningPlayer, TSoftClassPtr<UActivatableWidget> widgetClass, FGameplayTag layerName, bool bSuspendInputUntilComplete)
{
    check(owningPlayer && !widgetClass.IsNull() && layerName.IsValid());

    if (auto world = owningPlayer->GetWorld())
    {
        auto action = NewObject<UAsyncAction_PushContentToLayerForPlayer>();
        action->WidgetClass = widgetClass;
        action->OwningPlayerPtr = owningPlayer;
        action->LayerName = layerName;
        action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
        action->RegisterWithGameInstance(world);
        return action;
    }

    return nullptr;
}

void UAsyncAction_PushContentToLayerForPlayer::SetReadyToDestroy()
{
    bIsReadyToDestroy = true;

    Super::SetReadyToDestroy();
    StreamingHandle.Reset();
    if (!IsEngineExitRequested())
    {
        MarkAsGarbage();
    }
}
