#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////

int main()
{
	int inputNumbers[14] = { 100, 99, 50, 80, 22, 33, 79, 56, 44, 69, 0, -1, 101, 999 };

	gradeManager_t *gradeManager = gradeManagerNew("./grade.ini");
	if(gradeManager == NULL)
	{
		return FAIL;
	}

	printf("[등급 검사 시작]\n");
	gradeManagerEvaluateGrade(gradeManager, inputNumbers, 14);
	printf("[등급 검사 종료]\n\n");

	gradeManagerDelete(&gradeManager);

	return SUCCESS;
}

