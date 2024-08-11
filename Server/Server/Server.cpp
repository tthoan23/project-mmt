// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <signal.h>
#include <chrono>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

struct fileInfor {
	string fileName;
	string fileType;
	long long fileSize;
	char buffer[1024];
	long long index;
};

struct fileArrayServer {
	string fileNameServer;
	long long fileSizeServer;
};

struct fileArrayClient {
	string fileNameClient;
	string fileTypeClient;
};

void inforFileArrayServer(vector<fileArrayServer>& arr, string fileArrayName) {
	ifstream iF(fileArrayName.c_str());
	if (!iF.is_open()) {
		cout << "Cannot open file";
		return;
	}
	else {
		string temp;
		fileArrayServer data;
		while (iF >> data.fileNameServer) {
			iF >> temp;
			string numericPart = temp.substr(0, temp.find_first_not_of("0123456789"));
			// Convert the numeric part to an integer
			stringstream ss(numericPart);
			int sizeFile;
			ss >> sizeFile;
			data.fileSizeServer = sizeFile * 1024 * 1024;
			arr.push_back(data);
		}
		iF.close();
	}
}


fileInfor newFile(vector<fileArrayClient> arr, int index, vector<fileArrayServer> ar) {
	int size;
	int n = ar.size();
	for (int i = 0; i < n; i++) {
		if (arr[index].fileNameClient == ar[i].fileNameServer) {
			size = ar[i].fileSizeServer;
			break;
		}
	}
	fileInfor temp;
	temp.fileName = arr[index].fileNameClient;
	temp.fileType = arr[index].fileTypeClient;
	temp.fileSize = size;
	temp.index = 0;
	return temp;
}

void addFile(vector<fileInfor>& head, fileInfor data) {
	int n = head.size();
	int check = 1;
	for (int i = 0; i < n; i++) {
		if (data.fileName == head[i].fileName)
			check = 0;
	}
	if (check)
		head.push_back(data);
}

void transferFileNormal(vector<fileInfor>& data, string file, int index) {
	ofstream oF(file.c_str(), ios::binary | ios::app);
	int sizeAdd = strlen(data[index].buffer);
	oF.write((char*)&(data[index].buffer), 1024);
	oF.close();
}
/*
void analyzeData(vector<fileInfor>& head) {
	int c = 0;
	int n = head.size();
	while (c != n) {
		for (int i = 0; i < n; i++) {
			//Neu gui het file thi bo qua
			if (head[i].index >= head[i].fileSize) {
				c++;
				continue;
			}
			//Bat dau doc file
			ifstream iF((head[i].fileName).c_str(), ios::binary);
			iF.seekg(head[i].index, ios::beg);
			//Xu ly file Normal
			if (head[i].fileType == "NORMAL") {
				if (head[i].index <= (head[i].fileSize - 1024)) {
					iF.read(head[i].buffer, 1024);
					mysock.Send(head[i].buffer, 1024, 0);
				}
				else {
					int lastSize = (head[i].fileSize) - (head[i].index);
					iF.read(head[i].buffer, lastSize);
					mysock.Send(head[i].buffer, lastSize, 0);
				}
				head[i].index += 1024;
			}
			else if (head[i].fileType == "HIGH") {
				for (int j = 0; j < 4; j++) {
					if (head[i].index >= head[i].fileSize) {
						c++;
						break;
					}
					if (head[i].index <= (head[i].fileSize - 1024)) {
						iF.read(head[i].buffer, 1024);
						mysock.Send(head[i].buffer, 1024, 0);
					}
					else {
						int lastSize = (head[i].fileSize) - (head[i].index);
						iF.read(head[i].buffer, lastSize);
						mysock.Send(head[i].buffer, lastSize, 0);
					}
					head[i].index += 1024;
				}
			}
			else if (head[i].fileType == "CRITICAL") {
				for (int j = 0; j < 10; j++) {
					if (head[i].index >= head[i].fileSize) {
						c++;
						break;
					}
					if (head[i].index <= (head[i].fileSize - 1024)) {
						iF.read(head[i].buffer, 1024);
						mysock.Send(head[i].buffer, 1024, 0);
					}
					else {
						int lastSize = (head[i].fileSize) - (head[i].index);
						iF.read(head[i].buffer, lastSize);
						mysock.Send(head[i].buffer, lastSize, 0);
					}
					head[i].index += 1024;
				}
			}
			iF.close();
		}
		if (c != n)
			c = 0;
	}
}*/

vector<fileArrayClient> arrayFileClient;
vector<fileInfor> head;
int checkFileAdd = 0;
int sizeClient;
int nameFileSize;
fileInfor a;
string directoryFile;
int c = 0;
int sizefileInfor;
fileArrayClient t;

