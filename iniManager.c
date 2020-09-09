#include "iniManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName);
static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field);

//////////////////////////////////////////////////////////////////////////
/// Public Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn iniManager_t *iniManagerNew(const char *fileName)
 * @brief iniManager_t 객체를 새로 생성하는 함수
 * @param fileName 점수에 대한 등급 정보를 가지고 있는 ini 파일의 이름(입력, 읽기 전용)
 * @return 새로 생성된 iniManager_t 객체 반환
 */
iniManager_t *iniManagerNew(const char *fileName)
{
	iniManager_t *iniManager = (iniManager_t*)malloc(sizeof(iniManager_t));
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(iniManagerGetFieldListfromINI(iniManager, fileName) == FAIL)
	{
		printf("[ERROR] iniManager 로 필드 가져오기 실패. NULL.\n");
		free(iniManager);
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

/**
 * @fn void iniManagerDelete(iniManager_t **iniManager)
 * @brief 생성된 iniManager_t 구조체 객체의 메모리를 해제하는 함수
 * @param iniManager 삭제할 iniManager_t 구조체 객체(입력)
 * @return 반환값 없음
 */
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

/**
 * @fn int iniManagerGetValueFromField(iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
 * @brief 지정한 필드에 대한 키의 값을 반환하는 함수
 * @param iniManager ini 파일에 있는 등급의 필드 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param field 키를 찾기 위한 필드 이름(입력, 읽기 전용)
 * @param key 값을 찾기 위한 키 이름(입력, 읽기 전용)
 * @param defaultValue 찾고자 하는 value 에 대한 키가 존재하지 않을 때 반환될 값(입력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 지정한 필드에 대한 키의 값을 반환, 실패 시 FAIL 반환
 */
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
{
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 가 NULL. (iniManager:%p)", iniManager);
		return FAIL;
	}

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
					return atoi(keyFindBuffer + VALUE_POS);
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

/**
 * @fn static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
 * @brief ini 파일로부터 필드 이름들을 가져와서 iniManager_t 구조체에 저장하는 함수
 * @param iniManager ini 파일에 대한 정보를 저장하기 위한 구조체(출력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
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
			memcpy((iniManager->fieldList)[fieldIndex], fieldFindBuffer, strlen(fieldFindBuffer) - 1);
			fieldIndex++;
		}
	}

	fclose(filePtr);
	return SUCCESS;
}

/**
 * @fn static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field)
 * @brief iniManager_t 구조체의 필드 리스트에서 지정한 필드 이름이 존재하는지 검색하는 함수
 * @param iniManager ini 파일에 대한 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param field 검색할 필드 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field)
{
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

