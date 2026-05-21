//Creat by Karlan 2026.05.21
#include "pch.h"
#include "Config.h"

void GetConfigPath(char* config_path, int size) {
    //获取 exe 同目录下的 config.ini 路径
    GetModuleFileNameA(NULL, config_path, size);
    char* p = strrchr(config_path, '\\');
    if (p) *(p + 1) = '\0';
    strcat_s(config_path, size, "config.ini");
}

void ReadConfigIP(char* ip, int size, const char* default_ip) {
    char config_path[MAX_PATH] = { 0 };
    GetConfigPath(config_path, MAX_PATH);
    GetPrivateProfileStringA("Network", "IP", default_ip, ip, size, config_path);
}

int ReadConfigPort(int default_port) {
    char config_path[MAX_PATH] = { 0 };
    GetConfigPath(config_path, MAX_PATH);
    return GetPrivateProfileIntA("Network", "Port", default_port, config_path);
}

int ReadConfigScreenWidth(int default_width) {
    char config_path[MAX_PATH] = { 0 };
    GetConfigPath(config_path, MAX_PATH);
    return GetPrivateProfileIntA("Screen", "Width", default_width, config_path);
}

int ReadConfigScreenHeight(int default_height) {
    char config_path[MAX_PATH] = { 0 };
    GetConfigPath(config_path, MAX_PATH);
    return GetPrivateProfileIntA("Screen", "Height", default_height, config_path);
}