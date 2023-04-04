#include "pch.h"
#include "ThreadUILanguageChanger.h"
#include "LoggingHelper.h"

using namespace NppShell::Helpers;

extern LoggingHelper g_loggingHelper;

ThreadUILanguageChanger::ThreadUILanguageChanger(const wstring& languageTag)
{
    // Save the original thread UI language setting
    m_originalLanguageTag = GetThreadLanguage();

    // Set the new thread UI language setting
    SetThreadLanguage(languageTag);

    g_loggingHelper.LogMessage(L"ThreadUILanguageChanger::ctor", L"Original language: " + m_originalLanguageTag);
    g_loggingHelper.LogMessage(L"ThreadUILanguageChanger::ctor", L"New language: " + languageTag);
}

ThreadUILanguageChanger::~ThreadUILanguageChanger()
{
    // Restore the original thread UI language setting
    SetThreadLanguage(m_originalLanguageTag);

    g_loggingHelper.LogMessage(L"ThreadUILanguageChanger::~tor", L"Original language: " + m_originalLanguageTag);
}

wstring ThreadUILanguageChanger::GetThreadLanguage()
{
    DWORD numLanguages = 0;
    DWORD bufferSize = 0;
    GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &bufferSize);

    wstring language(bufferSize, '\0');
    GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, language.data(), &bufferSize);

    return language;
}

void ThreadUILanguageChanger::SetThreadLanguage(const wstring& languageTag)
{
    // Set the new thread UI language setting
    SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, languageTag.c_str(), nullptr);
}