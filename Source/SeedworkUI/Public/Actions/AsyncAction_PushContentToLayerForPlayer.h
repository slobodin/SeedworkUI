// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "AsyncAction_PushContentToLayerForPlayer.generated.h"

class APlayerController;
class UActivatableWidget;
struct FStreamableHandle;

/**
 * 
 */
UCLASS(BlueprintType)
class SEEDWORKUI_API UAsyncAction_PushContentToLayerForPlayer : public UCancellableAsyncAction
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToLayerForPlayerDelegate, UActivatableWidget*, userWidget);

    UPROPERTY(BlueprintAssignable)
    FPushContentToLayerForPlayerDelegate BeforePush;

    UPROPERTY(BlueprintAssignable)
    FPushContentToLayerForPlayerDelegate AfterPush;

    virtual void Activate() override;
    virtual void Cancel() override;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true"))
    static UAsyncAction_PushContentToLayerForPlayer* PushContentToLayerForPlayer(
        APlayerController* owningPlayer, UPARAM(meta = (AllowAbstract = false)) TSoftClassPtr<UActivatableWidget> widgetClass, FGameplayTag layerName, bool bSuspendInputUntilComplete = true);

private:
    TWeakObjectPtr<APlayerController> OwningPlayerPtr;
    TSoftClassPtr<UActivatableWidget> WidgetClass;
    TSharedPtr<FStreamableHandle> StreamingHandle;
    FGameplayTag LayerName;
    bool bSuspendInputUntilComplete = false;
    bool bIsReadyToDestroy = false;

    virtual void SetReadyToDestroy() override;
};
