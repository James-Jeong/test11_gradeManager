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

// ini 파일에서의 필드 이름 최대 길이
#define MAX_FIELD_LEN	64

/* 점수에 대한 등급 정보를 가지는 ini 파일에 담긴 모든 필드 이름을 저장하는 구조체 */ 
// <ini 파일 구성>
// [field] -> 등급 이름
// (key)=(value) -> key : 최대 또는 최소값을 식별하는 이름 / value : 해당 key 의 값
typedef struct iniManager_s iniManager_t;
struct iniManager_s
{
	// 필드 전체 개수
	int fieldMaxNum;
	// 필드 이름을 저장하는 배열
	char **fieldList;
};

//////////////////////////////////////////////////////////////////////////
/// Public Functions for iniManager_t
//////////////////////////////////////////////////////////////////////////

iniManager_t *iniManagerNew(const char *fileName);
void iniManagerDelete(iniManager_t **iniManager);
int iniManagerGetValueFromField(const iniManager_t *iniManager, const char *field, const char *key, int defaultValue, const char *fileName, int *result);

#endif // #ifndef __INI_PARSER_H__
