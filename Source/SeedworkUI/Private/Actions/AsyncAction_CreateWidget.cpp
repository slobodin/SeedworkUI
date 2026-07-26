// Copyright (c) Mistfly Games. All Rights Reserved.

#include "Actions/AsyncAction_CreateWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UIUtilityLibrary.h"

void UAsyncAction_CreateWidget::Activate()
{
    static const FName SuspendReason = FName(TEXT("CreatingWidgetAsync"));

    SuspendInputToken = bSuspendInputUntilComplete
        ? UUIUtilityLibrary::SuspendInputForPlayer(OwningPlayer.Get(), SuspendReason)
        : NAME_None;

    StreamingHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
        UserWidgetSoftClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnWidgetLoaded),
        FStreamableManager::AsyncLoadHighPriority
    );

    // Setup a cancel delegate so that we can resume input if this handler is canceled.
    StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
        [this]()
        {
            UUIUtilityLibrary::ResumeInputForPlayer(OwningPlayer.Get(), SuspendInputToken);
        })
    );
}

void UAsyncAction_CreateWidget::Cancel()
{
    if (StreamingHandle.IsValid())
    {
        StreamingHandle->CancelHandle();
        StreamingHandle.Reset();
    }

    Super::Cancel();
}

UAsyncAction_CreateWidget* UAsyncAction_CreateWidget::CreateWidgetAsync(TSoftClassPtr<UUserWidget> userWidgetClass, APlayerController* owningPlayer, bool bSuspendInputUntilComplete)
{
    if (userWidgetClass.IsNull())
    {
        FFrame::KismetExecutionMessage(TEXT("CreateWidgetAsync was passed a null userWidgetClass"), ELogVerbosity::Error);
        return nullptr;
    }

    check(owningPlayer && owningPlayer->GetWorld());

    auto action = NewObject<UAsyncAction_CreateWidget>();
    action->UserWidgetSoftClass = userWidgetClass;
    action->OwningPlayer = owningPlayer;
    action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
    action->RegisterWithGameInstance(owningPlayer->GetWorld()->GetGameInstance());

    return action;
}

void UAsyncAction_CreateWidget::OnWidgetLoaded()
{
    if (bSuspendInputUntilComplete)
    {
        UUIUtilityLibrary::ResumeInputForPlayer(OwningPlayer.Get(), SuspendInputToken);
    }

    // If the load as successful, create it, otherwise don't complete this.
    TSubclassOf<UUserWidget> userWidgetClass = UserWidgetSoftClass.Get();
    if (userWidgetClass)
    {
        if (ensure(OwningPlayer.IsValid()))
        {
            auto widgetInst = UWidgetBlueprintLibrary::Create(OwningPlayer->GetWorld(), userWidgetClass, OwningPlayer.Get());
            OnComplete.Broadcast(widgetInst);
        }
    }

    SetReadyToDestroy();
}

void UAsyncAction_CreateWidget::SetReadyToDestroy()
{
    Super::SetReadyToDestroy();
    StreamingHandle.Reset();
    MarkAsGarbage();
}
