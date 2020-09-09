#ifndef __GRADE_LIMIT_H__
#define __GRADE_LIMIT_H__

#include "iniManager.h"

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
	gradeInfo_t gradeA;
	gradeInfo_t gradeB;
	gradeInfo_t gradeC;
	gradeInfo_t gradeD;
	int totalMin;
	int totalMax;
	iniManager_t *iniManager;
};

//////////////////////////////////////////////////////////////////////////
/// Local Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

gradeManager_t* gradeManagerNew();
void gradeManagerDelete(gradeManager_t **manager);
void gradeManagerEvaluateGrade(gradeManager_t *manager, const int *scores, int size);

#endif // #ifndef __GRADE_LIMIT_H__
