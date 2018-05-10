#ifndef KEY_CONFIG_H
#define KEY_CONFIG_H
#include "nes/Controller.h"

int getDefaultKey(Button btn);
const char *getButtonName(Button btn);
void getKeyNameByCode(int code, char *buffer, int size);
#endif