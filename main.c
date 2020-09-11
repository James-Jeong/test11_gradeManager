#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Macro
//////////////////////////////////////////////////////////////////////////

// 등급이 판단될 점수 배열의 전체 길이
#define MAX_INPUT_NUM 14

//////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////

int main()
{
	int inputNumbers[MAX_INPUT_NUM] = { 100, 99, 50, 80, 22, 33, 79, 56, 44, 69, 0, -1, 101, 999 };

	gradeManager_t *gradeManager = gradeManagerNew("./grade.ini");
	if(gradeManager == NULL)
	{
		return FAIL;
	}

	printf("[등급 검사 시작]\n");
	gradeManagerEvaluateGrade(gradeManager, inputNumbers, MAX_INPUT_NUM);
	printf("[등급 검사 종료]\n\n");

	gradeManagerDelete(&gradeManager);

	return SUCCESS;
}

