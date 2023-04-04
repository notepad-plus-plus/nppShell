#include "pch.h"
#include "ClassicEditWithNppExplorerCommandHandler.h"

#include "LoggingHelper.h"
#include "SharedState.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

ClassicEditWithNppExplorerCommandHandler::ClassicEditWithNppExplorerCommandHandler()
{
    g_loggingHelper.LogMessage(L"ClassicEditWithNppExplorerCommandHandler::ctor", L"Creating object");
}

ClassicEditWithNppExplorerCommandHandler::~ClassicEditWithNppExplorerCommandHandler()
{
    g_loggingHelper.LogMessage(L"ClassicEditWithNppExplorerCommandHandler::~tor", L"Destroying object");
}

const EXPCMDSTATE ClassicEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    // First we get the current state, before we clear it.
    CounterState currentState = state->GetState();
    state->SetState(NotSet);

    g_loggingHelper.LogMessage(L"ClassicEditWithNppExplorerCommandHandler::State", L"Current state: " + std::to_wstring(currentState));

    // If it is set, it means the State function has been called in the Modern command handler last, which means we should hide this one.
    if (currentState == CounterState::Set)
    {
        return ECS_HIDDEN;
    }

    return ECS_ENABLED;
}