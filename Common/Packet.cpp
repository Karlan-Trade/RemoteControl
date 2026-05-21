//Creat by Karlan 2026.05.18
#include "pch.h"
#include "Packet.h"
#include <cstring>
#include <cstdlib>
Packet* ParsePacket(char* buffer, int len){
    Packet pck;
    Packet* pck_pt;
    int i = 0;
    for (; i < len; i++){
        if (*(int*)(buffer + i) == 0x55AA77CC){
            pck.header.magic = *(int*)(buffer + i);
            i += 4;
            break;
        }
    }
    if (i >= len || i + 8 > len){
        return nullptr;
    }
    pck.header.cmd = *(int*)(buffer + i); i += 4;
    pck.header.body_len = *(int*)(buffer + i); i += 4;
    if (pck.header.body_len < 0 || pck.header.body_len > len - i){
        return nullptr;
    }
    if (pck.header.body_len == 0){
        pck_pt = (Packet*)malloc(sizeof(PacketHeader));
        if (pck_pt){
            pck_pt->header = pck.header;
        }
        return pck_pt;
    } else if (pck.header.body_len > 0){
        pck_pt = (Packet*)malloc(sizeof(PacketHeader) + pck.header.body_len);
        if (pck_pt){
            memcpy(pck_pt->body, buffer + i, pck.header.body_len);
            pck_pt->header = pck.header;
        }
        return pck_pt;
    }
    return nullptr;
}
Packet* PackPacket(const char* buffer, int len, int cmd) {
    Packet* packet = (Packet*)malloc(sizeof(PacketHeader) + len);
    packet->header.magic = 0x55AA77CC;
    packet->header.cmd = cmd;
    packet->header.body_len = len;
    if (len > 0 && buffer != nullptr) {
        memcpy(packet->body, buffer, len);
    }
    return packet;
}
int GetPacketLen(Packet* pck) {
    if (pck != nullptr) {
        return pck->header.body_len + sizeof(PacketHeader);
    }else {
        return 0;
    }
}
