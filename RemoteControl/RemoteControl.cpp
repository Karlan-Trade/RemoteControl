//Creat by Karlan 2026.05.18
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>     
#include <windows.h>
#include <atlimage.h>
#include <ShellScalingAPI.h>
#include "Packet.h"
#include "Config.h"
<<<<<<< HEAD
#define BUFFER_SIZE 1024*1024*10
=======
#define BUFFER_SIZE 1024*1024*1
>>>>>>> 27f33bc5ab031db329aa01522201768b7cbd9578

#pragma comment(lib, "ws2_32.lib")
int InitServer();
int HandleCommand(Packet* packet);
int HandleScreenShot(Packet* packet);
int HandleMouse(Packet* packet);
int HandleKeyboard(Packet* packet);
int HandleConnection(Packet* packet);
SOCKET g_server_socket = INVALID_SOCKET;
SOCKET g_client_socket = INVALID_SOCKET;
SOCKADDR_IN ser_addr;
<<<<<<< HEAD
unsigned long screen_thread_id = 0;
unsigned long mouse_thread_id = 0;
unsigned long keyboard_thread_id = 0;
enum{
	WM_HANDLE_SCREEN = WM_USER + 1,
	WM_HANDLE_MOUSE = WM_USER + 2,
	WM_HANDLE_KEYBOARD = WM_USER + 3,
	WM_START_THREAD = WM_USER + 4,
};
DWORD WINAPI HandleScreenThread(LPVOID lpParameter){
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)){
		if (msg.message == WM_HANDLE_SCREEN){
			Packet* packet = (Packet*)msg.lParam;
			HandleScreenShot(packet);
			free(packet);
		}
	}
	return 0;
}
DWORD WINAPI HandleMouseThread(LPVOID lpParameter){
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)){
        if (msg.message == WM_HANDLE_MOUSE){
			Packet* packet = (Packet*)msg.lParam;
			HandleMouse(packet);
			free(packet);
        }
    }
    return 0;
}
DWORD WINAPI HandleKeyboardThread(LPVOID lpParameter){
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)){
        if (msg.message == WM_HANDLE_KEYBOARD){
			Packet* packet = (Packet*)msg.lParam;
			HandleKeyboard(packet);
			free(packet);
        }
    }
    return 0;
}
=======
>>>>>>> 27f33bc5ab031db329aa01522201768b7cbd9578
int main(){
    if (InitServer()!=0) {
        std::cout<<"启动服务失败"<<std::endl;
        return 0;
    }
<<<<<<< HEAD
	//为每个命令单独创建新线程处理，避免命令处理时间过长导致接收数据阻塞
    CreateThread(nullptr, 0, HandleScreenThread, nullptr, 0, &screen_thread_id);
	CreateThread(nullptr, 0, HandleMouseThread, nullptr, 0, &mouse_thread_id);
	CreateThread(nullptr, 0, HandleKeyboardThread, nullptr, 0, &keyboard_thread_id);
    //启动消息队列
	PostThreadMessage(screen_thread_id, WM_START_THREAD, 0, 0);
	PostThreadMessage(mouse_thread_id, WM_START_THREAD, 0, 0);
	PostThreadMessage(keyboard_thread_id, WM_START_THREAD, 0 , 0);
=======
>>>>>>> 27f33bc5ab031db329aa01522201768b7cbd9578
    //客户端连接
    SOCKADDR_IN client_addr;
    int client_addr_len = sizeof(client_addr);
    std::cout<<"等待客户端连接..."<<std::endl;
    g_client_socket = accept(g_server_socket, (sockaddr*)&client_addr, &client_addr_len);
	std::cout << "客户端连接成功" << std::endl;
    char* buffer = (char*)malloc(BUFFER_SIZE);
    static int idx = 0;
    while (true){
		std::cout << "等待接收数据..." << std::endl;
        int len = recv(g_client_socket, buffer, BUFFER_SIZE-idx, 0);
        if (len > 0) {
            idx += len;
			std::cout << "接收数据成功，数据长度为：" << len << "，当前缓冲区数据长度为：" << idx << std::endl;
        }else if(len ==0){
			std::cout << "客户端已断开连接" << std::endl;
            break;
        }else{
            std::cout << "接收数据失败，错误码：" << WSAGetLastError() << std::endl;
            break;
        }
        if (idx > 0) {
			std::cout << "开始解析数据"<< std::endl;
            Packet* pck = ParsePacket(buffer, idx);
            while (idx > 0 && pck != nullptr){
                if (pck != nullptr){
                    idx -= GetPacketLen(pck);
                    memmove(buffer, buffer + GetPacketLen(pck), idx);
                    HandleCommand(pck);
                }
				pck = ParsePacket(buffer, idx);
            }

        }
    }
	//清理网络环境
	closesocket(g_server_socket);
	closesocket(g_client_socket);
    WSACleanup();
    return 0;
}
int InitServer() {
    //初始化网络环境
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    //创建服务器Socket
    g_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_socket == INVALID_SOCKET) {
        std::cout << "创建服务器Socket失败" << std::endl;
        return -1;
    }
    //服务器socket绑定地址
    ser_addr.sin_family = AF_INET;
    char ip[32] = "192.168.32.129";
    int port = 23333;
    // 从配置文件读取 IP 和端口
    ReadConfigIP(ip, sizeof(ip), "192.168.32.129");
    port = ReadConfigPort(23333);
    ser_addr.sin_port = htons(port);
    ser_addr.sin_addr.S_un.S_addr = inet_addr(ip);
    if (bind(g_server_socket, (sockaddr*)&ser_addr, sizeof(ser_addr)) == SOCKET_ERROR) {
        std::cout << "绑定失败" << std::endl;
        return -2;
    }
    //开启服务器监听
    if (listen(g_server_socket, 1) == SOCKET_ERROR) {
        std::cout << "开启监听失败" << std::endl;
        return -3;
    }
	return 0;
}
int HandleCommand(Packet* packet) {
    int ret = 0;
    switch (packet->header.cmd) {
    case CMD::CMD_SCREEN_SHOT:
<<<<<<< HEAD
		PostThreadMessage(screen_thread_id, WM_HANDLE_SCREEN, 0, (LPARAM)packet);
        //处理屏幕截图命令
        break;
    case CMD::CMD_MOUSE:
		PostThreadMessage(mouse_thread_id, WM_HANDLE_MOUSE, 0, (LPARAM)packet);
        //处理鼠标命令
        break;
    case CMD::CMD_KEYBOARD:
		PostThreadMessage(keyboard_thread_id, WM_HANDLE_KEYBOARD, 0, (LPARAM)packet);
=======
        ret = HandleScreenShot(packet);
        //处理屏幕截图命令
        break;
    case CMD::CMD_MOUSE:
		ret = HandleMouse(packet);
        //处理鼠标命令
        break;
    case CMD::CMD_KEYBOARD:
        ret = HandleKeyboard(packet);
>>>>>>> 27f33bc5ab031db329aa01522201768b7cbd9578
        //处理键盘命令
        break;
    case CMD::CMD_CONNECTION:
        //测试连接
        break;
    default:
        break;
    }
    return ret;
}
int HandleScreenShot(Packet* packet){
    CImage img;
    HDC hScreen = GetDC(NULL); //获取屏幕上下文
    int bitwidth = GetDeviceCaps(hScreen, BITSPIXEL); //获取屏幕像素宽
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);//设置动态感知DPI
    int swidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕宽度
    int sheight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕高度
    img.Create(swidth, sheight, bitwidth); //创建图像对象
    BitBlt(img.GetDC(), 0, 0, swidth, sheight, hScreen, 0, 0, SRCCOPY);
    ReleaseDC(NULL, hScreen);   //释放屏幕上下文
    HGLOBAL hMen = GlobalAlloc(GMEM_MOVEABLE, 0);//申请一块可变化空间
    if (hMen == nullptr){
        std::cout << "创建全局内存对象失败" << std::endl;
        return -1;
    }
    IStream* pStream = nullptr;
    HRESULT ret = CreateStreamOnHGlobal(hMen, TRUE, &pStream);//创建一个流对象
    if (ret == S_OK){
        img.Save(pStream, Gdiplus::ImageFormatPNG); //将图像保存到流对象中
        //将p指针放到开头
        LARGE_INTEGER lg = { 0 };
        pStream->Seek(lg, STREAM_SEEK_SET, nullptr);
        //获取流指针
        char* pdata = (char*)GlobalLock(hMen);
        //获取流大小
        int len = GlobalSize(hMen);
        //发送数据
        Packet* packet = PackPacket(pdata, len, CMD::CMD_SCREEN_SHOT);
        send(g_client_socket, (char*)packet, sizeof(PacketHeader) + len, 0);
        free(packet);
        //释放流对象
        GlobalUnlock(hMen);
    }
    //释放流指针
    pStream->Release();
    //释放图像DC
    img.ReleaseDC();
    return 0;
}
int HandleMouse(Packet* packet){
	Mouse mouse;
	memcpy(&mouse, packet->body, packet->header.body_len);
	std::cout << mouse.ptXY.x << " " << mouse.ptXY.y << " " << mouse.action << std::endl;
	//模拟执行鼠标事件
    SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);//设置鼠标位置
    switch (mouse.action){
	case MOUSE_MOVE:
        SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);//设置鼠标位置
		break;
	case MOUSE_L_DOWN:
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0,0,GetMessageExtraInfo());
		break;
	case MOUSE_L_UP:
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_R_DOWN:
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_R_UP:
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_M_DOWN:
		mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_M_UP:
		mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_L_CLICK:
		mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_R_CLICK:
		mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_M_CLICK:
		mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case MOUSE_L_DCLICK:
        mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0,0,GetMessageExtraInfo());
        mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
        break;
	case MOUSE_R_DCLICK:
        mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
        mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		break;  
	case MOUSE_M_DCLICK:
		mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
        mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		break;
    default:
        break;
    }
    return 0;
}
int HandleKeyboard(Packet* packet){
    KeyBoard keyboard;
    memcpy(&keyboard, packet->body, packet->header.body_len); 
    keybd_event(keyboard.virtual_code, 0, keyboard.key_status, 0);
    std::cout << "执行键盘事件" << keyboard.virtual_code << std::endl;
    return 0;
}

//int HandleKeyboard(Packet* packet){
//	KeyBoard keyboard;
//	memcpy(&keyboard.virtual_code, packet->body, packet->header.body_len);
//    INPUT input{ 0 };
//    input.type = INPUT_KEYBOARD;
//	input.ki.wVk = keyboard.virtual_code;
//    input.ki.wScan = 0;
//    input.ki.dwFlags = keyboard.key_status;
//    input.ki.dwExtraInfo = 0;
//	int ret = SendInput(1, &input, sizeof(INPUT));
//    if (ret > 0){
//		std::cout << "执行键盘事件" <<keyboard.virtual_code <<std::endl;
//    }
//	return 0;
//}