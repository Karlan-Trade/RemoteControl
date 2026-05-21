//Creat by Karlan 2026.05.21
#pragma once
#include <windows.h>

void GetConfigPath(char* config_path, int size);
void ReadConfigIP(char* ip, int size, const char* default_ip);
int ReadConfigPort(int default_port);
