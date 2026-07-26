// Copyright (c) Mistfly Games. All Rights Reserved.

#include "SeedHUD.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "PrimaryGameLayout.h"

void ASeedHUD::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (GetWorld() && GetWorld()->IsGameWorld() && !GetGameInstance()->IsDedicatedServerInstance())
    {
        check(!RootLayout);
        check(GetOwningPlayerController());

        RootLayout = CreateWidget<UPrimaryGameLayout>(GetOwningPlayerController(), PrimaryGameLayoutClass);
        RootLayout->AddToViewport(0);

#if WITH_EDITOR
        if (GIsEditor && GetOwningPlayerController()->IsPrimaryPlayer())
        {
            // So our controller will work in PIE without needing to click in the viewport
            FSlateApplication::Get().SetUserFocusToGameViewport(0);
        }
#endif
    }
}

void ASeedHUD::EndPlay(const EEndPlayReason::Type endPlayReason)
{
    if (RootLayout)
    {
        RootLayout->RemoveFromParent();
        RootLayout = nullptr;
    }

    Super::EndPlay(endPlayReason);
}
