// Copyright (c) Mistfly Games. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSeedworkUIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
