#include "iniManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions for iniManager_t
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
 * @brief 지정한 ini 파일에 대한 정보를 관리하기 위한 iniManager_t 객체를 새로 생성하는 함수
 * 외부에서 접근할 수 있는 함수이므로 생성된 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param fileName 점수에 대한 등급 정보를 가지고 있는 ini 파일의 이름(입력, 읽기 전용)
 * @return 새로 생성된 iniManager_t 객체 반환
 */
iniManager_t *iniManagerNew(const char *fileName)
{
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL.\n");
		return NULL;
	}

	iniManager_t *iniManager = (iniManager_t*)malloc(sizeof(iniManager_t));
	if(iniManager == NULL)
	{
		printf("[DEBUG] iniManager 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	printf("\n[ini 파일의 필드 로딩 중...]\n");

	iniManager->fieldMaxNum = 0;
	iniManager->fieldList = NULL;

	if(iniManagerLoadInfoFromINI(iniManager, fileName) == FAIL)
	{
		printf("[로딩 실패]\n\n");
		iniManagerDelete(&iniManager);
		return NULL;
	}

	int fieldIndex = 0;
	for( ; fieldIndex < iniManager->fieldMaxNum; fieldIndex++)
	{
		printf("Loaded field : %s\n", (iniManager->fieldList)[fieldIndex]);
	}
	printf("[로딩 완료]\n");

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
		printf("[DEBUG] iniManager 해제 실패. 객체가 NULL.\n");
		return;
	}

	if((*iniManager)->fieldList != NULL)
	{
		int fieldIndex = 0;
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
 * @param iniManager ini 파일 내용을 저장하는 구조체(입력, 읽기 전용)
 * @param field 키를 찾기 위한 필드 이름(입력, 읽기 전용)
 * @param key 값을 찾기 위한 키 이름(입력, 읽기 전용)
 * @param defaultValue 찾고자 하는 키에 대한 값이 존재하지 않을 때 반환될 값(입력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @param result 함수 실행 성공 여부(출력, 성공 시 SUCCESS, 실패 시 FAIL 저장)
 * @return 성공 시 지정한 필드에 대한 키의 값, 실패 시 FAIL 반환
 */
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName, int *result)
{
	if(iniManager == NULL || field == NULL || key == NULL || fileName == NULL || result == NULL)
	{
		printf("[DEBUG] 매개변수 참조 오류. (iniManager:%p, field:%p, key:%p, fileName:%p, result:%p)\n", iniManager, field, key, fileName, result);
		*result = FAIL;
		return FAIL;
	}

	if(iniManagerFindFieldFromList(iniManager, field) == FAIL)
	{
		printf("[ERROR] ini 필드 리스트부터 주어진 필드 검색 실패. (fileName:%s, field:%s)\n", fileName, field);
		*result = FAIL;
		return FAIL;
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)\n", fileName);
		*result = FAIL;
		return FAIL;
	}

	int returnValue = defaultValue;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	// 1) 전체 필드 검색
	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		// 2) 주어진 필드 일치
		if(strncmp(fieldFindBuffer, field, strlen(field)) == 0)
		{
			char keyFindBuffer[MAX_FIELD_LEN] = { '\0' };

			// 3) 해당 필드에서 키 검색
			while(fgets(keyFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
			{
				if(strchr(keyFindBuffer, '[') != NULL) break;

				// 4) 주어진 키 일치
				if(strncmp(keyFindBuffer, key, strlen(key)) == 0)
				{
					// 5) 해당 키의 값 반환
					// 키가 위치한 문자열에서 값의 위치 : [키 길이] + [등호 길이]
					// ex) min=80 -> 80 은 문자열에서 4 번째 위치에 위치한다.
					char *valuePtr = keyFindBuffer + (strlen(key) + 1);
					int value = atoi(valuePtr);

					// 읽은 문자열의 맨 앞 문자가 숫자 문자가 아니면 defaultValue 반환
					if(isdigit(valuePtr[0]) == 0) break;
					returnValue = value;
					*result = SUCCESS;
					break;
				}
			}
			break;
		}
	}

	if(*result == FAIL) printf("[ERROR] 지정한 필드에 대한 키의 값을 찾을 수 없음. (field:%s, key:%s, fileName:%s)\n", field, key, fileName);
	fclose(filePtr);
	return returnValue;
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static int iniManagerFindFieldFromList(const iniManager_t *iniManager, const char *field)
 * @brief iniManager_t 구조체의 필드 리스트에서 지정한 필드 이름이 존재하는지 검색하는 함수
 * iniManagerGetValueFromField 함수에서 호출되기 때문에 전달받은 구조체 포인터와 필드 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 저장하는 구조체(입력, 읽기 전용)
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

/**
 * @fn static int iniManagerLoadInfoFromINI(iniManager_t *iniManager, const char *fileName)
 * @brief 지정한 ini 파일 내용을 iniManager_t 구조체에 저장하는 함수
 * iniManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터와 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 저장하는 구조체(출력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
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
 * @fn static int iniManagerGetFieldMaxNumfromINI(iniManager_t *iniManager, const char *fileName)
 * @brief 지정한 ini 파일로부터 필드의 전체 개수를 구해서 iniManager_t 구조체에 저장하는 함수
 * iniManagerLoadInfoFromINI 함수에서 호출되기 때문에 전달받은 구조체 포인터와 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 저장하는 구조체(출력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
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
	int result = SUCCESS;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strchr(fieldFindBuffer, '[') != NULL) fieldNum++;
	}

	iniManager->fieldMaxNum = fieldNum;

	if(iniManager->fieldMaxNum == 0)
	{
		printf("[ERROR] 파일 읽기 실패. 파일 내용 또는 필드가 존재하지 않음. (필드 형식:[field])\n");
		result = FAIL;
	}

	fclose(filePtr);
	return result;
}

/**
 * @fn static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
 * @brief 지정한 ini 파일로부터 필드 이름들을 가져와서 iniManager_t 구조체에 저장하는 함수
 * iniManagerLoadInfoFromINI 함수에서 호출되기 때문에 전달받은 구조체 포인터와 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param iniManager ini 파일 내용을 저장하는 구조체(출력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniManagerGetFieldListfromINI(iniManager_t *iniManager, const char *fileName)
{
	int fieldIndex = 0;
	int fieldMaxNum = iniManager->fieldMaxNum;

	iniManager->fieldList = (char**)malloc((size_t)fieldMaxNum * sizeof(char*));
	if(iniManager->fieldList == NULL)
	{
		printf("[DEBUG] 새로 생성한 fieldList 객체가 NULL.\n");
		return FAIL;
	}

	for( ; fieldIndex < fieldMaxNum; fieldIndex++)
	{
		iniManager->fieldList[fieldIndex] = (char*)malloc(MAX_FIELD_LEN * sizeof(char));
		if(iniManager->fieldList == NULL)
		{
			printf("[DEBUG] 새로 생성한 fieldList 의 내부 동적 배열이 NULL.\n");
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

