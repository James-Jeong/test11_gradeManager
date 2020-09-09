#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int gradeManagerCheckGrade(gradeManager_t *gradeManager, char grade, int score);
static char gradeManagerGetGradeFromNumber(gradeManager_t *gradeManager, int score);
static gradeInfo_t* gradeManagerGetInfoFromGrade(gradeManager_t *gradeManager, char grade);
static int gradeManagerLoadINI(gradeManager_t *gradeManager, const char *fileName);
static int gradeManagerSetGradeInfo(gradeManager_t *gradeManager, char grade, int min, int max);

//////////////////////////////////////////////////////////////////////////
/// Static Function for gradeInfo_t
//////////////////////////////////////////////////////////////////////////

static void gradeInfoSetData(gradeInfo_t *info, char grade, int min, int max)
{
	if(info == NULL)
	{
		printf("[DEBUG] gradeInfo 가 NULL. (info:%p)\n", info);
		return;
	}

	info->grade = grade;
	info->min = min;
	info->max = max;
}

//////////////////////////////////////////////////////////////////////////
/// Local Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

gradeManager_t* gradeManagerNew()
{
	gradeManager_t *gradeManager = (gradeManager_t*)malloc(sizeof(gradeManager_t));
	if(gradeManager == NULL)
	{
		printf("[DEBUG] 등급 임계치 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	gradeManager->iniManager = iniManagerNew("./grade.ini");
	if(gradeManager->iniManager == NULL)
	{
		free(gradeManager);
		return NULL;
	}

	if(gradeManagerLoadINI(gradeManager, "./grade.ini") == FAIL)
	{
		gradeManagerDelete(&gradeManager);
		return NULL;
	}

	return gradeManager;
}

void gradeManagerDelete(gradeManager_t **gradeManager)
{
	if(*gradeManager == NULL)
	{
		printf("[DEBUG] gradeManager 해제 실패. 객체가 NULL. (gradeManager:%p)\n", *gradeManager);
		return;
	}

	iniManagerDelete(&((*gradeManager)->iniManager));
	free(*gradeManager);
	*gradeManager = NULL;
}

void gradeManagerEvaluateGrade(gradeManager_t *gradeManager, const int *scores, int size)
{
	if(gradeManager == NULL)
	{
		printf("[DEBUG] gradeManager 가 NULL. (gradeManager:%p)\n", gradeManager);
		return;
	}

	if(scores == NULL)
	{
		printf("[DEBUG] 입력받은 점수 목록이 NULL. (scores:%p)\n", scores);
		return;
	}

	if(size <= 0)
	{
		printf("[ERROR] 입력받은 점수 목록의 크기가 0 보다 작거나 같음. (size:%d)\n", size);
		return;
	}

	int numPos = 0;
	for( ; numPos < size; numPos++)	
	{
		printf("%d -> %c\n", scores[numPos], gradeManagerGetGradeFromNumber(gradeManager, scores[numPos]));
	}
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

static int gradeManagerLoadINI(gradeManager_t *gradeManager, const char *fileName)
{
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL. (fileName:%p)\n", fileName);
		return FAIL;
	}

	printf("\n[Load values from the field list to variables]\n");
	int totalMin = iniManagerGetValueFromINI(gradeManager->iniManager, "[Total]", "min", 0, fileName);
	if(totalMin == FAIL) return FAIL;
	printf("Total min value : %d\n", totalMin);
	int totalMax = iniManagerGetValueFromINI(gradeManager->iniManager, "[Total]", "max", 0, fileName);
	if(totalMax == FAIL) return FAIL;
	printf("Total max value : %d\n", totalMax);

	gradeManager->totalMin = totalMin;
	gradeManager->totalMax = totalMax;

	int minA = iniManagerGetValueFromINI(gradeManager->iniManager, "[A]", "min", 0, fileName);
	if(minA == FAIL) return FAIL;
	printf("Grade A min value : %d\n", minA);
	int maxA = iniManagerGetValueFromINI(gradeManager->iniManager, "[A]", "max", 0, fileName);
	if(maxA == FAIL) return FAIL;
	printf("Grade A max value : %d\n", maxA);

	int minB = iniManagerGetValueFromINI(gradeManager->iniManager, "[B]", "min", 0, fileName);
	if(minB == FAIL) return FAIL;
	printf("Grade B min value : %d\n", minB);
	int maxB = iniManagerGetValueFromINI(gradeManager->iniManager, "[B]", "max", 0, fileName);
	if(maxB == FAIL) return FAIL;
	printf("Grade B max value : %d\n", maxB);

	int minC = iniManagerGetValueFromINI(gradeManager->iniManager, "[C]", "min", 0, fileName);
	if(minC == FAIL) return FAIL;
	printf("Grade C min value : %d\n", minC);
	int maxC = iniManagerGetValueFromINI(gradeManager->iniManager, "[C]", "max", 0, fileName);
	if(maxC == FAIL) return FAIL;
	printf("Grade C max value : %d\n", maxC);

	int minD = iniManagerGetValueFromINI(gradeManager->iniManager, "[D]", "min", 0, fileName);
	if(minD == FAIL) return FAIL;
	printf("Grade D min value : %d\n", minD);
	int maxD = iniManagerGetValueFromINI(gradeManager->iniManager, "[D]", "max", 0, fileName);
	if(maxD == FAIL) return FAIL;
	printf("Grade D max value : %d\n", maxD);

	if(gradeManagerSetGradeInfo(gradeManager, 'A', minA, maxA) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'B', minB, maxB) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'C', minC, maxC) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'D', minD, maxD) == FAIL) return FAIL;

	printf("\n");
	return SUCCESS;
}

