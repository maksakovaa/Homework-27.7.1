#include "navigation.h"

void menu()
{
	if (authStatChk())
	{
		menuAuth();
		return;
	}
	consoleClear();
	printHeader("[Главное меню]");
	cout << "Добро пожаловать в консольный чат!\nДля просмотра и отправки сообщений необходимо войти или зарегистрироваться. " << endl;
	cout << "Выберите действие:\n 1 - Регистрация пользователя \n 2 - Авторизация \n 3 - Список пользователей \n 4 - О программе \n 5 - Выход из программы" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice !=5)
	{
		incorrect();
		cin >> choice;
	}

	switch (choice)
	{
	case 1:
	{
		menuRegUser();
		break;
	}
	case 2:
	{
		menuLogin();
		break;
	}
	case 3:
	{
		menuUserList();
		break;
	}
	case 4:
	{
		menuAbout();
		break;
	}
	case 5:
	{
		exit();
		break;
	}
	default:
		break;
	}
}

void menuRegUser()
{
	string name, login, pwd;
	cinClearIgnore();
	consoleClear();
	printHeader("[Регистрация]");
	cout << "Введите имя пользователя:" << endl;
	getline(cin, name);
	cp1251toUtf8(name);
	cout << "Введите логин для пользователя " << name << ":" << endl;
	getline(cin, login);
	cp1251toUtf8(login);
	while (!regLoginChk(login))
	{
		getline(cin, login);
		cp1251toUtf8(login);
	}
	cout << "Введите желаемый пароль для " << name << ":" << endl;
	getline(cin, pwd);
	cp1251toUtf8(pwd);
	while (!regPwdChk(pwd))
	{
		getline(cin, pwd);
		cp1251toUtf8(pwd);
	}
	UserBase->regUsr(name, login, pwd);
	cout << "Пользователь " << name << " успешно зарегистрирован. \n 1 - Главное меню\n 2 - Авторизация" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2)
	{
		incorrect();
		cin >> choice;
	}
	switch (choice)
	{
	case 1:
	{
		if (authStatChk())
		{
			menuAuth();
			break;
		}
		else
		{
			menu();
			break;
		}
		break;
	}
	case 2:
	{
		menuLogin();
		break;
	}
	default:
		break;
	}
}

void menuLogin()
{
	string name, login, pwd;
	cinClearIgnore();
	consoleClear();
	printHeader("[Авторизация]");
	cout << "Вход для зарегистрированных пользователей" << endl;
	cout << "Введите логин:" << endl;
	getline(cin, login);
	cp1251toUtf8(login);
	if (UserBase->logInChk(login))
	{
		cout << "Введите пароль:" << endl;
		getline(cin, pwd);
		cp1251toUtf8(pwd);
		while (!authPwdChk(login, pwd))
		{
			cout << "Неверный пароль, повторите:" << endl;
			getline(cin, pwd);
			cp1251toUtf8(pwd);
		}
		if (authPwdChk(login, pwd))
		{
			auth = true;
			userUUID = UserBase->getUsrUUID(login);
			AllMsgBase->getAllMsgBase(userUUID);
			PerMsgBase->getPerMsgBase(userUUID);
			menuAuth();
		}
	}
	else
	{
		cout << "Неверное имя пользователя" << endl;
		cout << "1 - вернуться в главное меню, 2 - выход из программы, 3 - повторить" << endl;
		cin >> choice;
		while (choice != 1 && choice != 2 && choice != 3)
		{
			incorrect();
			cin >> choice;
		}
		switch (choice)
		{
		case 1:
		{
			menu();
			break;
		}
		case 2:
		{
			exit();
			break;
		}
		case 3:
		{
			menuLogin();
			break;
		}
		default:
			break;
		}
	}
}

void menuUserList()
{
	consoleClear();
	printHeader("[Список пользователей]");
	printUserBase();
	cout << "1 - вернуться в главное меню, 2 - выход из программы";
	if (authStatChk())
		cout << " 3 - Просмотр сообщений";
	cout << endl;

	cin >> choice;
	if (authStatChk())
	{
		while (choice != 1 && choice != 2 && choice != 3)
		{
			incorrect();
			cin >> choice;
		}
	}
	else
	{
		while (choice != 1 && choice != 2)
		{
			incorrect();
			cin >> choice;
		}
	}
	switch (choice)
	{
	case 1:
	{
		if (authStatChk())
		{
			menuAuth();
		}
		else
		{
			menu();
		}
		break;
	}
	case 2:
	{
		exit();
		break;
	}
	case 3:
	{
		menuPrivateSendMsg();
		break;
	}
	default:
		break;
	}
}

