#include "pch.h"
#include "ClassicEditWithNppExplorerCommandHandler.h"

using namespace NppShell::CommandHandlers;

const EXPCMDSTATE ClassicEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    int state = counter->GetValue();

    if (state == 3 || state == 5)
    {
        // This is on Windows 11, with both the modern and classic being shows, so we hide this one.
        return ECS_HIDDEN;
    }

    return ECS_ENABLED;
}