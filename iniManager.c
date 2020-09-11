#include "iniManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions for iniKey_t
//////////////////////////////////////////////////////////////////////////

static iniKey_t* iniKeyNew(const char *fieldName, const char *keyName);
static void iniKeyDelete(iniKey_t **key);
static int iniKeyGetValue(const iniKey_t *key);
static void iniKeySetValue(iniKey_t *key, int value);
static const char* iniKeyGetName(const iniKey_t *key);

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions for iniField_t
//////////////////////////////////////////////////////////////////////////

static iniField_t* iniFieldNew(const char *fieldName, int keyMaxNum);
static void iniFieldDelete(iniField_t **field);
static iniKey_t* iniFieldGetKeyFromListByName(const iniField_t *field, const char *keyName);
static iniKey_t* iniFieldGetKeyFromListByIndex(const iniField_t *field, int index);
static int iniFieldInitializeKeyList(iniField_t *field, int keyMaxNum);
static int iniFieldStoreKeyInList(iniField_t *field, int keyIndex, const char *keyName, int value);
static const char* iniFieldGetName(const iniField_t *field);

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
 * @return 성공 시 새로 생성된 iniManager_t 구조체 객체, 실패 시 NULL 반환
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
	int keyIndex = 0;

	printf("\n[로딩된 데이터]\n");
	for( ; fieldIndex < iniManager->fieldMaxNum; fieldIndex++)
	{
		int keyMaxNum = ((iniManager->fieldList)[fieldIndex])->keyMaxNum;
		printf("Loaded field : %s\n", iniFieldGetName((iniManager->fieldList)[fieldIndex]));
		for(keyIndex = 0; keyIndex < keyMaxNum; keyIndex++)
		{
			iniKey_t *key = iniFieldGetKeyFromListByIndex((iniManager->fieldList)[fieldIndex], keyIndex);
			printf("\t%s = %d\n", key->name, key->value);
		}
	}
	printf("[로딩 완료]\n");

	return iniManager;
}

