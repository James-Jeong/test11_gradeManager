#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Predefinition of Static Util Function
//////////////////////////////////////////////////////////////////////////

static int compareNumbers(const char *valueName1, int value1, const char *valueName2, int value2, int type);

//////////////////////////////////////////////////////////////////////////
/// Predefinitions of Static Functions
//////////////////////////////////////////////////////////////////////////

static int gradeManagerCheckGrade(const gradeManager_t *gradeManager, char grade, int score);
static char gradeManagerGetGradeFromNumber(const gradeManager_t *gradeManager, int score);
static const gradeInfo_t* gradeManagerGetGradeInfo(const gradeManager_t *gradeManager, char grade);
static int gradeManagerLoadINI(gradeManager_t *gradeManager, const char *fileName);
static int gradeManagerSetGradeInfo(gradeManager_t *gradeManager, char grade, int min, int max);
static int gradeManagerGetValueFromINI(const gradeManager_t *gradeManager, const char *field, const char *key, int defaultValue, const char *fileName);

//////////////////////////////////////////////////////////////////////////
/// Static Function for gradeInfo_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static void gradeInfoSetData(gradeInfo_t *info, char grade, int min, int max)
 * @brief 등급 정보를 저장하는 함수
 * @param info 등급 정보를 저장할 gradeInfo_t 객체(출력)
 * @param grade 정보를 저장할 등급 문자(입력)
 * @param min 지정한 점수에 대한 등급을 판단하기 위한 범위 최소값(입력)
 * @param max 지정한 점수에 대한 등급을 판단하기 위한 범위 최대값(입력)
 * @return 반환값 없음
 */
static void gradeInfoSetData(gradeInfo_t *info, char grade, int min, int max)
{
	if(info == NULL)
	{
		printf("[DEBUG] gradeInfo 가 NULL.\n");
		return;
	}

	info->grade = grade;
	info->min = min;
	info->max = max;
}

//////////////////////////////////////////////////////////////////////////
/// Public Functions for gradeManager_t
//////////////////////////////////////////////////////////////////////////

/**
 * @fn gradeManager_t* gradeManagerNew(const char *fileName)
 * @brief 지정한 점수에 대한 등급 정보를 관리하기 위한 구조체 객체를 새로 생성하는 함수
 * 외부에서 접근할 수 있는 함수이므로 생성된 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param fileName 점수에 대한 등급 정보를 가지고 있는 ini 파일의 이름(입력, 읽기 전용)
 * @return 성공 시 새로 생성된 gradeManager_t 구조체 객체, 실패 시 NULL 반환
 */
gradeManager_t* gradeManagerNew(const char *fileName)
{
	if(fileName == NULL)
	{
		printf("[DEBUG] 주어진 fileName 이 NULL.\n");
		return NULL;
	}

	gradeManager_t *gradeManager = (gradeManager_t*)malloc(sizeof(gradeManager_t));
	if(gradeManager == NULL)
	{
		printf("[DEBUG] 등급 임계치 객체 동적 생성 실패. NULL.\n");
		return NULL;
	}

	gradeManager->iniManager = NULL;
	if(gradeManagerLoadINI(gradeManager, fileName) == FAIL)
	{
		printf("[로딩 실패]\n\n");
		gradeManagerDelete(&gradeManager);
		return NULL;
	}

	return gradeManager;
}

/**
 * @fn void gradeManagerDelete(gradeManager_t **gradeManager)
 * @brief 생성된 gradeManager_t 구조체 객체의 메모리를 해제하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param gradeManager 삭제할 gradeManager_t 구조체 객체(입력, 이중 포인터)
 * @return 반환값 없음
 */
