//Creat by Karlan 2026.05.18
#pragma once
#include <winsock2.h>
#include <windows.h>

#pragma pack(push, 1) //设置结构体内存对齐为1字节
typedef struct {
    int magic;
    int cmd;
    int body_len;
} PacketHeader;
#pragma pack(pop) //恢复默认内存对齐
typedef struct {
    PacketHeader header;
    char body[];
}Packet;
typedef struct{
    int action;
    POINT ptXY;
}Mouse;
typedef struct{
    int virtual_code;
    int key_status;
}KeyBoard;
enum CMD {
    CMD_SCREEN_SHOT = 1,
    CMD_MOUSE = 2,
    CMD_KEYBOARD = 3,
    CMD_CONNECTION = 4
};
enum MOUSE_ACTION{
    MOUSE_MOVE = 1,
    MOUSE_L_DOWN = 2,
    MOUSE_L_UP = 3,
    MOUSE_R_DOWN = 4,
    MOUSE_R_UP = 5,
    MOUSE_M_DOWN = 6,
    MOUSE_M_UP = 7,
    MOUSE_L_CLICK=8,
    MOUSE_R_CLICK=9,
    MOUSE_M_CLICK=10,
    MOUSE_L_DCLICK = 11,
    MOUSE_R_DCLICK = 12,
    MOUSE_M_DCLICK = 13,
};
int GetPacketLen(Packet* pck);
Packet* ParsePacket(char* buffer, int len);
Packet* PackPacket(const char* buffer, int len, int cmd);
