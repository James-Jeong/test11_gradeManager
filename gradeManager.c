#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int gradeManagerCheckGrade(gradeManager_t *limit, char grade, int score);
static char gradeManagerGetGradeFromNumber(gradeManager_t *limit, int score);
static gradeInfo_t* gradeManagerGetLimitFromGrade(gradeManager_t *limit, char grade);
static int gradeManagerLoadINI(gradeManager_t *limit, const char *fileName);
static void gradeManagerSetGradeLimit(gradeManager_t *limit, char grade, int min, int max);

//////////////////////////////////////////////////////////////////////////
/// Public Function
//////////////////////////////////////////////////////////////////////////

void evaluateGrade(gradeManager_t *limit, const int *scores, int size)
{
	if(scores == NULL)
	{
		printf("[DEBUG] 입력받은 점수 목록이 NULL.\n");
		return;
	}

	int numPos = 0;

	for( ; numPos < size; numPos++)	
	{
		printf("%d -> %c\n", scores[numPos], gradeManagerGetGradeFromNumber(limit, scores[numPos]));
	}
}

//////////////////////////////////////////////////////////////////////////
/// Static Function for gradeInfo_t
//////////////////////////////////////////////////////////////////////////

static void gradeInfoSetData(gradeInfo_t *info, char grade, int min, int max)
{
	info->grade = grade;
	info->min = min;
	info->max = max;
}

//////////////////////////////////////////////////////////////////////////
/// Local Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

gradeManager_t* gradeManagerNew()
{
	gradeManager_t *limit = (gradeManager_t*)malloc(sizeof(gradeManager_t));
	if(limit == NULL)
	{
		printf("[DEBUG] 등급 임계치 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	limit->iniManager = iniManagerNew("./grade.ini");
	if(limit->iniManager == NULL)
	{
		return NULL;
	}

	if(gradeManagerLoadINI(limit, "./grade.ini") == FAIL)
	{
		gradeManagerDelete(&limit);
		return NULL;
	}

	return limit;
}

void gradeManagerDelete(gradeManager_t **limit)
{
	iniManagerDelete(&((*limit)->iniManager));
	free(*limit);
	*limit = NULL;
}


//////////////////////////////////////////////////////////////////////////
/// Static Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

static int gradeManagerLoadINI(gradeManager_t *limit, const char *fileName)
{
	printf("\n[Load values from the field list to variables]\n");
	int totalMin = iniManagerGetValueFromINI(limit->iniManager, "[Total]", "min", 0, fileName);
	if(totalMin == FAIL) return FAIL;
	printf("Total min value : %d\n", totalMin);
	int totalMax = iniManagerGetValueFromINI(limit->iniManager, "[Total]", "max", 0, fileName);
	if(totalMax == FAIL) return FAIL;
	printf("Total max value : %d\n", totalMax);

	limit->totalMin = totalMin;
	limit->totalMax = totalMax;

	int minA = iniManagerGetValueFromINI(limit->iniManager, "[A]", "min", 0, fileName);
	if(minA == FAIL) return FAIL;
	printf("Grade A min value : %d\n", minA);
	int maxA = iniManagerGetValueFromINI(limit->iniManager, "[A]", "max", 0, fileName);
	if(maxA == FAIL) return FAIL;
	printf("Grade A max value : %d\n", maxA);

	int minB = iniManagerGetValueFromINI(limit->iniManager, "[B]", "min", 0, fileName);
	if(minB == FAIL) return FAIL;
	printf("Grade B min value : %d\n", minB);
	int maxB = iniManagerGetValueFromINI(limit->iniManager, "[B]", "max", 0, fileName);
	if(maxB == FAIL) return FAIL;
	printf("Grade B max value : %d\n", maxB);

	int minC = iniManagerGetValueFromINI(limit->iniManager, "[C]", "min", 0, fileName);
	if(minC == FAIL) return FAIL;
	printf("Grade C min value : %d\n", minC);
	int maxC = iniManagerGetValueFromINI(limit->iniManager, "[C]", "max", 0, fileName);
	if(maxC == FAIL) return FAIL;
	printf("Grade C max value : %d\n", maxC);

	int minD = iniManagerGetValueFromINI(limit->iniManager, "[D]", "min", 0, fileName);
	if(minD == FAIL) return FAIL;
	printf("Grade D min value : %d\n", minD);
	int maxD = iniManagerGetValueFromINI(limit->iniManager, "[D]", "max", 0, fileName);
	if(maxD == FAIL) return FAIL;
	printf("Grade D max value : %d\n", maxD);

	gradeManagerSetGradeLimit(limit, 'A', minA, maxA);
	gradeManagerSetGradeLimit(limit, 'B', minB, maxB);
	gradeManagerSetGradeLimit(limit, 'C', minC, maxC);
	gradeManagerSetGradeLimit(limit, 'D', minD, maxD);

	return SUCCESS;
}

static void gradeManagerSetGradeLimit(gradeManager_t *limit, char grade, int min, int max)
{
	switch(grade)
	{
		case 'A':
			gradeInfoSetData(&limit->limitOfA, grade, min, max);
			break;
		case 'B':
			gradeInfoSetData(&limit->limitOfB, grade, min, max);
			break;
		case 'C':
			gradeInfoSetData(&limit->limitOfC, grade, min, max);
			break;
		case 'D':
			gradeInfoSetData(&limit->limitOfD, grade, min, max);
			break;
		default:
			break;
	}
}

static int gradeManagerCheckGrade(gradeManager_t *limit, char grade, int score)
{
	gradeInfo_t *gradeInfo = gradeManagerGetLimitFromGrade(limit, grade);
	if(gradeInfo == NULL)
	{
		printf("[DEBUG] 입력받은 등급에 대한 임계치 정보가 없음.\n");
		return FALSE;
	}
	return ((score >= gradeInfo->min) && (score <= gradeInfo->max)) ? TRUE : FALSE;
}

static char gradeManagerGetGradeFromNumber(gradeManager_t *limit, int score)
{
	if(limit == NULL)
	{
		printf("[DEBUG] 등급 임계치 목록 객체가 NULL.\n");
		return '?';
	}

	if(score < 0 || score > 100)
	{
		printf("\n[DEBUG] 입력받은 점수에 대한 등급을 판단할 수 없음 .\n");
		return '?';
	}

	if(gradeManagerCheckGrade(limit, 'A', score) == TRUE) return 'A';
	else if(gradeManagerCheckGrade(limit, 'B', score) == TRUE) return 'B';
	else if(gradeManagerCheckGrade(limit, 'C', score) == TRUE) return 'C';
	else if(gradeManagerCheckGrade(limit, 'D', score) == TRUE) return 'D';

	return 'F';
}

static gradeInfo_t* gradeManagerGetLimitFromGrade(gradeManager_t *limit, char grade)
{
	if(limit->limitOfA.grade == grade)
	{
		return &(limit->limitOfA);
	}
	else if(limit->limitOfB.grade == grade)
	{
		return &(limit->limitOfB);
	}
	else if(limit->limitOfC.grade == grade)
	{
		return &(limit->limitOfC);
	}
	else if(limit->limitOfD.grade == grade)
	{
		return &(limit->limitOfD);
	}

	return NULL;
}