DWORD WINAPI function_cal(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);

	int number_continue = 0;

	string fileGetValueServer = "C:/Users/DELL 5570/Documents/Ref/HDTH_DA_Socket/Socket Demo/Chat_1Server_1Client/Source/Server/Server/FileInfor/value.txt";
	vector<fileArrayServer> arrayFileServer;
	inforFileArrayServer(arrayFileServer, fileGetValueServer);
	//Gui danh sach file server
	int n = arrayFileServer.size();
	mysock.Send((char*)&n, sizeof(n), 0);

	for (int i = 0; i < n; i++) {
		int nameFileSize = arrayFileServer[i].fileNameServer.size();
		mysock.Send((char*)&nameFileSize, sizeof(int), 0);
		char temp[100];
		for (int j = 0; j < nameFileSize; j++)
			temp[j] = arrayFileServer[i].fileNameServer[j];
		mysock.Send(temp, nameFileSize, 0);
		int size = arrayFileServer[i].fileSizeServer;
		mysock.Send((char*)&size, sizeof(int), 0);
	}

	char* temp = NULL;
	char* temp1 = NULL;

	while (1) {
		mysock.Receive((char*)&checkFileAdd, sizeof(int), 0);
		//mysock.Receive((char*)&c, sizeof(int), 0);
		//Nhan danh sach file client
		mysock.Receive((char*)&sizeClient, sizeof(int), 0);
		//if (sizeClient == 0 || (checkFileAdd == sizeClient && checkFileAdd == c))
			//continue;

		for (int i = checkFileAdd; i < sizeClient; i++) {
			//Nhan size ten file
			mysock.Receive((char*)&nameFileSize, sizeof(int), 0);
			temp = new char[nameFileSize+1];
			//Nhan ten file
			mysock.Receive((char*)temp, nameFileSize, 0);
			temp[nameFileSize] = '\0';
			string str(temp);
			t.fileNameClient = str;
			delete[] temp;
			//Nhan size loai file
			int fileTypeSize;
			mysock.Receive((char*)&fileTypeSize, sizeof(int), 0);
			temp1 = new char[fileTypeSize+1];
			//Nhan loai file
			mysock.Receive((char*)temp1, fileTypeSize, 0);
			temp1[fileTypeSize] = '\0';
			string str1(temp1);
			t.fileTypeClient = str1;
			delete[] temp1;
			arrayFileClient.push_back(t);
		}

		//Tao bien fileInfor de luu thong tin cua file
		for (int j = checkFileAdd; j < arrayFileClient.size(); j++) {
			a = newFile(arrayFileClient, j, arrayFileServer);
			addFile(head, a);
		}

		//Tien hanh doc file tu server
		directoryFile = "C:/Users/DELL 5570/Documents/Ref/HDTH_DA_Socket/Socket Demo/Chat_1Server_1Client/Source/Server/Server/FileInfor/";
		sizefileInfor = head.size();
		auto start = std::chrono::high_resolution_clock::now();
		while (c != sizefileInfor) {
			for (int i = 0; i < sizefileInfor; i++) {
				//Neu gui het file thi bo qua
				if (head[i].index >= head[i].fileSize) {
					c++;
					continue;
				}
				//Bat dau doc file
				string fileOpen = directoryFile + head[i].fileName;
				ifstream iF(fileOpen.c_str(), ios::binary);
				iF.seekg(head[i].index, ios::beg);
				//Xu ly file Normal
				if (head[i].fileType == "NORMAL") {
					if (head[i].index <= (head[i].fileSize - 1024)) {
						iF.read(head[i].buffer, 1024);
						mysock.Send(head[i].buffer, 1024, 0);
					}
					else {
						int lastSize = (head[i].fileSize) - (head[i].index);
						iF.read(head[i].buffer, lastSize);
						mysock.Send(head[i].buffer, lastSize, 0);
					}
					head[i].index += 1024;
				}
				else if (head[i].fileType == "HIGH") {
					for (int j = 0; j < 4; j++) {
						if (head[i].index >= head[i].fileSize) {
							c++;
							break;
						}
						if (head[i].index <= (head[i].fileSize - 1024)) {
							iF.read(head[i].buffer, 1024);
							mysock.Send(head[i].buffer, 1024, 0);
						}
						else {
							int lastSize = (head[i].fileSize) - (head[i].index);
							iF.read(head[i].buffer, lastSize);
							mysock.Send(head[i].buffer, lastSize, 0);
						}
						head[i].index += 1024;
					}
				}
				else if (head[i].fileType == "CRITICAL") {
					for (int j = 0; j < 10; j++) {
						if (head[i].index >= head[i].fileSize) {
							c++;
							break;
						}
						if (head[i].index <= (head[i].fileSize - 1024)) {
							iF.read(head[i].buffer, 1024);
							mysock.Send(head[i].buffer, 1024, 0);
						}
						else {
							int lastSize = (head[i].fileSize) - (head[i].index);
							iF.read(head[i].buffer, lastSize);
							mysock.Send(head[i].buffer, lastSize, 0);
						}
						head[i].index += 1024;
					}
				}
				iF.close();
			}
			int timing;
			mysock.Receive((char*)&timing, sizeof(int), 0);
			if (timing >= 2) {
				if (c != sizefileInfor)
					c = 0;
				break;
			}
			if (c != sizefileInfor)
				c = 0;
		}
	}
	
	/*do {
		fflush(stdin);
		int number_a, number_b, number_result;
		char letter;
		//Nhan phep toan
		mysock.Receive(&letter, sizeof(letter), 0);
		//Nhan so thu nhat
		mysock.Receive(&number_a, sizeof(number_a), 0);
		//Nhan so thu hai
		mysock.Receive(&number_b, sizeof(number_b), 0);

		//So sanh neu client muon thuc hien phep cong
		if (letter == '+')
			number_result = number_a + number_b;
		else if (letter == '-')
			number_result = number_a - number_b;

		//Gui ket qua tinh toan cho client
		mysock.Send(&number_result, sizeof(number_result), 0);

		//Nhan number xem client co tiep tuc hay khong
		mysock.Receive(&number_continue, sizeof(number_continue), 0);

	} while (number_continue);*/
	delete hConnected;
	return 0;
	//return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		/*string fileGetValueServer = "C:/Users/DELL 5570/Documents/Ref/HDTH_DA_Socket/Socket Demo/Chat_1Server_1Client/Source/Server/Server/FileInfor/value.txt";
		vector<fileArrayServer> arrayFileServer;
		inforFileArrayServer(arrayFileServer, fileGetValueServer);
		int n = arrayFileServer.size();
		int nameFileSize = arrayFileServer[0].fileNameServer.size();
		char* temp = new char[nameFileSize];
		for (int j = 0; j < nameFileSize; j++)
			temp[j] = arrayFileServer[0].fileNameServer[j];
		temp[nameFileSize] = '\0';
		cout << temp << endl;
		for (int i = 0; i < n; i++) {
			cout << arrayFileServer[i].fileNameServer << endl;
		}*/

		// Khoi tao thu vien Socket
		if( AfxSocketInit() == FALSE)
		{ 
			cout<<"Khong the khoi tao Socket Libraray";
			return FALSE; 
		}

		AfxSocketInit(NULL);
			CSocket server, s;
			DWORD threadID;
			HANDLE threadStatus;

			server.Create(4567);
			do {
				printf("Server lang nghe ket noi tu client\n");
				server.Listen();
				server.Accept(s);
				//Khoi tao con tro Socket
				SOCKET* hConnected = new SOCKET();
				//Chuyển đỏi CSocket thanh Socket
				*hConnected	= s.Detach();
				//Khoi tao thread tuong ung voi moi client Connect vao server.
				//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
				threadStatus = CreateThread(NULL, 0, function_cal, hConnected, 0, &threadID);
			}while(1);
