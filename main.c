#include "gradeManager.h"

//////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////

int main()
{
	gradeManager_t *limit = gradeManagerNew();
	if(limit == NULL)
	{
		return FAIL;
	}

//	int inputNumbers[10] = { 100, 99, 50, 80, 20, 33, 79, 56, 44, 69 };
//	evaluateGrade(limit, inputNumbers, 10);

	gradeManagerDelete(&limit);
	return SUCCESS;
}

