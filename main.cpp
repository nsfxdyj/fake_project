// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <set>

#pragma comment (lib, "Ws2_32.lib")


int id;
std::string s[10];
std::string s1[10];
bool Host[10];

int getid() {
    return id++;
}

class Mes {
public:
    int waitTime;
    int from, to;
    char con[101];
    bool friend operator< (const Mes& oper1, const Mes& oper2) {
        return oper1.waitTime != oper2.waitTime ? oper1.waitTime < oper2.waitTime :
            oper1.from < oper2.from;
    }
};

int Mux;
Mes queue[7000010];
int head, tail;

int sending;
Mes sendM;

void Mess()
{
    srand(time(NULL));


    Mux = 1;
    while (true) {
        if (sending) {
            sending -= 50;

            if (sending == 0) {
                s1[sendM.from] = "No Message to send.";
                Mux = 0;
                for (int i = head; i < tail; i++)
                    queue[i].waitTime += 1000;
                Mux = 1;
            }

            if (sending == 2000)
                s[sendM.to] = sendM.con;
        }

        while (!Mux);
        Mux = 0;
        std::set <Mes> Set;
        int temptail = tail;
        for (; head < temptail; head++) {
            queue[head].waitTime -= 50;
            if (queue[head].waitTime <= 0) {
                Set.insert(queue[head]);
            }
            else {
                queue[tail++] = queue[head];
            }
        }

        bool flag = true;
        for (auto it : Set) {
            if (flag && sending == 0) {
                sending = 3000;
                sendM = it;
                flag = false;

                printf("Sent     from:%d,to:%d,content:%s\n", it.from, it.to, it.con);
                s1[it.from] = it.con;
            }
            else {
                Mes temp = it;
                temp.waitTime += rand() % 3000 + 1000;
                printf("Wait     from:%d,to:%d,content:%s\n", it.from, it.to, it.con);
                queue[tail++] = temp;
            }
        }
        Mux = 1;
        Sleep(50);
    }
}

DWORD WINAPI ThreadProc(
    __in  LPVOID lpParameter
)
{
    SOCKET AcceptSocket = (SOCKET)lpParameter;
    int num = getid();
    char recvBuf[101];
    while (1) {
        int count = recv(AcceptSocket, recvBuf, 100, 0);
        if (count == 0)break;

        Mes temp;
        temp.from = num;
        temp.to = recvBuf[0] - '0';
        temp.waitTime = 0;
        int i;
        temp.con[0] = '1' + num;
        temp.con[1] = ':';
        for (i = 0; recvBuf[i] != '\0'; i++) {
            temp.con[i + 2] = recvBuf[i];
        }
        temp.con[i + 2] = '\0';

        //std::cout << Mux << std::endl;
        s1[num] = "Pending to send.";
        while (!Mux);
        Mux = 0;
        queue[tail++] = temp;
        Mux = 1;
        //printf("\n%d ：%s\n", num, recvBuf);
    }

    closesocket(AcceptSocket);
    return 0;
}

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int imgui() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("模拟CSMA/CD协议"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("模拟CSMA/CD协议161810233丁彦杰"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        
        ImGui::Begin("Channel");

        if (sending) {
            ImGui::Text("Sending from channel %d to channel %d",sendM.from + 1,sendM.to + 1);
            ImGui::Text("Content is");
            ImGui::SameLine();
            ImGui::Text(sendM.con);
        }
        else {
            ImGui::Text("Now the channel is free");
        }
        ImGui::End();

        char HostName[] = "Host0\0";
        for (int i=0;i<8;i++)
        {
            Host[i] = true;
            HostName[5] = i + '1';
            ImGui::Begin(HostName, Host+i);
            ImGui::Text("Recvive State:");
            if (s[i].length() == 0) {
                ImGui::Text("No message received.");
            }
            else {
                ImGui::Text("Recived Message:");
                ImGui::Text(s[i].c_str());
            }
            ImGui::Text("Send State:");
            if (s1[0].length() == 0) {
                ImGui::Text("No message to send.");
            }
            else {
                if (s1[i][0] != 'P' && s1[i][0]!='N')
                    ImGui::Text("Sending Message:");
                ImGui::Text(s1[i].c_str());
            }

            ImGui::End();
        }


        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

int main(int, char**)
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %ld\n", iResult);
        return 1;
    }

    SOCKET ListenSocket;
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrServer.sin_port = htons(20001);

    if (bind(ListenSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
        wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, 5) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    HANDLE fThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Mess, 0, 0, 0);
    HANDLE tThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)imgui, 0, 0, 0);

    if (fThread == NULL)
    {
        wprintf(L"Thread Creat Failed!\n");
        return 1;
    }


    std::cout << "ok1";
    while (1)
    {
        SOCKET AcceptSocket = accept(ListenSocket, (SOCKADDR*)&addrClient, &len);
        if (AcceptSocket == INVALID_SOCKET)break;

        DWORD dwThread;
        HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)AcceptSocket, 0, &dwThread);
        if (hThread == NULL)
        {
            closesocket(AcceptSocket);
            wprintf(L"Thread Creat Failed!\n");
            break;
        }

        CloseHandle(hThread);
    }

    CloseHandle(fThread);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
