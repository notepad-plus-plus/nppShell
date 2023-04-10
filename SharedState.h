#pragma once

namespace NppShell::Helpers
{
    enum CounterState
    {
        NotSet,
        Set
    };

    class SharedState
    {
    public:
        SharedState();
        ~SharedState();

        CounterState GetState() const;
        void SetState(CounterState state);

    private:
        HANDLE hFileMapping;
        HANDLE hMutex;
        CounterState* pState;
    };
}