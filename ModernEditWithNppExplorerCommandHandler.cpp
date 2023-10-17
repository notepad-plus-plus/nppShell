#include "pch.h"
#include "ModernEditWithNppExplorerCommandHandler.h"

#include "SharedState.h"
#include "LoggingHelper.h"

using namespace NppShell::CommandHandlers;
using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

ModernEditWithNppExplorerCommandHandler::ModernEditWithNppExplorerCommandHandler()
{
    g_loggingHelper.LogMessage(L"ModernEditWithNppExplorerCommandHandler::ctor", L"Creating object");
}

ModernEditWithNppExplorerCommandHandler::~ModernEditWithNppExplorerCommandHandler()
{
    g_loggingHelper.LogMessage(L"ModernEditWithNppExplorerCommandHandler::~tor", L"Destroying object");
}

const EXPCMDSTATE ModernEditWithNppExplorerCommandHandler::State(IShellItemArray* psiItemArray)
{
    UNREFERENCED_PARAMETER(psiItemArray);

    state->SetState(L"ModernEditWithNppExplorerCommandHandler", Set);

    g_loggingHelper.LogMessage(L"ModernEditWithNppExplorerCommandHandler::State", L"Current state: " + std::to_wstring(state->GetState(L"ModernEditWithNppExplorerCommandHandler")));

    return ECS_ENABLED;
}