#ifndef __INI_PARSER_H__
#define __INI_PARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////////////////
/// Definitions & Macros
//////////////////////////////////////////////////////////////////////////

// 함수 실행 성공
#define SUCCESS	1
// 함수 실행 실패
#define FAIL	-1

// ini 파일에서의 키 이름 최대 길이
#define MAX_KEY_LEN		64
// ini 파일에서의 필드 이름 최대 길이
#define MAX_FIELD_LEN	64
// 필드에 지정된 키의 개수
#define NUM_OF_KEYS		2

/**
 * @struct iniKey_t
 * @brief 필드에 대한 키와 키에 대한 값을 관리하는 구조체
 */
typedef struct iniKey_s iniKey_t;
struct iniKey_s
{
	// 해당 키 정보를 소유한 필드 이름
	char fieldName[MAX_FIELD_LEN];
	// 키 이름
	char name[MAX_KEY_LEN];
	// 키와 1:1 대응하는 값
	int value;
};

/**
 * @struct iniField_t
 * @brief ini 파일에서 키에 대한 정보를 식별하기 위한 필드를 관리하는 구조체
 */
typedef struct iniField_s iniField_t;
struct iniField_s
{
	// 필드 이름
	char name[MAX_FIELD_LEN];
	// 필드가 가지고 있는 키의 전체 개수
	int keyMaxNum;
	// 해당 필드에 대한 키와 키에 대한 값을 저장
	iniKey_t **keyList;
};

/**
 * @struct iniManager_t
 * @brief 점수에 대한 등급 정보를 가지는 ini 파일에 담긴 모든 필드 이름을 저장하는 구조체
 * <ini 파일 구성>
 * [field] -> 등급 이름
 * (key)=(value) -> key : 최대 또는 최소값을 식별하는 이름 / value : 해당 key 의 값
 */
typedef struct iniManager_s iniManager_t;
struct iniManager_s
{
	// iniManager 가 가지고 있는 필드 전체 개수
	int fieldMaxNum;
	// ini 파일에 있는 필드에 대한 모든 정보 관리
	iniField_t **fieldList;
};

//////////////////////////////////////////////////////////////////////////
/// Public Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

iniManager_t *iniManagerNew(const char *fileName);
void iniManagerDelete(iniManager_t **iniManager);
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *fieldName, const char *keyName, int defaultValue, const char *fileName, int *result);

#endif // #ifndef __INI_PARSER_H__
