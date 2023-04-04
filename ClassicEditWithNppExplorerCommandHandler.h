#pragma once
#include "BaseNppExplorerCommandHandler.h"

using namespace NppShell::Helpers;

namespace NppShell::CommandHandlers
{
#ifdef WIN64
    class __declspec(uuid("B298D29A-A6ED-11DE-BA8C-A68E55D89593")) ClassicEditWithNppExplorerCommandHandler : public BaseNppExplorerCommandHandler
#else
    class __declspec(uuid("00F3C2EC-A6EE-11DE-A03A-EF8F55D89593")) ClassicEditWithNppExplorerCommandHandler : public BaseNppExplorerCommandHandler
#endif
    {
    public:
        ClassicEditWithNppExplorerCommandHandler();
        ~ClassicEditWithNppExplorerCommandHandler();

        const EXPCMDSTATE State(IShellItemArray* psiItemArray);
    };
}