#include "iniManager.h"

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
		printf("[DEBUG] iniManager 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(iniManagerGetFieldListfromINI(iniManager->fieldList, fileName) == FAIL)
	{
		printf("[ERROR] iniManager 로 필드 가져오기 실패. NULL.\n");
		return NULL;
	}

	printf("\n[Load fields from the ini file to the field list]\n");
	int fieldIndex = 0;
	for( ; fieldIndex < MAX_FIELD_NUM; fieldIndex++)
	{
		printf("Loaded field : %s\n", (iniManager->fieldList)[fieldIndex]);
	}

	return iniManager;
}

void iniManagerDelete(iniManager_t **iniManager)
{
	if(*iniManager == NULL)
	{
		printf("[DEBUG] iniManager 해제 실패. 객체가 NULL. (iniManager:%p)\n", *iniManager);
		return;
	}

	free(*iniManager);
	*iniManager = NULL;
}

int iniManagerGetValueFromField(iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
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
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL. (fileName:%p)", fileName);
		return FAIL;
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)", fileName);
		return FAIL;
	}

	int fieldIndex = 0;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strchr(fieldFindBuffer, '[') != NULL)
		{
			memcpy(fieldList[fieldIndex], fieldFindBuffer, strlen(fieldFindBuffer) - 1);
			fieldIndex++;
		}
	}

	fclose(filePtr);
	return SUCCESS;
}

static int iniManagerFindFieldFromList(iniManager_t *iniManager, const char *field)
{
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 가 NULL. (iniManager:%p)", iniManager);
		return FAIL;
	}

	if(field == NULL)
	{
		printf("[DEBUG] 주어진 field 가 NULL. (field:%p)", field);
		return FAIL;
	}

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

