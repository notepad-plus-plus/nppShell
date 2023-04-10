#include "pch.h"
#include "ClassicEditWithNppExplorerCommandHandler.h"

#include "SharedState.h"

using namespace NppShell::CommandHandlers;

const EXPCMDSTATE ClassicEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    // First we get the current state, before we clear it.
    CounterState currentState = state->GetState();
    state->SetState(NotSet);

    // If it is set, it means the State function has been called in the Modern command handler last, which means we should hide this one.
    if (currentState == CounterState::Set)
    {
        return ECS_HIDDEN;
    }

    return ECS_ENABLED;
}