static int gradeManagerSetGradeInfo(gradeManager_t *gradeManager, char grade, int min, int max)
{
	switch(grade)
	{
		case 'A':
			gradeInfoSetData(&gradeManager->gradeA, grade, min, max);
			break;
		case 'B':
			gradeInfoSetData(&gradeManager->gradeB, grade, min, max);
			break;
		case 'C':
			gradeInfoSetData(&gradeManager->gradeC, grade, min, max);
			break;
		case 'D':
			gradeInfoSetData(&gradeManager->gradeD, grade, min, max);
			break;
		default:
			return FAIL;
	}

	return SUCCESS;
}

static int gradeManagerCheckGrade(gradeManager_t *gradeManager, char grade, int score)
{
	gradeInfo_t *gradeInfo = gradeManagerGetInfoFromGrade(gradeManager, grade);
	if(gradeInfo == NULL)
	{
		printf("[DEBUG] 입력받은 등급에 대한 임계치 정보가 없음.\n");
		return FALSE;
	}
	return ((score >= gradeInfo->min) && (score <= gradeInfo->max)) ? TRUE : FALSE;
}

static char gradeManagerGetGradeFromNumber(gradeManager_t *gradeManager, int score)
{
	if(score < gradeManager->totalMin || score > gradeManager->totalMax)
	{
		printf("\n[ERROR] 입력받은 점수에 대한 등급을 판단할 수 없음 .\n");
		return '?';
	}

	if(gradeManagerCheckGrade(gradeManager, 'A', score) == TRUE) return 'A';
	else if(gradeManagerCheckGrade(gradeManager, 'B', score) == TRUE) return 'B';
	else if(gradeManagerCheckGrade(gradeManager, 'C', score) == TRUE) return 'C';
	else if(gradeManagerCheckGrade(gradeManager, 'D', score) == TRUE) return 'D';

	return 'F';
}

static gradeInfo_t* gradeManagerGetInfoFromGrade(gradeManager_t *gradeManager, char grade)
{
	if(gradeManager->gradeA.grade == grade)
	{
		return &(gradeManager->gradeA);
	}
	else if(gradeManager->gradeB.grade == grade)
	{
		return &(gradeManager->gradeB);
	}
	else if(gradeManager->gradeC.grade == grade)
	{
		return &(gradeManager->gradeC);
	}
	else if(gradeManager->gradeD.grade == grade)
	{
		return &(gradeManager->gradeD);
	}

	return NULL;
}

