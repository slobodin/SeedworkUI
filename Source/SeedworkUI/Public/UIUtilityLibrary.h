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

    /** Removes the widget from whichever layer of its owning player's layout it was pushed to. Safe to call with an already destroyed widget. */
    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    static void PopContentFromLayer(UActivatableWidget* activatableWidget);

private:
    static int32 InputSuspensions;
};
