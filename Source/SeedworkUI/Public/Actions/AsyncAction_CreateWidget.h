// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "Engine/StreamableManager.h"
#include "AsyncAction_CreateWidget.generated.h"

class APlayerController;
class UUserWidget;

UCLASS(BlueprintType)
class SEEDWORKUI_API UAsyncAction_CreateWidget : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreateWidgetDelegate, UUserWidget*, UserWidget);

    UPROPERTY(BlueprintAssignable)
    FCreateWidgetDelegate OnComplete;

    virtual void Activate() override;
    virtual void Cancel() override;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true"))
    static UAsyncAction_CreateWidget* CreateWidgetAsync(
        TSoftClassPtr<UUserWidget> userWidgetClass, APlayerController* owningPlayer, bool bSuspendInputUntilComplete = true);

private:
    TWeakObjectPtr<APlayerController> OwningPlayer;
    TSoftClassPtr<UUserWidget> UserWidgetSoftClass;
    TSharedPtr<FStreamableHandle> StreamingHandle;
    FName SuspendInputToken;
    bool bSuspendInputUntilComplete = false;

    void OnWidgetLoaded();

    virtual void SetReadyToDestroy() override;
};
