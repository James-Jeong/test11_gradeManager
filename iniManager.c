#include "iniManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int iniManagerLoadInfoFromINI(iniManager_t *iniManager, const char *fileName);
static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName);
static int iniManagerGetFieldMaxNumfromINI(iniManager_t *iniManager, const char *fileName);
static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field);

//////////////////////////////////////////////////////////////////////////
/// Public Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn iniManager_t *iniManagerNew(const char *fileName)
 * @brief ini 파일에 대한 정보를 관리하기 위한 iniManager_t 객체를 새로 생성하는 함수
 * 외부에서 접근할 수 있는 함수이므로 생성된 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param fileName 점수에 대한 등급 정보를 가지고 있는 ini 파일의 이름(입력, 읽기 전용)
 * @return 새로 생성된 iniManager_t 객체 반환
 */
iniManager_t *iniManagerNew(const char *fileName)
{
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL. (fileName:%p)\n", fileName);
		return NULL;
	}

	iniManager_t *iniManager = (iniManager_t*)malloc(sizeof(iniManager_t));
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(iniManagerLoadInfoFromINI(iniManager, fileName) == FAIL)
	{
		free(iniManager);
		return NULL;
	}

	printf("\n[Load fields from the ini file to the field list]\n");
	int fieldIndex = 0;
	for( ; fieldIndex < iniManager->fieldMaxNum; fieldIndex++)
	{
		printf("Loaded field : %s\n", (iniManager->fieldList)[fieldIndex]);
	}

	return iniManager;
}

/**
 * @fn void iniManagerDelete(iniManager_t **iniManager)
 * @brief 생성된 iniManager_t 구조체 객체의 메모리를 해제하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
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

	int fieldIndex = 0;
	if((*iniManager)->fieldList != NULL)
	{
		for( ; fieldIndex < (*iniManager)->fieldMaxNum; fieldIndex++)
		{
			if((*iniManager)->fieldList[fieldIndex] != NULL)
			{
				free((*iniManager)->fieldList[fieldIndex]);
				(*iniManager)->fieldList[fieldIndex] = NULL;
			}
		}

		free((*iniManager)->fieldList);
		(*iniManager)->fieldList = NULL;
	}

	free(*iniManager);
	*iniManager = NULL;
}

/**
 * @fn int iniManagerGetValueFromField(iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
 * @brief 지정한 필드에 대한 키의 값을 반환하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * field, key, value 에 대한 설명은 헤더 파일에 명시됨.
 * @param iniManager ini 파일 내용을 관리하는 구조체(입력, 읽기 전용)
 * @param field 키를 찾기 위한 필드 이름(입력, 읽기 전용)
 * @param key 값을 찾기 위한 키 이름(입력, 읽기 전용)
 * @param defaultValue 찾고자 하는 키에 대한 값이 존재하지 않을 때 반환될 값(입력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 지정한 필드에 대한 키의 값을 반환, 실패 시 FAIL 반환
 */
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName)
{
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 가 NULL. (iniManager:%p)\n", iniManager);
		return FAIL;
	}

	if(field == NULL || key == NULL || fileName == NULL)
	{
		printf("[DEBUG] 매개변수 참조 오류. (field:%p, key:%p, fileName:%p)\n", field, key, fileName);
		return FAIL;
	}

	if(iniManagerFindFieldFromList(iniManager, field) == FAIL)
	{
		printf("[ERROR] ini 필드 리스트부터 주어진 필드 검색 실패. (fileName:%s, field:%s)\n", fileName, field);
		return FAIL;
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)\n", fileName);
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
 * @fn static int iniManagerLoadInfoFromINI(iniManager_t *iniManager, const char *fileName)
 * @brief ini 파일 내용을 iniManager_t 구조체에 저장하는 함수
 * iniManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 관리하는 구조체(출력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerLoadInfoFromINI(iniManager_t *iniManager, const char *fileName)
{
	if(iniManagerGetFieldMaxNumfromINI(iniManager, fileName) == FAIL)
	{
		printf("[ERROR] iniManager 로 필드 전체 개수 저장 실패. NULL.\n");
		return FAIL;
	}

	if(iniManagerGetFieldListfromINI(iniManager, fileName) == FAIL)
	{
		printf("[ERROR] iniManager 로 필드 가져오기 실패. NULL.\n");
		return FAIL;
	}

	return SUCCESS;
}

/**
 * @fn static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
 * @brief ini 파일로부터 필드 이름들을 가져와서 iniManager_t 구조체에 저장하는 함수
 * iniManagerLoadInfoFromINI 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 관리하는 구조체(출력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
{
	int fieldIndex = 0;
	int fieldMaxNum = iniManager->fieldMaxNum;

	iniManager->fieldList = (char**)malloc((size_t)fieldMaxNum * sizeof(char*));
	if(iniManager->fieldList == NULL)
	{
		printf("[DEBUG] 새로 생성한 fieldList 객체가 NULL. (iniManager:%p, iniManager->fieldList:%p)\n", iniManager, iniManager->fieldList);
		return FAIL;
	}

	for( ; fieldIndex < fieldMaxNum; fieldIndex++)
	{
		iniManager->fieldList[fieldIndex] = (char*)malloc(MAX_FIELD_LEN * sizeof(char));
		if(iniManager->fieldList == NULL)
		{
			printf("[DEBUG] 새로 생성한 fieldList 의 내부 동적 배열이 NULL. (iniManager:%p, iniManager->fieldList:%p, iniManager->fieldList[%d]:%p)\n", iniManager, iniManager->fieldList, fieldIndex, iniManager->fieldList[fieldIndex]);
			return FAIL;
		}
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)\n", fileName);
		return FAIL;
	}

	fieldIndex = 0;
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
 * @fn static int iniManagerGetFieldMaxNumfromINI(iniManager_t *iniManager, const char *fileName)
 * @brief ini 파일로부터 필드의 전체 개수를 구해서 iniManager_t 구조체에 저장하는 함수
 * iniManagerLoadInfoFromINI 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 관리하는 구조체(출력)
 * @param fileName 등급에 대한 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerGetFieldMaxNumfromINI(iniManager_t *iniManager, const char *fileName)
{
	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)\n", fileName);
		return FAIL;
	}

	int fieldNum = 0;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strchr(fieldFindBuffer, '[') != NULL) fieldNum++;
	}

	iniManager->fieldMaxNum = fieldNum;

	if(iniManager->fieldMaxNum == 0)
	{
		printf("[ERROR] 파일 읽기 실패. 파일 내용 또는 필드가 존재하지 않음. (필드 형식:[field])\n");
		fclose(filePtr);
		return FAIL;
	}

	fclose(filePtr);
	return SUCCESS;
}

/**
 * @fn static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field)
 * @brief iniManager_t 구조체의 필드 리스트에서 지정한 필드 이름이 존재하는지 검색하는 함수
 * iniManagerGetValueFromField 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 관리하는 구조체(입력, 읽기 전용)
 * @param field 검색할 필드 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field)
{
	int fieldIndex = 0;
	for( ; fieldIndex < iniManager->fieldMaxNum; fieldIndex++)
	{
		if(strncmp(iniManager->fieldList[fieldIndex], field, strlen(field)) == 0)
		{
			return SUCCESS;
		}
	}

	return FAIL;
}

