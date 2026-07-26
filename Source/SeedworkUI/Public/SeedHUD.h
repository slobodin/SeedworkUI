// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "SeedHUD.generated.h"

class UPrimaryGameLayout;

UCLASS()
class SEEDWORKUI_API ASeedHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void PostInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

    UPrimaryGameLayout* GetPrimaryGameLayout() const { return RootLayout; }

private:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPrimaryGameLayout> PrimaryGameLayoutClass;

    UPROPERTY(Transient)
    TObjectPtr<UPrimaryGameLayout> RootLayout = nullptr;
};
