#pragma once
#include "MsgBase.h"

extern net* Online;
extern UsrBase* UserBase;
extern MsgBase* AllMsgBase;
extern MsgBase* PerMsgBase;
extern string userUUID;
extern int choice;
extern bool auth;

void consoleClear();
void localeSetup();
void logOut();
void cinClearIgnore();
void incorrect();
bool nonLatinChk(string& text);
bool noSpacesChk(string& text);
bool lengthChk(string& text);
bool regLoginChk(string& _login);
bool regPwdChk(string& _pwd);
bool authPwdChk(string& _login, string& _pwd);
bool authStatChk();
bool authAdminStatChk();
bool exit();
string getOsName();
int getProcId();
string middle(const string& str);
void printHeader(const string& header);
void cp1251toUtf8(string& str);
std::string curDateTime();
void printUserBase();