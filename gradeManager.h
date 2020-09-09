#ifndef __GRADE_LIMIT_H__
#define __GRADE_LIMIT_H__

#include "iniParser.h"

//////////////////////////////////////////////////////////////////////////
/// Definitions & Macros
//////////////////////////////////////////////////////////////////////////

#define TRUE	1
#define FALSE	0

typedef struct gradeInfo_s gradeInfo_t;
struct gradeInfo_s
{
	char grade;
	int min;
	int max;
};

typedef struct gradeManager_s gradeManager_t;
struct gradeManager_s
{
	gradeInfo_t limitOfA;
	gradeInfo_t limitOfB;
	gradeInfo_t limitOfC;
	gradeInfo_t limitOfD;
	int totalMin;
	int totalMax;
	iniManager_t *iniManager;
};

//////////////////////////////////////////////////////////////////////////
/// Local Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

gradeManager_t* gradeManagerNew();
void gradeManagerDelete(gradeManager_t **limit);
void evaluateGrade(gradeManager_t *limit, const int *scores, int size);

#endif // #ifndef __GRADE_LIMIT_H__
