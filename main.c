#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////

int main()
{
	int inputNumbers[10] = { 100, 99, 50, 80, 20, 33, 79, 56, 44, 69 };

	gradeManager_t *limit = gradeManagerNew("./grade.ini");
	gradeManagerEvaluateGrade(limit, inputNumbers, 10);
	gradeManagerDelete(&limit);

	return SUCCESS;
}

