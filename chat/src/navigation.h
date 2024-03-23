#pragma once
#include "functions.h"

extern UsrBase* UserBase;
extern MsgBase* AllMsgBase;
extern MsgBase* PerMsgBase;
extern string userUUID;
extern int choice;
extern bool auth;

void menu();
void menuRegUser();
void menuLogin();
void menuUserList();
void menuUserMgmt();
void menuDelUser();
void chkUsersAv();
void menuUsrMgmtSetPwd();
void menuChgPwd(int choice);
void menuAuth();
void menuPrivateSendMsg();
void menuMainChat();
void menuAbout();
