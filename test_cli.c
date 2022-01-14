/**
 * @file test_cli.c
 * @author qiaoqiming
 * @brief cli
 * @version 0.9
 * @date 2019-06-06
 */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "UARTCommandConsole.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.9.2"

static BaseType_t prvUnameCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	strcpy(pcWriteBuffer, "FreeRTOS");

	return pdFALSE;
}

static const CLI_Command_Definition_t xUname =
{
	"uname",
	"uname:\t\t\tEchos uname in turn\r\n",
    NULL,
	prvUnameCommand,
	0
};

static BaseType_t prvEchoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pcParameter;
	BaseType_t xParameterStringLength;

	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

	configASSERT(pcParameter);

	memset(pcWriteBuffer, 0x00, xWriteBufferLen);
	strncat(pcWriteBuffer, pcParameter, (size_t)xParameterStringLength);

	return pdFALSE;
}

static const CLI_Command_Definition_t xEcho =
{
	"echo",
	"echo:\t\t\tEchos each in turn\r\n",
	"echo\r\n\
	<param>:\t\techos string\r\n",
	prvEchoCommand,
	1
};

static BaseType_t prvPsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *const pcHeader = "\tState\tPRI\tStack\t#\r\n****************************************************\r\n";
	BaseType_t xSpacePadding;

	strcpy(pcWriteBuffer, "Task");
	pcWriteBuffer += strlen(pcWriteBuffer);

	configASSERT(configMAX_TASK_NAME_LEN > 3);
	for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3);xSpacePadding++)
	{
		*pcWriteBuffer = ' ';
		pcWriteBuffer++;

		*pcWriteBuffer = 0x00;
	}
	strcpy(pcWriteBuffer, pcHeader);
	vTaskList(pcWriteBuffer + strlen(pcHeader));
	pcWriteBuffer[strlen(pcWriteBuffer) - 2] = '\0';

	return pdFALSE;
}

static const CLI_Command_Definition_t xPs =
{
	"ps",
	"ps:\t\t\tshowing the state of each FreeRTOS task\r\n",
    NULL,
	prvPsCommand,
	0
};

static BaseType_t prvFreeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	size_t heapInfo;

	heapInfo = xPortGetFreeHeapSize();
	iCliSprintf(pcWriteBuffer, "FreeRTOS HEAP \talloc: %d free: %d", configTOTAL_HEAP_SIZE - heapInfo, heapInfo);

	return pdFALSE;
}

static const CLI_Command_Definition_t xFree =
{
	"free",
	"free:\t\t\tshowing the mem of each FreeRTOS heap\r\n",
    NULL,
	prvFreeCommand,
	0
};

static BaseType_t prvVersionCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	extern const char *const pcWelcomeMessage;
	strcpy(pcWriteBuffer, pcWelcomeMessage);
	strcat(pcWriteBuffer, "\r\n");
	iCliSprintf(pcWriteBuffer+strlen(pcWriteBuffer),"verison %s  ",VERSION);
	iCliSprintf(pcWriteBuffer+strlen(pcWriteBuffer),"build in %s",__DATE__);
	return pdFALSE;
}

static const CLI_Command_Definition_t xVersion =
{
	"version",
	"version:\t\tshowing version\r\n",
    NULL,
	prvVersionCommand,
	0
};

void vRegisterCLICommands(void)
{
	FreeRTOS_CLIRegisterCommand(&xVersion);
	FreeRTOS_CLIRegisterCommand(&xUname);
	FreeRTOS_CLIRegisterCommand(&xEcho);
	FreeRTOS_CLIRegisterCommand(&xPs);
	FreeRTOS_CLIRegisterCommand(&xFree);
}