void menuUserMgmt()
{
	if (!authAdminStatChk())
	{
		menuAuth();
		return;
	}
	consoleClear();
	printHeader("Управление пользователями");
	UserBase->printUsrBase();
	cout << "\n Выберите действие: \n 1 - Добавить пользователя \n 2 - Удалить пользователя \n 3 - Сменить пароль пользователя \n 4 - Главное меню" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2 && choice != 3 && choice != 4)
	{
		incorrect();
		cin >> choice;
	}
	switch (choice)
	{
	case 1:
	{
		menuRegUser();
		break;
	}
	case 2:
	{
		menuDelUser();
		break;
	}
	case 3:
	{
		menuUsrMgmtSetPwd();
		break;
	}
	case 4:
	{
		menuAuth();
		break;
	}
	default:
		break;
	}
}

void chkUsersAv()
{
	if (UserBase->getCount() <= 1)
	{
		cout << "Нет доступных для измениения пользователей. Продолжить? \n 1 - Да 2 - нет" << endl;
		cin >> choice;
		while (choice != 1 && choice != 2)
		{
			incorrect();
			cin >> choice;
		}
		cinClearIgnore();
		switch (choice)
		{
		case 1:
		{
			menuUserMgmt();
			break;
		}
		case 2:
		{
			menuAuth();
			break;
		}
		default:
			break;
		}
	}
}

void menuDelUser()
{
	if (!authAdminStatChk())
	{
		menuAuth();
		return;
	}
	if (UserBase->getCount() > 1)
	{
		cout << "Учётную запись \"Администратор\" удалить нельзя. Для удаления пользователя введите его ID : " << endl;
		cin >> choice;
		while (choice <= 0 || choice >= UserBase->getCount())
		{
			incorrect();
			cin >> choice;
		}
		cinClearIgnore();
		cout << "Вы уверены что хотите удалить " << UserBase->getUsrName(choice) << "? \n 1 - да, 2 - нет" << endl;
		int temp;
		cin >> temp;
		while (temp != 1 && temp != 2)
		{
			incorrect();
			cin >> temp;
		}
		cinClearIgnore();
		switch (temp)
		{
		case 1:
		{
			UserBase->delUsr(choice);
			menuUserMgmt();
			break;
		}
		case 2:
		{
			menuUserMgmt();
			break;
		}
		default:
			break;
		}
	}
	else
	{
		chkUsersAv();
	}
}

void menuUsrMgmtSetPwd()
{
	if (UserBase->getCount() > 1)
	{
		cout << "Изменить пароль учётной записи \"Администратор\" нельзя. Для изменения пароля пользователя введите его ID : " << endl;
		cin >> choice;
		while (choice <= 0 || choice >= UserBase->getCount())
		{
			incorrect();
			cin >> choice;
		}
		cinClearIgnore();
		menuChgPwd(choice);
	}
	else
	{
		chkUsersAv();
	}
}

void menuChgPwd(int choice = UserBase->getUsrId())
{
	cinClearIgnore();
	string pwd;
	cout << "Введите новый пароль:" << endl;
	getline(cin, pwd);
	cp1251toUtf8(pwd);
	while (!regPwdChk(pwd))
	{
		getline(cin, pwd);
		cp1251toUtf8(pwd);
	}
	UserBase->setUsrPwd(choice, pwd);
	cout << "Пароль пользователя " << UserBase->getUsrName(choice) << " успешно изменен. Введите 1 чтобы продолжить, 2 чтобы выйти из программы" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2)
	{
		incorrect();
		cin >> choice;
	}
	switch (choice)
	{
		case 1:
		{
			if(authAdminStatChk())
				menuUserMgmt();
			if (authStatChk())
				menuAuth();
			break;
		}
		case 2:
		{
			exit();
			break;
		}
		default:
			break;
	}
}

