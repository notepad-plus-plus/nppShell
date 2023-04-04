#include "pch.h"
#include "ModernEditWithNppExplorerCommandHandler.h"

using namespace NppShell::CommandHandlers;

const EXPCMDSTATE ModernEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    return ECS_ENABLED;
}