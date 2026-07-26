// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIUtilityLibrary.generated.h"

class APlayerController;
class UActivatableWidget;

/**
 * UI Functions library.
 */
UCLASS()
class SEEDWORKUI_API UUIUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    static FName SuspendInputForPlayer(APlayerController* playerController, FName suspendReason);

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    static void ResumeInputForPlayer(APlayerController* playerController, FName suspendToken);

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    static void PopContentFromLayer(UActivatableWidget* activatableWidget);

private:
    static int32 InputSuspensions;
};
