// Copyright (c) Mistfly Games. All Rights Reserved.

#include "ActivatableWidget.h"
#include "Editor/WidgetCompilerLog.h"

#define LOCTEXT_NAMESPACE "SeedworkUI"

TOptional<FUIInputConfig> UActivatableWidget::GetDesiredInputConfig() const
{
    switch (InputConfig)
    {
    case EGameWidgetInputMode::GameAndUI:
        return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);

    case EGameWidgetInputMode::GameOnly:
        return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);

    case EGameWidgetInputMode::UIOnly:
        return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);

    default:
        break;
    }

    return {};
}

#if WITH_EDITOR

void UActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& blueprintWidgetTree, class IWidgetCompilerLog& compileLog) const
{
    Super::ValidateCompiledWidgetTree(blueprintWidgetTree, compileLog);

    if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UActivatableWidget, BP_GetDesiredFocusTarget)))
    {
        if (GetParentNativeClass(GetClass()) == UActivatableWidget::StaticClass())
        {
            compileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
        }
        else
        {
            //TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
            compileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
        }
    }
}

#endif

#undef LOCTEXT_NAMESPACE
