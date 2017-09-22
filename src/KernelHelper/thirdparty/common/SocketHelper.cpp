
#include "SocketHelper.h"
#include "TCPSocket.h"

#include <thread>

CWebServicesHandler::CWebServicesHandler()
{
	this->SetAutoReConnect(false);
	this->SetRecvFlag(false);
	m_clientSocket = INVALID_SOCKET;
}

CWebServicesHandler::~CWebServicesHandler()
{

}

int CWebServicesHandler::Init(const char *cServerAddr, int nServerPort, bool bAutoReConnect/* = false*/)
{
	this->SetAutoReConnect(bAutoReConnect);
	this->SetServerAddr(cServerAddr);
	this->SetServerPort(nServerPort);
		
	ClientInit();

	SetRecvFlag(true);

	//���������߳�
	StartRecvDataThread();

	return 0;
}

void CWebServicesHandler::ClientInit()
{
	ClientSockInitialize();
	ClientSetSockOptions();
	ClientSetSockAddrInf();
	ClientExecuteConnect();
}

void CWebServicesHandler::ClientSockInitialize()
{
	m_clientReConnectSocket = Tcp_SockInitialize();
}

void CWebServicesHandler::ClientSetSockOptions()
{
	//Ĭ�ϲ����ó�ʱ
	Tcp_SetSockOptions(m_clientSocket, 0, 0, TRUE);
}

void CWebServicesHandler::ClientSetSockAddrInf()
{
	Tcp_SetSockAddrInf(this->m_strServerAddr.c_str(), this->m_nServerPort, this->m_serverSockAddrIn);
}
void CWebServicesHandler::ClientExecuteConnect()
{
	m_clientSocket = Tcp_Connect(m_clientReConnectSocket, this->m_serverSockAddrIn);
}

int CWebServicesHandler::Exit()
{
	SetRecvFlag(false);	
	SetAutoReConnect(false);
	while (this->GetReConnecting())
	{
		Sleep(1);
	}
			
	Tcp_Exit(m_clientSocket);
	m_clientSocket = INVALID_SOCKET;

	return 0;
}

void CWebServicesHandler::RecvDataFromApp(void * p)
{
	SOCKET clientSocket = INVALID_SOCKET;
	char cResult[16384] = { 0 };
	int nResult = 0;
	CWebServicesHandler *pWSH = (CWebServicesHandler *)p;
	if (pWSH)
	{
		//�Զ�������δ��ʼ
		pWSH->SetReConnecting(false);
		while (pWSH->GetRecvFlag())
		{
			clientSocket = pWSH->GetClientSocket();
			if (clientSocket != INVALID_SOCKET)
			{
				memset(cResult, 0, sizeof(cResult));
				nResult = Tcp_Recv(clientSocket, cResult, sizeof(cResult));
				if (!pWSH->GetRecvFlag())
				{
					break;
				}
				if (nResult > 0)
				{
#ifdef DEBUG
					printf("Bytes received: %d-%ws\n", nResult, UTF8ToUnicode(cResult).c_str());
#endif // DEBUG
					printf("Bytes received: %d-%s\n", nResult, cResult);
				}
				else //���nResult <= 0
				{
					//���õ�ǰSocketΪ��Чֵ
					pWSH->SetClientSocket(INVALID_SOCKET);

					//�������ڽ�������
					pWSH->SetReConnecting(true);
				}
			}
			else
			{
				//�������ڽ�������
				pWSH->SetReConnecting(true);
			}
			
			//�ж��Ƿ��������Զ�����
			if (pWSH->GetAutoReConnect())
			{
				//��ʼ��
				pWSH->ClientSockInitialize();
				//����ѡ��
				pWSH->ClientSetSockOptions();
				//����Զ���������
				while (pWSH->GetReConnecting() && pWSH->GetAutoReConnect() && pWSH->GetRecvFlag() && pWSH->GetClientSocket() == INVALID_SOCKET)
				{
					pWSH->ClientExecuteConnect();
				}
				
				std::string strCommand = "";
				pWSH->RequestCommand(strCommand.c_str(), strCommand.length());

				//�Զ������Ѿ�����
				pWSH->SetReConnecting(false);
			}
			else
			{
				break;
			}
		}
	}
}

int CWebServicesHandler::StartRecvDataThread()
{
	std::thread mythread(&CWebServicesHandler::RecvDataFromApp, this);
	mythread.detach();

	return 0;
}

int CWebServicesHandler::RequestCommand(const char * pCommand, int nCommandLength)
{
	return Tcp_Send(m_clientSocket, pCommand, nCommandLength);
}

