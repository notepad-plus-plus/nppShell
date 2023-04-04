#pragma once
#include "ExplorerCommandBase.h"
#include "SharedCounter.h"

using namespace NppShell::Helpers;

namespace NppShell::CommandHandlers
{
    class BaseNppExplorerCommandHandler : public ExplorerCommandBase
    {
    public:
        BaseNppExplorerCommandHandler();

        const wstring Title() override;
        const wstring Icon() override;

        IFACEMETHODIMP Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc) noexcept override;

        virtual const EXPCMDSTATE State(IShellItemArray* psiItemArray) override;

    private:
        const wstring GetNppExecutableFullPath();
        const wstring GetCommandLine(const wstring& itemName);

    protected:
        unique_ptr<SharedCounter> counter;
    };
}