/**
 * @fn void iniManagerDelete(iniManager_t **iniManager)
 * @brief 생성된 iniManager_t 구조체 객체의 메모리를 해제하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param iniManager 삭제할 iniManager_t 구조체 객체(입력, 이중 포인터)
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
			iniFieldDelete(&((*iniManager)->fieldList[fieldIndex]));
		}

		free((*iniManager)->fieldList);
		(*iniManager)->fieldList = NULL;
	}

	free(*iniManager);
	*iniManager = NULL;
}

/**
 * @fn int iniManagerGetValueFromField(iniManager_t *iniManager, const char *fieldName, const char *keyName, int defaultValue, const char *fileName)
 * @brief 지정한 필드에 대한 키의 값을 반환하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * field, key, value 에 대한 설명은 헤더 파일에 명시됨.
 * @param iniManager ini 파일 내용을 저장하는 구조체(입력, 읽기 전용)
 * @param fieldName 키를 찾기 위한 필드 이름(입력, 읽기 전용)
 * @param keyName 값을 찾기 위한 키 이름(입력, 읽기 전용)
 * @param defaultValue 찾고자 하는 키에 대한 값이 존재하지 않을 때 반환될 값(입력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @param result 함수 실행 성공 여부(출력, 성공 시 SUCCESS, 실패 시 FAIL 저장)
 * @return 성공 시 지정한 필드에 대한 키의 값, 실패 시 FAIL 반환
 */
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *fieldName, const char *keyName, int defaultValue, const char *fileName, int *result)
{
	if(iniManager == NULL || fieldName == NULL || keyName == NULL || fileName == NULL || result == NULL)
	{
		printf("[DEBUG] 매개변수 참조 오류. (iniManager:%p, field:%p, key:%p, fileName:%p, result:%p)\n", iniManager, fieldName, keyName, fileName, result);
		return FAIL;
	}

	if(iniManagerFindFieldFromList(iniManager, fieldName) == FAIL)
	{
		printf("[ERROR] ini 필드 리스트부터 주어진 필드 검색 실패. (fileName:%s, field:%s)\n", fileName, fieldName);
		return FAIL;
	}

	int fieldIndex = 0;
	int returnValue = defaultValue;
	
	for( ; fieldIndex < iniManager->fieldMaxNum; fieldIndex++)
	{
		// 1. 필드 먼저 찾고
		iniField_t *field = iniManager->fieldList[fieldIndex];
		if(field == NULL)
		{
			printf("[DEBUG] iniField 객체 참조 실패. NULL. (fieldIndex:%d)\n", fieldIndex);
			break;
		}
		if(strncmp(field->name, fieldName, MAX_FIELD_LEN) != 0) continue;
		
		// 2. 키를 찾는다.
		iniKey_t *key = iniFieldGetKeyFromListByName(iniManager->fieldList[fieldIndex], keyName);
		if(key != NULL)
		{
			returnValue = iniKeyGetValue(key);
			*result = SUCCESS;
			break;
		}
	}

	if(*result == FAIL) printf("[ERROR] 지정한 필드에 대한 키의 값을 찾을 수 없음. (field:%s, key:%s, fileName:%s)\n", fieldName, keyName, fileName);
	return returnValue;
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for iniKey_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static iniKey_t* iniKeyNew(const char *fieldName, const char *keyName)
 * @brief 새로운 키 데이터를 iniKey_t 구조체를 통해 생성하는 함수
 * iniFieldStoreKeyInList 함수에서 호출되기 때문에 전달받은 구조체 포인터와 키 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param fieldName iniKey_t 구조체를 소유하는 필드의 이름(입력, 읽기 전용)
 * @param keyName iniKey_t 구조체가 가지는 키의 이름(입력, 읽기 전용)
 * @return 성공 시 새로 생성된 iniKey_t 구조체 객체, 실패 시 NULL 반환
 */
static iniKey_t* iniKeyNew(const char *fieldName, const char *keyName)
{
	iniKey_t *key = (iniKey_t*)malloc(sizeof(iniKey_t));
	if(key == NULL)
	{
		printf("[DEBUG] iniKey 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(strncpy(key->fieldName, fieldName, MAX_FIELD_LEN) == NULL)
	{
		printf("[DEBUG] iniKey 객체에 필드 이름 저장 실패.\n");
		free(key);
		return NULL;
	}

	if(strncpy(key->name, keyName, MAX_KEY_LEN) == NULL)
	{
		printf("[DEBUG] iniKey 객체에 키 이름 저장 실패.\n");
		free(key);
		return NULL;
	}

	return key;
}

/**
 * @fn static void iniKeyDelete(iniKey_t **key)
 * @brief 생성된 iniKey_t 구조체 객체의 메모리를 해제하는 함수
 * iniFieldDelete 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param key 삭제할 iniKey_t 구조체 객체(입력, 이중 포인터)
 * @return 반환값 없음
 */
static void iniKeyDelete(iniKey_t **key)
{
	if(*key == NULL)
	{
		printf("[DEBUG] iniKey 해제 실패. 객체가 NULL.\n");
		return;
	}

	free(*key);
	*key = NULL;
}

/**
 * @fn static int iniKeyGetValue(const iniKey_t *key)
 * @brief 지정한 키에 대해 1:1 대응하는 값을 반환하는 함수
 * iniManagerGetValueFromField 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param key 값을 반환할 iniKey_t 구조체 객체(입력, 읽기 전용)
 * @return 항상 지정한 키의 값을 반환
 */
static int iniKeyGetValue(const iniKey_t *key)
{
	return key->value;
}

/**
 * @fn static void iniKeySetValue(iniKey_t *key, int value)
 * @brief 지정한 키에 대해 1:1 대응하도록 전달받은 값을 저장하는 함수
 * iniFieldStoreKeyInList 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param key 전달받은 값을 저장할 iniKey_t 구조체 객체(출력)
 * @param value 저장할 값(입력)
 * @return 반환값 없음
 */
static void iniKeySetValue(iniKey_t *key, int value)
{
	key->value = value;
}

/**
 * @fn static const char* iniKeyGetName(const iniKey_t *key)
 * @brief 지정한 키를 소유한 필드의 이름을 반환하는 함수
 * iniFieldGetKeyFromListByName 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param key 자신을 소유한 필드의 이름을 저장하고 있는 iniKey_t 구조체 객체(입력, 읽기 전용)
 * @return 반환값 없음
 */
static const char* iniKeyGetName(const iniKey_t *key)
{
	return key->name;
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for iniField_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static iniField_t* iniFieldNew(const char *fieldName, int keyMaxNum)
 * @brief 새로운 필드 데이터를 iniField_t 구조체를 통해 생성하는 함수
 * @param fieldName iniField_t 구조체가 가지는 필드의 이름(입력, 읽기 전용)
 * @param keyMaxNum key list 를 동적으로 생성하기 위해 필요한 list 의 전체 크기(입력)
 * @return 성공 시 새로 생성된 iniField_t 구조체 객체, 실패 시 NULL 반환
 */
static iniField_t* iniFieldNew(const char *fieldName, int keyMaxNum)
{
	iniField_t *field = (iniField_t*)malloc(sizeof(iniField_t));
	if(field == NULL)
	{
		printf("[DEBUG] iniKey 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(strncpy(field->name, fieldName, MAX_FIELD_LEN) == NULL)
	{
		printf("[DEBUG] iniField 객체에 필드 이름 저장 실패.\n");
		free(field);
		return NULL;
	}

	field->keyMaxNum = keyMaxNum;
	if(iniFieldInitializeKeyList(field, field->keyMaxNum) == FAIL)
	{
		free(field);
		return NULL;
	}

	return field;
}
	
/**
 * @fn static void iniFieldDelete(iniField_t **field)
 * @brief 생성된 iniField_t 구조체 객체의 메모리를 해제하는 함수
 * @param field 삭제할 iniField_t 구조체 객체(입력, 이중 포인터)
 * @return 반환값 없음
 */
static void iniFieldDelete(iniField_t **field)
{
	if(*field == NULL)
	{
		printf("[DEBUG] iniField 해제 실패. 객체가 NULL.\n");
		return;
	}

	if((*field)->keyList != NULL)
	{
		int keyIndex = 0;
		for( ; keyIndex < (*field)->keyMaxNum; keyIndex++)
		{
			if(((*field)->keyList)[keyIndex] != NULL)
			{
				iniKeyDelete(&(((*field)->keyList)[keyIndex]));
				((*field)->keyList)[keyIndex] = NULL;
			}
		}
	}

	free(*field);
	*field = NULL;
}

/**
 * @fn static iniKey_t* iniFieldGetKeyFromListByName(const iniField_t *field, const char *keyName)
 * @brief 키의 이름으로 키 리스트에서 키를 찾아 반환하는 함수
 * iniManagerGetValueFromField 함수에서 호출되기 때문에 전달받은 구조체 포인터와 키 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param field 키 리스트를 가지고 있는 iniField_t 구조체 객체(입력, 읽기 전용)
 * @param keyName 키 리스트에서 검색할 키 이름(입력, 읽기 전용)
 * @return 성공 시 검색된 iniKey_t 구조체 객체, 실패 시 NULL 반환
 */
static iniKey_t* iniFieldGetKeyFromListByName(const iniField_t *field, const char *keyName)
{
	int keyIndex = 0;
	int keyMaxNum = field->keyMaxNum;

	for( ; keyIndex < keyMaxNum; keyIndex++)
	{
		if(field->keyList[keyIndex] != NULL)
		{
			if(strncmp(iniKeyGetName(field->keyList[keyIndex]), keyName, strlen(keyName)) == 0)
			{
				return field->keyList[keyIndex];
			}
		}
	}

	return NULL;
}

/**
 * @fn static iniKey_t* iniFieldGetKeyFromListByIndex(const iniField_t *field, int index)
 * @brief 배열 인덱스로 키 리스트에서 키를 찾아 반환하는 함수
 * iniManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param field 키 리스트를 가지고 있는 iniField_t 구조체 객체(입력, 읽기 전용)
 * @param index 키 리스트에 접근하기 위한 배열 인덱스(입력)
 * @return 항상 검색된 iniKey_t 구조체 객체 반환
 */
static iniKey_t* iniFieldGetKeyFromListByIndex(const iniField_t *field, int index)
{
	return field->keyList[index];
}

/**
 * @fn static int iniFieldInitializeKeyList(iniField_t *field, int keyMaxNum)
 * @brief iniField_t 구조체의 멤버 변수인 키 리스트를 초기화하는 함수
 * iniFieldNew 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param field 키 리스트를 새로 초기화할 iniField_t 구조체 객체(출력)
 * @param keyMaxNum 키 리스트를 동적으로 생성하기 위해 필요한 리스트 전체 길이(입력)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniFieldInitializeKeyList(iniField_t *field, int keyMaxNum)
{
	field->keyList = (iniKey_t**)malloc(sizeof(iniKey_t*) * (size_t)keyMaxNum);
	if(field->keyList == NULL)
	{
		printf("[DEBUG] keyList 객체 동적 생성 실패. NULL.\n");
		return FAIL;
	}

	return SUCCESS;
}

/**
 * @fn static int iniFieldStoreKeyInList(iniField_t *field, int keyIndex, const char *keyName, int value)
 * @brief iniField_t 구조체의 멤버 변수인 키 리스트에 지정한 키 데이터를 저장하는 함수
 * iniManagerGetFieldListfromINI 함수에서 호출되기 때문에 전달받은 구조체 포인터와 키 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param field 키 데이터를 저장할 키 리스트를 가진 iniField_t 구조체 객체(출력)
 * @param keyIndex 저장할 키 리스트의 인덱스(입력)
 * @param keyName 저장할 키의 이름(입력, 읽기 전용)
 * @param value 저장할 키의 값(입력)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int iniFieldStoreKeyInList(iniField_t *field, int keyIndex, const char *keyName, int value)
{
	field->keyList[keyIndex] = iniKeyNew(field->name, keyName);
	if(field->keyList[keyIndex] == NULL)
	{
		return FAIL;
	}

	iniKeySetValue(field->keyList[keyIndex], value);
	return SUCCESS;
}

/**
 * @fn static int iniFieldStoreKeyInList(iniField_t *field, int keyIndex, const char *keyName, int value)
 * @brief 지정한 필드의 이름을 반환하는 함수
 * iniManagerFindFieldFromList 와 iniManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param field 자신의 이름을 반환할 필드(입력, 읽기 전용)
 * @return 항상 필드의 이름 반환
 */
static const char* iniFieldGetName(const iniField_t *field)
{
	return field->name;
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
	int fieldMaxNum = iniManager->fieldMaxNum;

	for( ; fieldIndex < fieldMaxNum; fieldIndex++)
	{
		if(iniManager->fieldList[fieldIndex] == NULL) continue;

		const char *fieldName = iniFieldGetName(iniManager->fieldList[fieldIndex]);
		if((fieldName == NULL) || (strlen(fieldName) == 0))
		{
			return FAIL;
		}

		if(strncmp(fieldName, field, strlen(field)) == 0)
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

	iniManager->fieldList = (iniField_t**)malloc(sizeof(iniField_t*) * (size_t)fieldMaxNum);
	if(iniManager->fieldList == NULL)
	{
		printf("[DEBUG] 새로 생성한 fieldList 객체가 NULL.\n");
		return FAIL;
	}

	FILE *filePtr = fopen(fileName, "r");
	if(filePtr == NULL)
	{
		printf("[DEBUG] 파일 읽기 실패. (fileName:%s)\n", fileName);
		return FAIL;
	}

	int result = FAIL;
	char fieldFindBuffer[MAX_FIELD_LEN] = { '\0' };

	while(fgets(fieldFindBuffer, MAX_FIELD_LEN, filePtr) != NULL)
	{
		if(strchr(fieldFindBuffer, '[') != NULL)
		{
			int keyIndex = 0;
			char keyFindBuffer[MAX_FIELD_LEN] = { '\0' };

			fieldFindBuffer[strlen(fieldFindBuffer) - 1] = '\0';
			iniManager->fieldList[fieldIndex] = iniFieldNew(fieldFindBuffer, NUM_OF_KEYS);
			if(iniManager->fieldList[fieldIndex] == NULL)
			{
				break;
			}

			while(fgets(keyFindBuffer, MAX_KEY_LEN, filePtr) != NULL)
			{
				if(strchr(keyFindBuffer, '[') != NULL)
				{
					if(fseek(filePtr, -((long int)(strlen(keyFindBuffer) + 2)), SEEK_CUR) == FAIL)
					{
						printf("[ERROR] fseek 실패.\n");
					}
					break;
				}

				char *key = strtok(keyFindBuffer, "=");
				if(key == NULL)
				{
					printf("[ERROR] 키 저장 실패. 키가 존재하지 않음.\n");
					break;
				}

				char *valuePtr = strtok(NULL, "=");
				if(isdigit(valuePtr[0]) == 0)
				{
					printf("[ERROR] 값 저장 실패. 정수가 아니거나 존재하지 않음.\n");
					break;
				}

				int value = atoi(valuePtr);
				if(iniFieldStoreKeyInList(iniManager->fieldList[fieldIndex], keyIndex, key, value) == FAIL)
				{
					break;
				}

				keyIndex++;
			}

			fieldIndex++;
			if(fieldIndex == fieldMaxNum)
			{
				result = SUCCESS;
				break;
			}
		}
	}

	fclose(filePtr);
	return result;
}

