#include "UsrBased.h"

UsrBase::UsrBase() {}

UsrBase::~UsrBase() {}

User UsrBase::splitUsrPkg(string& usrPkg)
{
    string name, login, pwdHash;
    name = unTag(m_Attr::indName, usrPkg);
    login = unTag(m_Attr::indLogin, usrPkg);
    pwdHash = unTag(m_Attr::indPwd, usrPkg);
    User newUser(name, login, pwdHash);
    return newUser;
}

User UsrBase::getUser(int id)
{
    return base->at(id);
}

User UsrBase::getUser(const string& uuid)
{
    for (User account: *base)
    {
        if (account.uuid == uuid)
        {
            return account;
        }
    }
    return base->at(0);
}

string UsrBase::packUsrPkg(User& account)
{
    string result = tagStructParam(m_Attr::indUUID, account.uuid);
    result += tagStructParam(m_Attr::indName, account.name);
    result += tagStructParam(m_Attr::indLogin, account.login);
    result += tagStructParam(m_Attr::indPwd, account.pwd);
    result += tagStructParam(m_Attr::indDel, std::to_string(account.deleted));
    return result;
}

void UsrBase::splitChgPwd(string& str, string arr[])
{
    arr[0] = unTag(m_Attr::indUUID, str);
    arr[1] = unTag(m_Attr::indPwd, str);
}

string UsrBase::tagStructParam(m_Attr id, string value)
{
    string result = sepBeg[static_cast<int>(id)];
    result += value;
    result += sepEnd[static_cast<int>(id)];
    return result;
}

string UsrBase::unTag(m_Attr id, string& str)
{
    string result;
    result = str.substr(str.find(sepBeg[static_cast<int>(id)]) + sepBeg[static_cast<int>(id)].length(), str.find(sepEnd[static_cast<int>(id)]) - sepBeg[static_cast<int>(id)].length());
    str.erase(0, str.find(sepEnd[static_cast<int>(id)]) + sepEnd[static_cast<int>(id)].length());
    return result;
}