/*
		CSocket ServerSocket; //cha
		// Tao socket cho server, dang ky port la 1234, giao thuc TCP
		if(ServerSocket.Create(1234,SOCK_STREAM,NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
		{
			cout << "Khoi tao that bai !!!"<<endl;
			cout << ServerSocket.GetLastError();
			return FALSE;
		}
		else
		{
			cout << "Server khoi tao thanh cong !!!"<<endl;

			if(ServerSocket.Listen(1)==FALSE)
			{
				cout<<"Khong the lang nghe tren port nay !!!"<<endl;
				ServerSocket.Close();
				return FALSE;
			}
		}

		CSocket Connector;
		// Khoi tao mot socket de duy tri viec ket noi va trao doi du lieu
		if (ServerSocket.Accept(Connector))
		{
			cout <<"Da co Client ket Noi !!!"<<endl<<endl;

			char ServerMsg[100];
			int MsgSize;
			char *temp;

			do
			{
				cout <<"Server: ";
				cin.getline(ServerMsg,100);

				MsgSize = strlen(ServerMsg);

				// Gui di do dai thong diep de server biet duoc do dai nay
				Connector.Send(&MsgSize,sizeof(MsgSize),0);

				// Gui di thong diep voi do dai la MsgSize
				Connector.Send(ServerMsg,MsgSize,0);

				//Nhan thong diep tu Client
				Connector.Receive((char*)&MsgSize,sizeof(int),0); // Neu nhan loi thi tra ve la SOCKET_ERROR.		
				temp = new char[MsgSize +1];
				Connector.Receive((char*)temp,MsgSize,0);

				// In thong diep ra
				temp[MsgSize] ='\0';
				cout<<"Client: "<<temp<<endl;
				delete temp;
			}while (1);
		}
		Connector.Close();
		ServerSocket.Close();*/
	}

	return nRetCode;
}
