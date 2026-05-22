// Creat by Karlan 2026.05.18
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h> 
#include <windows.h>
#include <string>
#include <atlimage.h>
#include "Packet.h"
#include "Config.h"
#define BUFFER_SIZE 1024 * 1024 * 10
#pragma comment(lib, "ws2_32.lib")
int InitWindow(HINSTANCE hInstance, int nCmdShow);
int InitSocket();
SOCKET g_server_socket;
SOCKET g_client_socket;
SOCKADDR_IN g_server_addr;
HWND g_hwnd = NULL;
CImage g_img;
int g_remote_width = -1, g_remote_height = -1;
CRITICAL_SECTION g_cri_sec;
void Mouse_Action(WPARAM wParam, LPARAM lParam, HWND hwnd, int cmd);
DWORD WINAPI SendScreenCallBack(LPVOID lpThreadParameter)
{
    char *recv_buffer = (char *)malloc(BUFFER_SIZE);
    while (true)
    {
        Packet *packet = PackPacket(NULL, 0, CMD::CMD_SCREEN_SHOT);
        // 发送获取屏幕请求
        send(g_server_socket, (char *)packet, GetPacketLen(packet), 0);
        free(packet);
        // 等待接收数据
        int len = recv(g_server_socket, recv_buffer, BUFFER_SIZE, 0);
        if (len > 0)
        {
            Packet *packet = ParsePacket(recv_buffer, len);
            if (packet != NULL)
            {
                HGLOBAL hMen = GlobalAlloc(GMEM_MOVEABLE, 0);
                if (hMen == NULL)
                {
                    continue;
                }
                IStream *pStream = nullptr;
                HRESULT ret = CreateStreamOnHGlobal(hMen, true, &pStream);
                if (ret == S_OK)
                {
                    ULONG lenght = 0;
                    pStream->Write(packet->body, packet->header.body_len, &lenght);
                    free(packet);
                    LARGE_INTEGER lg = {0};
                    pStream->Seek(lg, STREAM_SEEK_SET, nullptr);
                    EnterCriticalSection(&g_cri_sec); // 进入临界区
                    if (!g_img.IsNull())
                    {
                        g_img.Destroy();
                    }
                    g_img.Load(pStream);
                    if (g_remote_width == -1 && g_remote_height == -1)
                    {
                        g_remote_width = g_img.GetWidth();
                        g_remote_height = g_img.GetHeight();
                    }
                    LeaveCriticalSection(&g_cri_sec);    // 离开临界区
                    InvalidateRect(g_hwnd, NULL, FALSE); // 使窗口无效，触发重绘
                    UpdateWindow(g_hwnd);
                }
            }
        }
    }
}
LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static ULONGLONG mouse_tick = GetTickCount64(); // 记录上次鼠标事件的时间戳
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (!g_img.IsNull())
        {
            RECT client_rect;
            GetClientRect(hwnd, &client_rect);
            // 计算窗口客户区宽高
            int client_width = client_rect.right - client_rect.left;
            int client_height = client_rect.bottom - client_rect.top;
            // 获取远程屏幕宽高
            int oldMode = SetStretchBltMode(hdc, HALFTONE); // 设置拉伸模式为高质量
            SetBrushOrgEx(hdc, 0, 0, NULL);                 // 设置画刷原点
            EnterCriticalSection(&g_cri_sec);               // 进入临界区
            int remote_width = g_img.GetWidth();
            int remote_height = g_img.GetHeight();
            g_img.StretchBlt(hdc, 0, 0, client_width, client_height, 0, 0, remote_width, remote_height, SRCCOPY);
            LeaveCriticalSection(&g_cri_sec); // 离开临界区
            // 恢复原来的拉伸模式
            SetStretchBltMode(hdc, oldMode);
        }
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_MOUSEMOVE:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_MOVE);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_L_DOWN);
        break;
    }
    case WM_LBUTTONUP:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_L_UP);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_R_DOWN);
        break;
    }
    case WM_RBUTTONUP:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_R_UP);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_M_DOWN);
        break;
    }
    case WM_MBUTTONUP:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_M_UP);
        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_L_DCLICK);
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        Mouse_Action(wParam, lParam, hwnd, MOUSE_R_DCLICK);
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        int v_code = wParam;
        KeyBoard kb;
        kb.virtual_code = v_code;
        kb.key_status = 0; // 按下
        Packet *packet = PackPacket((char *)&kb, sizeof(KeyBoard), CMD::CMD_KEYBOARD);
        send(g_server_socket, (char *)packet, GetPacketLen(packet), 0);
        free(packet);
        break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        KeyBoard kb;
        kb.virtual_code = wParam;
        kb.key_status = KEYEVENTF_KEYUP; // 2，弹起
        Packet *p = PackPacket((char *)&kb, sizeof(KeyBoard), CMD::CMD_KEYBOARD);
        send(g_server_socket, (char *)p, GetPacketLen(p), 0);
        free(p);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }
        return 0;
    }
}
int WINAPI WinMain(HINSTANCE hInstance,     // 当前实例句柄
                   HINSTANCE hPrevInstance, // 前一个实例句柄
                   PSTR pCmdLine,
                   int nCmdShow)
{
    InitializeCriticalSection(&g_cri_sec);
    InitWindow(hInstance, nCmdShow);
    // 连接服务器
    InitSocket();
    unsigned long send_screen_thread_id = 0;
    if (connect(g_server_socket, (SOCKADDR *)&g_server_addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        OutputDebugString("连接服务器失败\r\n");
        return 0;
    }
    OutputDebugString("连接服务器成功\r\n");
    HANDLE handle_send_screen = CreateThread(NULL, 0, SendScreenCallBack, NULL, 0, &send_screen_thread_id); // 创建线程发送屏幕数据
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
int InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS ws = {0};
    LPCSTR CLASS_NAME = "MainWindow";
    ws.lpfnWndProc = winProc; // 窗口消息处理函数
    ws.hInstance = hInstance; // 实例句柄
    ws.lpszClassName = CLASS_NAME;
    ws.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    ws.hCursor = LoadCursor(nullptr, IDC_ARROW);    // 光标
    ws.hIcon = LoadIconA(nullptr, IDI_APPLICATION); // 图标
    ws.style = CS_HREDRAW | CS_VREDRAW;             // 窗口大小变化时重置窗口
    if (!RegisterClass(&ws))
    {
        MessageBox(nullptr, "注册窗口类失败", "错误", MB_OK | MB_ICONERROR);
        return 0;
    }
    // 创建窗口
    g_hwnd = CreateWindow(
        CLASS_NAME,                   // 窗口类名
        "远程控制",                   // 窗口标题
        WS_OVERLAPPEDWINDOW,          // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT, // 窗口坐标x,y位置
        ReadConfigScreenWidth(800), ReadConfigScreenHeight(600),                     // 窗口宽高
        NULL,                         // 父窗口句柄
        NULL,                         // 菜单句柄
        hInstance,                    // 实例句柄
        NULL);                        // 附加参数
    if (g_hwnd == NULL)
    {
        MessageBox(nullptr, "创建窗口失败", "错误", MB_OK | MB_ICONERROR);
        return 0;
    }
    // 显示窗口
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
}
int InitSocket()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    // 创建Socket连接
    g_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_socket == INVALID_SOCKET)
    {
        std::cout << "创建Socket失败" << std::endl;
        return 0;
    }
    g_server_addr.sin_family = AF_INET;
    char ip[32] = "192.168.32.129";
    int port = 23333;
    // 从配置文件读取 IP 和端口
    ReadConfigIP(ip, sizeof(ip), "192.168.32.129");
    port = ReadConfigPort(23333);

    g_server_addr.sin_port = htons(port);
    g_server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
    return 0;
}
void Mouse_Action(WPARAM wParam, LPARAM lParam, HWND hwnd, int cmd)
{
    static ULONGLONG last_move_tick = 0;
    int xPos = LOWORD(lParam); // 获取鼠标x坐标
    int yPos = HIWORD(lParam); // 获取鼠标y坐标
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    int client_width = client_rect.right - client_rect.left;
    int client_height = client_rect.bottom - client_rect.top;
    if (g_remote_height != -1 && g_remote_width != -1)
    {
        int rxPos = xPos * g_remote_width / client_width;
        int ryPos = yPos * g_remote_height / client_height;
        Mouse mouse;
        mouse.action = cmd;
        mouse.ptXY.x = rxPos;
        mouse.ptXY.y = ryPos;
        //if (cmd == MOUSE_MOVE)
        //{
            //ULONGLONG now = GetTickCount64();
            //if (now - last_move_tick < 100)
                //return;
            //last_move_tick = now;
        //}
        Packet *packet = PackPacket((char *)&mouse, sizeof(Mouse), CMD::CMD_MOUSE);
        send(g_server_socket, (char *)packet, GetPacketLen(packet), 0);
        free(packet);
    }
}
