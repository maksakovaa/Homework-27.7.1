#include "functions.h"

void consoleClear()
{
#if defined (_WIN32) || defined (_WIN64)
	system("cls");
#elif defined (__linux__)
	system("clear");
#endif
}

void localeSetup()
{
#if defined(_WIN32) || defined (_WIN64)
	SetConsoleOutputCP(65001);
	SetConsoleCP(1251);
#elif defined(__linux__)
	setlocale(LC_ALL, "ru_RU.utf8");
#endif
}

void logOut()
{
	auth = false;
	userUUID.clear();
}

void cinClearIgnore()
{
	cin.clear();
	cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

void incorrect()
{
	cinClearIgnore();
	cout << "Некорректный ввод, повторите:" << endl;
}

bool nonLatinChk(string& text)
{
	string letters = "абвгдеёжщийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
	if (text.find_first_of(letters) != string::npos)
	{
		cout << "ERROR: Некорректный ввод. Используйте латинницу." << endl;
		return false;
	}
	return true;
}

bool noSpacesChk(string& text)
{
	if (text.find_first_of(" ") != string::npos)
	{
		cout << "ERROR: Некорректный ввод. Не используйте пробел." << endl;
		return false;
	}
	return true;
}

bool lengthChk(string& text)
{
	if (text.size() < 4)
	{
		cout << "ERROR: Длина должна быть не менее 4 символов." << endl;
		return false;
	}
	return true;
}

bool regLoginChk(string& _login)
{
	int errCount = 0;
	if (!nonLatinChk(_login))
		errCount++;
	if (!noSpacesChk(_login))
		errCount++;
	if (!lengthChk(_login))
		errCount++;

	if (UserBase->logInChk(_login))
	{
		cout << "ERROR: Имя пользователя уже занято." << endl;
		errCount++;
	}

	if (errCount == 0)
	{
		return true;
	}
	else
	{
		cout << "Повторите ввод:" << endl;
		return false;
	}
}

bool regPwdChk(string& _pwd)
{
	int errCount = 0;
	if (!nonLatinChk(_pwd))
		errCount++;
	if (!noSpacesChk(_pwd))
		errCount++;
	if (!lengthChk(_pwd))
		errCount++;

	if (errCount == 0)
	{
		return true;
	}
	else
	{
		cout << "Повторите ввод:" << endl;
		return false;
	}
}

bool authPwdChk(string& _login, string& _pwd)
{
	int id = UserBase->getUsrId(_login);
	return UserBase->pwdChk(id, _pwd);
}

bool authStatChk()
{
	if (auth && UserBase->getUsrName(userUUID) != "deleted")
	{
		return true;
	}
	return false;
}

bool authAdminStatChk()
{
	if (auth && UserBase->getUsrLogin(userUUID) == "admin")
		return true;
	else
		return false;
}

bool exit()
{
	Online->sendRequest("disconnect");
	return 0;
}

string getOsName()
{
#if defined(_WIN32) || defined (_WIN64)
	return "Windows";
#elif defined(__linux__)
	return "Linux";
#endif
}

int getProcId()
{
#if defined(__linux__)
	return getpid();
#elif defined(_WIN32) || defined(_WIN64)
	return GetCurrentProcessId();
#endif
}

string middle(const string& str)
{
	const int max_len = 80;

	size_t spaces_needed, str_len;
	str_len = str.length();
	spaces_needed = (max_len - str_len) / 2;

	string buffer(spaces_needed, ' ');
	buffer += str;

	return buffer;
}

void printHeader(const string& header)
{
	string border(80, '-');
	printf("%s\n%s\n", middle(header).data(), border.data());
}


void cp1251toUtf8(string& str)
{
#if defined(_WIN32) || defined(_WIN64)
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str.data(), -1, 0, 0);
	
	if (!result_u)
		return;

	wchar_t* ures = new wchar_t[result_u];	
	if (!MultiByteToWideChar(1251, 0, str.data(), -1, ures, result_u))
	{
		delete[] ures;
		return;
	}

	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c)
	{
		delete[] ures;
		return;
	}

	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0))
	{
		delete[] cres;
		return;
	}
	str = cres;
	delete[] ures, cres;
#endif
}


std::string curDateTime()
{
	time_t curtime;
	struct tm* loctime;
	char buffer[26];
	time(&curtime);
	loctime = localtime(&curtime);
	strftime(buffer, 26, "%Y.%m.%d %H:%M:%S", loctime);
	std::string result = buffer;
	return result;
}

void printUserBase()
{
	string id = "ID";
	string login = "Login";
	string name = "Имя пользователя";

	printf("%-*s %-*s %-*s\n", 5, id.data(), 12, login.data(), 32, name.data());

	for (int i = 0; i < UserBase->getCount(); i++)
	{
		string uuid = UserBase->getUsrUUID(i);
		string login = UserBase->getUsrLogin(i);
		string name = UserBase->getUsrName(i);

		if (auth && userUUID == UserBase->getUsrUUID(login))
			continue;
		if (UserBase->isDeleted(i))
			continue;

		if (authStatChk())
		{
			int msgCount = PerMsgBase->countUnreadPM(uuid);
			if (msgCount > 0)
			{
				printf("%-*i %-*s %-*s [%-i]\n", 5, i, 12, login.data(), 32, name.data(), msgCount);
			}
			else
			{
				printf("%-*i %-*s %-*s\n", 5, i, 12, login.data(), 32, name.data());
			}
		}
		else
		{
			printf("%-*i %-*s %-*s\n", 5, i, 12, login.data(), 32, name.data());
		}

	}
}