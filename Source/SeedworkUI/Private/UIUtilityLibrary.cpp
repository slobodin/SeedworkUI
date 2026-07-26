// Copyright (c) Mistfly Games. All Rights Reserved.

#include "UIUtilityLibrary.h"
#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "Components/Widget.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

int32 UUIUtilityLibrary::InputSuspensions = 0;

FName UUIUtilityLibrary::SuspendInputForPlayer(APlayerController* playerController, FName suspendReason)
{
    if (auto inputSubsystem = UCommonInputSubsystem::Get((playerController ? playerController->GetLocalPlayer() : nullptr)))
    {
        InputSuspensions++;
        FName suspendToken = suspendReason;
        suspendToken.SetNumber(InputSuspensions);

        inputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, suspendToken, true);
        inputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, suspendToken, true);
        inputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, suspendToken, true);

        return suspendToken;
    }

    return NAME_None;
}

void UUIUtilityLibrary::ResumeInputForPlayer(APlayerController* playerController, FName suspendToken)
{
    if (suspendToken == NAME_None)
    {
        return;
    }

    if (auto inputSubsystem = UCommonInputSubsystem::Get((playerController ? playerController->GetLocalPlayer() : nullptr)))
    {
        inputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, suspendToken, false);
        inputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, suspendToken, false);
        inputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, suspendToken, false);
    }
}

void UUIUtilityLibrary::PopContentFromLayer(UActivatableWidget* activatableWidget)
{
    if (!activatableWidget)
    {
        // Ignore request to pop an already deleted widget
        return;
    }

    if (auto primaryLayout = UPrimaryGameLayout::GetPrimaryGameLayout(activatableWidget->GetOwningPlayer()))
    {
        primaryLayout->FindAndRemoveWidgetFromLayer(activatableWidget);
    }
}
