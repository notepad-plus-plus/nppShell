#pragma once

class AclHelper
{
public:
    AclHelper();
    ~AclHelper();

    DWORD ResetAcl(const wstring& path);

private:
    PACL emptyAcl;
};