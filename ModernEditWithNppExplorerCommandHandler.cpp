#include "pch.h"
#include "ModernEditWithNppExplorerCommandHandler.h"

#include "SharedState.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

const EXPCMDSTATE ModernEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    state->SetState(Set);

    return ECS_ENABLED;
}