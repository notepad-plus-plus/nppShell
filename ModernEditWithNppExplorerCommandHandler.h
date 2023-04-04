#pragma once
#include "BaseNppExplorerCommandHandler.h"

using namespace NppShell::Helpers;

namespace NppShell::CommandHandlers
{
    class __declspec(uuid("E6950302-61F0-4FEB-97DB-855E30D4A991")) ModernEditWithNppExplorerCommandHandler : public BaseNppExplorerCommandHandler
    {
    public:
        const EXPCMDSTATE State(IShellItemArray* psiItemArray);
    };
}