void gradeManagerDelete(gradeManager_t **gradeManager)
{
	if(*gradeManager == NULL)
	{
		printf("[DEBUG] gradeManager 해제 실패. 객체가 NULL.\n");
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
 * @fn void gradeManagerEvaluateGrade(const gradeManager_t *gradeManager, const int *scores, int size)
 * @brief 지정한 점수에 대한 등급을 판단하는 함수
 * 외부에서 접근할 수 있는 함수이므로 전달받은 구조체 포인터에 대한 NULL 체크를 수행한다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param scores 등급 판단을 위한 점수들을 담은 정수 배열(입력, 읽기 전용)
 * @param size 정수 배열의 전체 크기(입력)
 * @return 반환값 없음
 */
void gradeManagerEvaluateGrade(const gradeManager_t *gradeManager, const int *scores, int size)
{
	if(gradeManager == NULL)
	{
		printf("[DEBUG] gradeManager 가 NULL.\n");
		return;
	}

	if(scores == NULL)
	{
		printf("[DEBUG] 입력받은 점수 목록이 NULL.\n");
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
 * @fn static char gradeManagerGetGradeFromNumber(const gradeManager_t *gradeManager, int score)
 * @brief 지정한 점수로 등급을 결정하는 함수
 * gradeManagerEvaluateGrade 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param score 등급 판단에 사용될 점수(입력)
 * @return 성공 시 결정된 등급 문자, 실패 시 'F' 문자, 지정한 점수가 등급 전체 범위에 포함되지 않을 때 '?' 문자 반환
 */
static char gradeManagerGetGradeFromNumber(const gradeManager_t *gradeManager, int score)
{
	if(score < gradeManager->totalMin || score > gradeManager->totalMax)
	{
		printf("\n[ERROR] 입력받은 점수에 대한 등급을 판단할 수 없음.\n");
		return '?';
	}

	if(gradeManagerCheckGrade(gradeManager, 'A', score) == TRUE) return 'A';
	else if(gradeManagerCheckGrade(gradeManager, 'B', score) == TRUE) return 'B';
	else if(gradeManagerCheckGrade(gradeManager, 'C', score) == TRUE) return 'C';
	else if(gradeManagerCheckGrade(gradeManager, 'D', score) == TRUE) return 'D';

	return 'F';
}

/**
 * @fn static int gradeManagerCheckGrade(const gradeManager_t *gradeManager, char grade, int score)
 * @brief 지정한 점수에 대해 등급을 판단하는 함수
 * gradeManagerGetGradeFromNumber 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param grade 판단할 등급 문자(입력)
 * @param score 등급 판단에 사용될 점수(입력)
 * @return 성공 시 TRUE, 실패 시 FALSE 반환
 */
static int gradeManagerCheckGrade(const gradeManager_t *gradeManager, char grade, int score)
{
	const gradeInfo_t *gradeInfo = gradeManagerGetGradeInfo(gradeManager, grade);
	if(gradeInfo == NULL)
	{
		printf("[DEBUG] 입력받은 등급에 대한 정보(min, max)가 없음.\n");
		return FALSE;
	}
	return ((score >= gradeInfo->min) && (score <= gradeInfo->max)) ? TRUE : FALSE;
}

/**
 * @fn static const gradeInfo_t* gradeManagerGetGradeInfo(const gradeManager_t *gradeManager, char grade)
 * @brief 지정한 등급에 대해, 등급 정보를 관리하는 구조체(gradeManager_t)에 속한 gradeInfo_t 구조체를 반환하기 위한 함수
 * gradeManagerCheckGrade 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력)
 * @param grade 등급 정보 식별에 필요한 등급 문자(입력)
 * @return 성공 시 지정한 등급 정보를 가지는 gradeInfo_t 구조체의 주소, 실패 시 NULL 반환
 */
static const gradeInfo_t* gradeManagerGetGradeInfo(const gradeManager_t *gradeManager, char grade)
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
 * @brief 지정한 ini 파일에 대한 정보를 gradeManager_t 구조체에 저장하는 함수
 * gradeManagerNew 함수에서 호출되기 때문에 전달받은 구조체 포인터와 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력 및 출력)
 * @param fileName 등급 정보를 관리하는 ini 파일 이름(입력, 읽기 전용)
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

	int totalMin = gradeManagerGetValueFromINI(gradeManager, "[Total]", "min", 0, fileName);
	if(totalMin == FAIL) return FAIL;
	int totalMax = gradeManagerGetValueFromINI(gradeManager, "[Total]", "max", 100, fileName);
	if(totalMax == FAIL) return FAIL;
	
	gradeManager->totalMin = totalMin;
	gradeManager->totalMax = totalMax;

	int minA = gradeManagerGetValueFromINI(gradeManager, "[A]", "min", 90, fileName);
	if(minA == FAIL) return FAIL;

	int maxA = gradeManagerGetValueFromINI(gradeManager, "[A]", "max", 100, fileName);
	if(maxA == FAIL) return FAIL;
	if(compareNumbers("A max", maxA, "Total max", totalMax, LT) == FAIL) return FAIL;
	if(compareNumbers("A min", minA, "A max", maxA, LT) == FAIL) return FAIL;

	int minB = gradeManagerGetValueFromINI(gradeManager, "[B]", "min", 80, fileName);
	if(minB == FAIL) return FAIL;
	if(compareNumbers("B min", minB, "A min", minA, LT) == FAIL) return FAIL;

	int maxB = gradeManagerGetValueFromINI(gradeManager, "[B]", "max", 89, fileName);
	if(maxB == FAIL) return FAIL;
	if(compareNumbers("B max", maxB, "A max", maxA, LT) == FAIL) return FAIL;
	if(compareNumbers("B min", minB, "B max", maxB, LT) == FAIL) return FAIL;

	int minC = gradeManagerGetValueFromINI(gradeManager, "[C]", "min", 70, fileName);
	if(minC == FAIL) return FAIL;
	if(compareNumbers("C min", minC, "B min", minB, LT) == FAIL) return FAIL;

	int maxC = gradeManagerGetValueFromINI(gradeManager, "[C]", "max", 79, fileName);
	if(maxC == FAIL) return FAIL;
	if(compareNumbers("C max", maxC, "B max", maxB, LT) == FAIL) return FAIL;
	if(compareNumbers("C min", minC, "C max", maxC, LT) == FAIL) return FAIL;

	int minD = gradeManagerGetValueFromINI(gradeManager, "[D]", "min", 60, fileName);
	if(minD == FAIL) return FAIL;
	if(compareNumbers("D min", minD, "C min", minC, LT) == FAIL) return FAIL;
	if(compareNumbers("D min", minD, "Total min", totalMin, GT) == FAIL) return FAIL;

	int maxD = gradeManagerGetValueFromINI(gradeManager, "[D]", "max", 69, fileName);
	if(maxD == FAIL) return FAIL;
	if(compareNumbers("D max", maxD, "C max", maxC, LT) == FAIL) return FAIL;
	if(compareNumbers("D min", minD, "D max", maxD, LT) == FAIL) return FAIL;

	if(gradeManagerSetGradeInfo(gradeManager, 'A', minA, maxA) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'B', minB, maxB) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'C', minC, maxC) == FAIL) return FAIL;
	if(gradeManagerSetGradeInfo(gradeManager, 'D', minD, maxD) == FAIL) return FAIL;

	printf("[로딩 완료]\n\n");
	return SUCCESS;
}

