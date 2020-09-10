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

/**
 * @fn static void gradeInfoSetData(gradeInfo_t *info, char grade, int min, int max)
 * @brief 등급에 대한 정보를 저장하는 함수
 * @param info 등급에 대한 정보를 저장할 gradeInfo_t 객체(출력)
 * @param grade 정보를 저장할 등급 문자(입력)
 * @param min 지정한 점수에 대한 등급을 판단하기 위한 범위 최소값(입력)
 * @param max 지정한 점수에 대한 등급을 판단하기 위한 범위 최대값(입력)
 * @return 반환값 없음
 */
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

/**
 * @fn gradeManager_t* gradeManagerNew(const char *fileName)
 * @brief 지정한 점수에 대한 등급 정보를 관리하기 위한 구조체 객체를 새로 생성하는 함수
 * 외부에서 접근할 수 있는 함수이므로 생성된 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param fileName 등급에 대한 정보를 관리하는 ini 파일 이름(입력, 읽기 전용)
 * @return 새로 생성된 gradeManager_t 구조체 객체 반환
 */
gradeManager_t* gradeManagerNew(const char *fileName)
{
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL. (fileName:%p)\n", fileName);
		return NULL;
	}

	gradeManager_t *gradeManager = (gradeManager_t*)malloc(sizeof(gradeManager_t));
	if(gradeManager == NULL)
	{
		printf("[DEBUG] 등급 임계치 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	if(gradeManagerLoadINI(gradeManager, fileName) == FAIL)
	{
		gradeManagerDelete(&gradeManager);
		return NULL;
	}

	return gradeManager;
}

/**
 * @fn void gradeManagerDelete(gradeManager_t **gradeManager)
 * @brief 생성된 gradeManager_t 구조체 객체의 메모리를 해제하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param gradeManager 삭제할 gradeManager_t 구조체 객체(입력)
 * @return 반환값 없음
 */
void gradeManagerDelete(gradeManager_t **gradeManager)
{
	if(*gradeManager == NULL)
	{
		printf("[DEBUG] gradeManager 해제 실패. 객체가 NULL. (gradeManager:%p)\n", *gradeManager);
		return;
	}

	if((*gradeManager)->iniManager != NULL)
	{
		iniManagerDelete(&((*gradeManager)->iniManager));
	}

	free(*gradeManager);
	*gradeManager = NULL;
}

/**
 * @fn void gradeManagerEvaluateGrade(gradeManager_t *gradeManager, const int *scores, int size)
 * @brief 지정한 점수에 대한 등급을 판단하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param gradeManager 등급에 대한 정보를 관리하는 구조체(입력)
 * @param scores 등급 판단을 위한 점수들을 담은 정수 배열(입력, 읽기 전용)
 * @param size 정수 배열의 전체 크기(입력)
 * @return 반환값 없음
 */
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

	int scorePos = 0;
	for( ; scorePos < size; scorePos++)	
	{
		printf("[%d] [%d -> %c]\n", scorePos, scores[scorePos], gradeManagerGetGradeFromNumber(gradeManager, scores[scorePos]));
	}
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////
/// Static Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static char gradeManagerGetGradeFromNumber(gradeManager_t *gradeManager, int score)
 * @brief 지정한 점수로 등급을 결정하는 함수
 * gradeManagerEvaluateGrade 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급에 대한 정보를 관리하는 구조체(입력)
 * @param score 등급 판단에 사용될 점수(입력)
 * @return 성공 시 결정된 등급 문자, 실패 시 'F' 문자, 지정한 점수가 등급 전체 범위에 포함되지 않을 때 '?' 문자 반환
 */
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

/**
 * @fn static int gradeManagerCheckGrade(gradeManager_t *gradeManager, char grade, int score)
 * @brief 지정한 점수에 대해 등급을 판단하는 함수
 * gradeManagerGetGradeFromNumber 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급에 대한 정보를 관리하는 구조체(입력)
 * @param grade 판단할 등급 문자(입력)
 * @param score 등급 판단에 사용될 점수(입력)
 * @return 성공 시 TRUE, 실패 시 FALSE 반환
 */
static int gradeManagerCheckGrade(gradeManager_t *gradeManager, char grade, int score)
{
	gradeInfo_t *gradeInfo = gradeManagerGetInfoFromGrade(gradeManager, grade);
	if(gradeInfo == NULL)
	{
		printf("[DEBUG] 입력받은 등급에 대한 정보(min, max)가 없음.\n");
		return FALSE;
	}
	return ((score >= gradeInfo->min) && (score <= gradeInfo->max)) ? TRUE : FALSE;
}

/**
 * @fn static gradeInfo_t* gradeManagerGetInfoFromGrade(gradeManager_t *gradeManager, char grade)
 * @brief 등급에 대한 정보를 가지고 있는 구조체(gradeInfo_t)를 반환하기 위한 함수
 * gradeManagerCheckGrade 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급에 대한 정보를 관리하는 구조체(입력)
 * @param grade 반환 시 식별에 필요한 등급 문자(입력)
 * @return 성공 시 지정한 등급에 대한 정보를 가지는 gradeInfo_t 구조체의 주소, 실패 시 NULL 반환
 */
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

/**
 * @fn static int gradeManagerLoadINI(gradeManager_t *gradeManager, const char *fileName)
 * @brief ini 파일에 대한 정보를 gradeManager 구조체에 저장하는 함수
 * gradeManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터와 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급에 대한 정보를 관리하는 구조체(입력 및 출력)
 * @param fileName 등급에 대한 정보를 관리하는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
static int gradeManagerLoadINI(gradeManager_t *gradeManager, const char *fileName)
{
	gradeManager->iniManager = iniManagerNew(fileName);
	if(gradeManager->iniManager == NULL)
	{
		return FAIL;
	}

	printf("\n[등급 정보 로딩 중...]\n");
	iniManager_t *iniManager = gradeManager->iniManager;

	int totalMin = iniManagerGetValueFromField(iniManager, "[Total]", "min", 0, fileName);
	if(totalMin == FAIL) return FAIL;
	printf("Total min value : %d\n", totalMin);
	int totalMax = iniManagerGetValueFromField(iniManager, "[Total]", "max", 0, fileName);
	if(totalMax == FAIL) return FAIL;
	printf("Total max value : %d\n", totalMax);

	gradeManager->totalMin = totalMin;
	gradeManager->totalMax = totalMax;

	int minA = iniManagerGetValueFromField(iniManager, "[A]", "min", 0, fileName);
	if(minA == FAIL) return FAIL;
	printf("Grade A min value : %d\n", minA);
	int maxA = iniManagerGetValueFromField(iniManager, "[A]", "max", 0, fileName);
	if(maxA == FAIL) return FAIL;
	printf("Grade A max value : %d\n", maxA);

	int minB = iniManagerGetValueFromField(iniManager, "[B]", "min", 0, fileName);
	if(minB == FAIL) return FAIL;
	printf("Grade B min value : %d\n", minB);
	int maxB = iniManagerGetValueFromField(iniManager, "[B]", "max", 0, fileName);
	if(maxB == FAIL) return FAIL;
	printf("Grade B max value : %d\n", maxB);

	int minC = iniManagerGetValueFromField(iniManager, "[C]", "min", 0, fileName);
	if(minC == FAIL) return FAIL;
	printf("Grade C min value : %d\n", minC);
	int maxC = iniManagerGetValueFromField(iniManager, "[C]", "max", 0, fileName);
	if(maxC == FAIL) return FAIL;
	printf("Grade C max value : %d\n", maxC);

	int minD = iniManagerGetValueFromField(iniManager, "[D]", "min", 0, fileName);
	if(minD == FAIL) return FAIL;
	printf("Grade D min value : %d\n", minD);
	int maxD = iniManagerGetValueFromField(iniManager, "[D]", "max", 0, fileName);
	if(maxD == FAIL) return FAIL;
	printf("Grade D max value : %d\n", maxD);
	
	if(gradeManagerSetGradeInfo(gradeManager, 'A', minA, maxA) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'B', minB, maxB) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'C', minC, maxC) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'D', minD, maxD) == FAIL) return FAIL;

	printf("[로딩 완료]\n\n");
	return SUCCESS;
}

/**
 * @fn static int gradeManagerSetGradeInfo(gradeManager_t *gradeManager, char grade, int min, int max)
 * @brief 등급에 대한 정보를 gradeManager 에 저장하기 위한 함수
 * gradeManagerLoadINI 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급에 대한 정보를 저장할 구조체(출력)
 * @param grade 정보를 저장할 등급 문자(입력)
 * @param min 지정한 점수에 대한 등급을 판단하기 위한 범위 최소값(입력)
 * @param max 지정한 점수에 대한 등급을 판단하기 위한 범위 최대값(입력)
 * @return 성공 시 SUCCESS, 실패 시 FAIL 반환
 */
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

