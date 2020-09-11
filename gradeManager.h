#ifndef __GRADE_LIMIT_H__
#define __GRADE_LIMIT_H__

#include "iniManager.h"

//////////////////////////////////////////////////////////////////////////
/// Definitions & Macros
//////////////////////////////////////////////////////////////////////////

// 조건 참
#define TRUE	1
// 조건 거짓
#define FALSE	0

// 숫자 비교 유형 열거형
enum COMPARE_TYPE
{
	LT = 1, // 작다. (Less Than)
	GT		// 크다. (Greater Than)
};

/* 점수에 따른 등급을 판단하기 위한 등급 관련 데이터를 저장하는 구조체 */
typedef struct gradeInfo_s gradeInfo_t;
struct gradeInfo_s
{
	// 등급 이름
	char grade;
	// 지정한 등급으로 판단되기 위한 최소 범위값
	int min;
	// 지정한 등급으로 판단되기 위한 최대 범위값
	int max;
};

/* 지정한 점수에 대한 등급 A ~ D 까지의 정보를 관리하고, 등급 전체 범위와 ini 파일에 대한 정보를 관리하는 구조체 */
typedef struct gradeManager_s gradeManager_t;
struct gradeManager_s
{
	// 등급 A 정보
	gradeInfo_t gradeA;
	// 등급 B 정보
	gradeInfo_t gradeB;
	// 등급 C 정보
	gradeInfo_t gradeC;
	// 등급 D 정보
	gradeInfo_t gradeD;
	// 전체 범위의 최소값
	int totalMin;
	// 전체 범위의 최대값
	int totalMax;
	// ini 파일에 대한 정보를 관리하는 구조체
	iniManager_t *iniManager;
};

//////////////////////////////////////////////////////////////////////////
/// Local Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

gradeManager_t* gradeManagerNew(const char *fileName);
void gradeManagerDelete(gradeManager_t **manager);
void gradeManagerEvaluateGrade(gradeManager_t *manager, const int *scores, int size);

#endif // #ifndef __GRADE_LIMIT_H__