/**
 * @fn static int gradeManagerSetGradeInfo(gradeManager_t *gradeManager, char grade, int min, int max)
 * @brief 등급 정보를 gradeManager 에 저장하기 위한 함수
 * gradeManagerLoadINI 함수에서 호출되기 때문에 전달받은 구조체 포인터에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 저장할 구조체(출력)
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
			gradeInfoSetData(&(gradeManager->gradeA), grade, min, max);
			break;
		case 'B':
			gradeInfoSetData(&(gradeManager->gradeB), grade, min, max);
			break;
		case 'C':
			gradeInfoSetData(&(gradeManager->gradeC), grade, min, max);
			break;
		case 'D':
			gradeInfoSetData(&(gradeManager->gradeD), grade, min, max);
			break;
		default:
			return FAIL;
	}

	return SUCCESS;
}

/**
 * @fn static int gradeManagerGetValueFromINI(const gradeManager_t *gradeManager, const char *field, const char *key, int defaultValue, const char *fileName)
 * @brief iniManager 로 부터 지정한 필드와 키에 해당하는 값을 반환하는 함수
 * gradeManagerLoadINI 함수에서 호출되기 때문에 전달받은 구조체 포인터와 필드, 키, 파일 이름에 대한 NULL 체크를 수행하지 않는다.
 * @param gradeManager 등급 정보를 관리하는 구조체(입력, 읽기 전용)
 * @param field 키를 찾기 위한 필드 이름(입력, 읽기 전용)
 * @param key 값을 찾기 위한 키 이름(입력, 읽기 전용)
 * @param defaultValue 찾고자 하는 키에 대한 값이 존재하지 않을 때 반환될 값(입력)
 * @param fileName 등급 정보를 가지는 ini 파일 이름(입력, 읽기 전용)
 * @return 성공 시 지정한 필드에 대한 키의 값, 실패 시 FAIL 반환
 */
static int gradeManagerGetValueFromINI(const gradeManager_t *gradeManager, const char *field, const char *key, int defaultValue, const char *fileName)
{
	int result = FAIL;
	int value = iniManagerGetValueFromField(gradeManager->iniManager, field, key, defaultValue, fileName, &result);
	if(result == FAIL) return FAIL;
	printf("%s %s value : %d\n", field, key, value);
	return value;
}

//////////////////////////////////////////////////////////////////////////
/// Static Util Function
//////////////////////////////////////////////////////////////////////////

/**
 * @fn static int compareNumbers(const char *valueName1, int value1, const char *valueName2, int value2, int type)
 * @brief 두 개의 숫자를 비교 유형에 따라 비교하는 함수
 * gradeManagerLoadINI 함수에서 호출되기 때문에 전달받은 문자열들에 대한 NULL 체크를 수행하지 않는다.
 * @param field1 비교 결과가 거짓일 때 출력할 왼쪽 피연산자의 필드와 키 이름(입력, 읽기 전용)
 * @param value1 왼쪽 피연산자 값(입력)
 * @param field2 비교 결과가 거짓일 때 출력할 오른쪽 피연산자의 필드와 키 이름(입력, 읽기 전용)
 * @param value2 오른쪽 피연산자 값(입력)
 * @param type 숫자 비교 유형(입력, gradeManager.h 의 숫자 비교 유형 열거형 참조)
 * @return 성공 시 TRUE, 실패 시 FALSE 반환
 */
static int compareNumbers(const char *valueName1, int value1, const char *valueName2, int value2, int type)
{
	int result = TRUE;

	if(type == GT)
	{
		if(value1 < value2) result = FALSE;
	}
	else if(type == LT)
	{
		if(value1 > value2) result = FALSE;
	}
	else
	{
		printf("[DEBUG] 알 수 없는 숫자 비교 유형.\n");
		return FALSE;
	}

	if(result == FALSE) printf("[ERROR] 잘못된 범위 (%s:%d, %s:%d)\n", valueName1, value1, valueName2, value2);
	return result;
}

