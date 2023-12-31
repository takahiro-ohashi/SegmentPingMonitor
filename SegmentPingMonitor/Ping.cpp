#include "pch.h"
#include <regex>
#include "Ping.h"
#if 0
int Ping::validateAddress(TCHAR* address) 
{
	// Regex expression for validating IPv4
	std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

	// Regex expression for validating IPv6
	std::regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

	if (!((regex_match(address, ipv4) ^ regex_match(address, ipv6)) ))
	{
		return -1;
	}
	// Checking if it is a valid IPv4 addresses
	if (regex_match(address, ipv4))
	{
		return 1;
	}
	// Checking if it is a valid IPv6 addresses
	//else if (regex_match(address, ipv6))
	return 2;

	// Return Invalid
}
#endif
//CCriticalSection pingCriticalSection;
void Ping::stopreq() 
{
	stopFlag = true;
}
/*
#include <winsock2.h>
*/
//#include <ws2tcpip.h>
#include "resource.h"
extern HINSTANCE hDLL1;// = LoadLibrary(_T("Iphlpapi.dll"));
extern HINSTANCE hDLL2;// = LoadLibrary(_T("Iphlpapi.dll"));
int Ping::gui_ping(CChildView* pView, long row)
{
	if (hDLL1)
	{
		// 関数へのポインタを取得
		typedef DWORD (*IcmpSendEchoType)(
			_In_                       HANDLE                   IcmpHandle,
			_In_                       IPAddr                   DestinationAddress,
			_In_reads_bytes_(RequestSize)   LPVOID                   RequestData,
			_In_                       WORD                     RequestSize,
			_In_opt_                   PIP_OPTION_INFORMATION   RequestOptions,
			_Out_writes_bytes_(ReplySize)    LPVOID                   ReplyBuffer,
			_In_range_(>= , sizeof(ICMP_ECHO_REPLY) + RequestSize + 8)
			DWORD                    ReplySize,
			_In_                       DWORD                    Timeout
		);
		typedef HANDLE (*IcmpCreateFileType)(VOID);
		typedef BOOL (*IcmpCloseHandleType)(
			_In_ HANDLE  IcmpHandle
		);
		typedef int (*InetPtonAType)(int   Family,
			PCSTR pszAddrString,
			PVOID pAddrBuf);

		IcmpSendEchoType IcmpSendEcho = (IcmpSendEchoType)GetProcAddress(hDLL1, "IcmpSendEcho");
		IcmpCreateFileType IcmpCreateFile = (IcmpCreateFileType)GetProcAddress(hDLL1, "IcmpCreateFile");
		IcmpCloseHandleType IcmpCloseHandle = (IcmpCloseHandleType)GetProcAddress(hDLL1, "IcmpCloseHandle");
		InetPtonAType InetPtonA = (InetPtonAType)GetProcAddress(hDLL2, "inet_pton");

		if (IcmpSendEcho && IcmpCreateFile && IcmpCloseHandle && InetPtonA)
		{
			stopFlag = false;
			int rowInf = 0;
			//int successCounter;

			while (true) {
				rowInf = pView->GetRow();
				if (rowInf < 0 || stopFlag) {
					break;
				}
				{
					char* gDestination = NULL;
					gDestination = (char*)malloc(1024);
					TCHAR *tszText = pView->rowLists->GetIpAddr(rowInf);
					HANDLE hIcmpFile;
					unsigned long ipaddr = INADDR_NONE;
					DWORD dwRetVal = 0;
					char SendData[32];
					sprintf_s(SendData, 32,"DataBuffer_%s_%d", tszText, rowInf);
					LPVOID ReplyBuffer = NULL;
					DWORD ReplySize = 0;
					//int ipx = validateAddress(tszText);
					int ipx = pView->rowLists->GetIPV(rowInf);
					if (ipx < 0) {
						char Message[256];
						sprintf_s(Message,256, "Invalid Address:%s", tszText);
						AfxMessageBox(Message, MB_OK);
						goto cleanup;
					}
					//ipaddr = inet_addr(argv[1]);
					if (ipx == 1) {
						InetPtonA(AF_INET, _T(tszText), &ipaddr);
					}
					else {
						InetPtonA(AF_INET6, _T(tszText), &ipaddr);
					}
					hIcmpFile = IcmpCreateFile();
					if (hIcmpFile == INVALID_HANDLE_VALUE) {
						goto cleanup;
					}
					ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
					ReplyBuffer = (void*)malloc(ReplySize);
					if (ReplyBuffer == NULL) {
						goto cleanup;
					}
					{
						//CSingleLock lock(&pingCriticalSection, TRUE);
						dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
							NULL,ReplyBuffer,ReplySize,1000);


					}
					if (dwRetVal != 0) 
					{
						PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
						struct in_addr ReplyAddr;
						ReplyAddr.S_un.S_addr = pEchoReply->Address;
						//if (dwRetVal > 1) {

						//}
						//char Message[256];
						//sprintf_s(Message,256, "4");
						//pView->SetState(rowInf, Message);
						//PostMessage(pView->GetSafeHwnd(), WM_CUSTOM_STATUS, rowInf, 4);
						pView->PostMessageA(WM_CUSTOM_STATUS, rowInf, 4);
					}
					else 
					{
						//char Message[256];
						//sprintf_s(Message,256, "0");
						//pView->SetState(rowInf, Message);
						//PostMessage(pView->GetSafeHwnd(), WM_CUSTOM_STATUS, rowInf, 0);
						pView->PostMessageA(WM_CUSTOM_STATUS, rowInf, 0);
					}
					IcmpCloseHandle(hIcmpFile);
					free(ReplyBuffer);
				cleanup:
					//free(argv);
					free(gDestination);
				}
			}
		}
		else
		{
		// 関数が見つからない場合の処理
		}
	}

	return 0;
}
