// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ShowMessageBox.generated.h"

UENUM(BlueprintType)
enum class EMessageBoxResultCode : uint8
{
    Accepted,
    Rejected
};

UCLASS(BlueprintType)
class SEEDWORKUI_API UAsyncAction_ShowMessageBox : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowMessageBoxResultDelegate, EMessageBoxResultCode, Result);

    UPROPERTY(BlueprintAssignable)
    FShowMessageBoxResultDelegate OnResult;

    virtual void Activate() override;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "worldContextObject"))
    static UAsyncAction_ShowMessageBox* ShowMessageBox(UObject* worldContextObject, FText title, FText message);
};
