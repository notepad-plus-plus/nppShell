#pragma once

namespace NppShell::Helpers
{
    class ThreadUILanguageChanger
    {
    public:
        ThreadUILanguageChanger(const wstring& languageTag);
        ~ThreadUILanguageChanger();

    private:
        wstring GetThreadLanguage();
        void SetThreadLanguage(const wstring& languageTag);

        wstring m_originalLanguageTag;
    };
}