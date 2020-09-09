#ifndef __INI_PARSER_H__
#define __INI_PARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SUCCESS	1
#define FAIL	-1

#define MAX_FIELD_NUM	5
#define MAX_FIELD_LEN	64

typedef struct iniManager_s iniManager_t;
struct iniManager_s
{
	char fieldList[MAX_FIELD_NUM][MAX_FIELD_LEN];
};

iniManager_t *iniManagerNew(const char *fileName);
void iniManagerDelete(iniManager_t **iniManager);

int iniManagerGetValueFromINI(iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName);

#endif // #ifndef __INI_PARSER_H__
