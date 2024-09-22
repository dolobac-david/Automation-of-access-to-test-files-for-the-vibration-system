#ifndef SNIMACE_H
#define SNIMACE_H

#include "sqlite3.h"
#include <string>
#include <vector>
#include <windows.h> 
#include <sql.h> 
#include <sqltypes.h> 
#include <sqlext.h>
#include <odbcinst.h>

using namespace std;

/************************ DB tests_parameters - tabulka vibrations_common ***********************/


//tabulka vibrations_common
class CVibrationsCommon
	{
	public:
	int TestFileYear, TestFileNumber;
	int TypeOfInput[8];
	int RelSensorID[8];
	double Sensitivity[8];
	int Unit[8];
	double LowerLimit[8];
	double UpperLimit[8];
	vector <string> InfoText;
	int Notching[8];

		CVibrationsCommon();
		~CVibrationsCommon();
		void PrintVibrationsCommon();
		void InsertToVibrationsCommon(sqlite3* aDB);
		CVibrationsCommon& FetchFromVibrationsCommon(sqlite3* aDB, int aTestNumber, int aTestYear);
		friend int callbackVibrationsCommon(void* aClass, int argc, char** argv, char** azColName);
		void InsertFrom_VibrationsCommon_ToTestFileRandom(string aFile);
		CVibrationsCommon& FetchFrom_TestFileRandom_ToVibrationsCommon(string aFile);
		void InsertFrom_VibrationsCommon_ToTestFileSine(string aFile);
		CVibrationsCommon& FetchFrom_TestFileSine_ToVibrationsCommon(string aFile);
		void InsertFrom_VibrationsCommon_ToTestFileShock(string aFile);
		CVibrationsCommon& FetchFrom_TestFileShock_ToVibrationsCommon(string aFile);
		string UpdateVibrationsCommon(sqlite3* aDB, CVibrationsCommon oldRecord, bool sinus);
		int* GetNotching()
			{
			return Notching;
			}
		int* GetTypeOfInput()
			{
			return TypeOfInput;
			}
		int GetTestFileYear()
			{
			return TestFileYear;
			}
		int GetTestFileNumber()
			{
			return TestFileNumber;
			}
	};
#endif