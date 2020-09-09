#include "iniParser.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int iniManagerGetFieldListfromINI(char fieldList[][MAX_FIELD_LEN], const char *fileName);
static int iniManagerFindFieldFromList(iniManager_t *iniManager, const char *field);

//////////////////////////////////////////////////////////////////////////
/// Local Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

iniManager_t *iniManagerNew(const char *fileName)
{
	iniManager_t *iniManager = (iniManager_t*)malloc(sizeof(iniManager_t));
	if(iniManager == NULL)
	{
		printf("[DEBUG] ini 매니저 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(iniManagerGetFieldListfromINI(iniManager->fieldList, fileName) == FAIL)
	{
		printf("[ERROR] ini 매니저로 필드 가져오기 실패. NULL.\n");
		return NULL;
	}

	int i = 0;
	printf("\n[Load fields from the ini file to the field list]\n");
	for( ; i < 5; i++)
	{
		printf("Loaded field : %s\n", (iniManager->fieldList)[i]);
	}

	return iniManager;
}

void iniManagerDelete(iniManager_t **iniManager)
{
	free(*iniManager);
	*iniManager = NULL;
}

int iniManagerGetValueFromINI(iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
{
	if(iniManagerFindFieldFromList(iniManager, field) == FAIL)
	{
		printf("[ERROR] ini 필드 리스트부터 주어진 필드 검색 실패. (fileName:%s, field:%s)", fileName, field);
		return FAIL;
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)", fileName);
		return FAIL;
	}

	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };
	char keyFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strncmp(fieldFindBuffer, field, strlen(field)) == 0)
		{
			while(fgets(keyFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
			{
				if(strncmp(keyFindBuffer, key, strlen(key)) == 0)
				{
					return atoi(keyFindBuffer + 4);
				}
			}
		}
	}

	fclose(filePtr);
	return defaultValue;
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

static int iniManagerGetFieldListfromINI(char fieldList[][MAX_FIELD_LEN], const char *fileName)
{
	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)", fileName);
		return FAIL;
	}

	int keyIndex = 0;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strchr(fieldFindBuffer, '[') != NULL)
		{
			memcpy(fieldList[keyIndex], fieldFindBuffer, strlen(fieldFindBuffer) - 1);
			keyIndex++;
		}
	}

	fclose(filePtr);
	return SUCCESS;
}

static int iniManagerFindFieldFromList(iniManager_t *iniManager, const char *field)
{
	int fieldIndex = 0;

	for( ; fieldIndex < MAX_FIELD_NUM; fieldIndex++)
	{
		if(strncmp(iniManager->fieldList[fieldIndex], field, strlen(field)) == 0)
		{
			return SUCCESS;
		}
	}

	return FAIL;
}