void menuAuth()
{
	if (!authStatChk())
	{
		menu();
		return;
	}
	consoleClear();
	printHeader("[Главное меню]");
	cout << UserBase->getUsrName(userUUID) << " добро пожаловать в консольный чат!" << endl;
	cout << "Выберите действие:\n 1 - Общий чат";
	if (AllMsgBase->countUnreadAM() > 0)
	{
		cout << "\t\t [" << AllMsgBase->countUnreadAM() << "] непроочитанных сообщений";
	}
	cout << "\n 2 - Личные сообщения";
	if (PerMsgBase->countUnreadPM() > 0)
	{
		cout << "\t [" << PerMsgBase->countUnreadPM() << "] непроочитанных сообщений";
	}
	cout << endl;
	if (UserBase->getUsrLogin(userUUID) == "admin")
	{
		cout << " 3 - Управление пользователями" << endl;
	}
	else
	{
		cout << " 3 - Изменить пароль" << endl;
	}
	cout << " 4 - Выйти из учетной записи\n 5 - О программе\n 6 - Выход из программы" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5 && choice != 6)
	{
		incorrect();
		cin >> choice;
	}
	switch (choice)
	{
	case 1:
	{
		menuMainChat();
		break;
	}
	case 2:
	{
		menuUserList();
		break;
	}
	case 3:
	{
		if (UserBase->getUsrLogin(userUUID) == "admin")
		{
			menuUserMgmt();
			break;
		}
		else
		{
			menuChgPwd();
			break;
		}
	}
	case 4:
	{
		logOut();
		menu();
		break;
	}
	case 5:
	{
		menuAbout();
		break;
	}
	case 6:
	{
		exit();
		break;
	}
	default:
		break;
	}
}

void menuPrivateSendMsg()
{
	if (!authStatChk())
	{
		menu();
		return;
	}
	int temp;
	cout << "Введите ID пользователя:" << endl;
	cin >> temp;
	while (temp < 0 || temp >= UserBase->getCount() || temp == UserBase->getUsrId())
	{
		incorrect();
		cin >> temp;
	}
	cinClearIgnore();
	while (choice == 3)
	{
		consoleClear();
		string header = "[Чат с " + UserBase->getUsrName(temp) + "]";
		printHeader(header);
		PerMsgBase->printPers(UserBase->getUsrUUID(temp), userUUID);
		cout << " 1 - Вернуться в главное меню\n 2 - Выход из программы\n 3 - Отправка сообщения\n 4 - Вернуться к списку пользователей" << endl;
		cin >> choice;
		while (choice != 1 && choice != 2 && choice != 3 && choice != 4)
		{
			incorrect();
			cin >> choice;
		}
		cinClearIgnore();
		switch (choice)
		{
			case 1:
			{
				menuAuth();
				break;
			}
			case 2:
			{
				exit();
				break;
			}
			case 3:
			{
				cout << "Введите сообщение:" << endl;
				string msgText;
				getline(cin, msgText);
				cp1251toUtf8(msgText);
				while (msgText.empty())
				{
					cout << "Некорректный ввод, повторите:" << endl;
					getline(cin, msgText);
					cp1251toUtf8(msgText);
				}
				PerMsgBase->sendMsg(userUUID, UserBase->getUsrUUID(temp), msgText);
				break;
			}
			case 4:
			{
				menuUserList();
				break;
			}
			default:
				break;
		}
	}
}

void menuMainChat()
{
	if (!authStatChk())
	{
		menu();
		return;
	}
	while (choice == 1)
	{
		consoleClear();
		printHeader("[Общий чат]");
		AllMsgBase->printMain();
		cout << " 1 - Отправить сообщение\n 2 - Вернуться в главное меню\n 3 - Выход из программы" << endl;
		cin >> choice;
		while (choice != 1 && choice != 2 && choice != 3)
		{
			incorrect();
			cin >> choice;
		}
		cinClearIgnore();
		switch (choice)
		{
		case 1:
		{
			cout << "Введите сообщение:" << endl;
			string msgText;
			getline(cin, msgText);
			cp1251toUtf8(msgText);
			while (msgText.empty())
			{
				cout << "Некорректный ввод, повторите:" << endl;
				getline(cin, msgText);
				cp1251toUtf8(msgText);
			}
			AllMsgBase->sendMsg(userUUID, "ALL",  msgText);
			break;
		}
		case 2:
		{
			menuAuth();
			break;
		}
		case 3:
		{
			exit();
			break;
		}
		default:
			break;
		}
	}
}

void menuAbout()
{
	consoleClear();
	printHeader("О программе");
	cout << "Программа консольный чат. [0.59]\n© Максаков А.А., 2024\nСборка для " << getOsName() << endl;
	cout << "PID процесса: " << getProcId() << endl;
	cout << "1 - Вернуться в главное меню\n2 - Выход из программы" << endl;
	cin >> choice;
	while (choice != 1 && choice != 2)
	{
		incorrect();
		cin >> choice;
	}
	cinClearIgnore();
	switch (choice)
	{
	case 1:
	{
		menu();
		break;
	}
	case 2:
	{
		exit();
		break;
	}
	default:
		break;
	}
}
