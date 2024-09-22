#ifndef SHOCK_H
#define SHOCK_H

#include "sqlite3.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <windows.h> 
#include <sql.h> 
#include <sqltypes.h> 
#include <sqlext.h>
#include <odbcinst.h>

using namespace std;

/************************ funkcie na vytvorenie testovacieho suboru Shock ***********************/


//premenovanie na *.shk subor
string RenameToShk(string aMDBFile);


/************************ DB test_profiles ***********************/

	// tabulka profiles_shocks

class CShockProfiles
	{
	public:
	int ProfileID, CurveType, Amplitude;
	double Width;

	CShockProfiles();
	~CShockProfiles();
	void PrintVectorShockProfiles();
	void InsertToShockProfiles(sqlite3* aDB);
	CShockProfiles& FetchFromShockProfiles(sqlite3* aDB, int aProfileID);
	friend int callbackShockProfiles(void* aVector, int argc, char** argv, char** azColName);
	/*vychadza z kniznice p. Novaka*/ void InsertFrom_ShockProfiles_ToTestFileShock(string aFile);
	};


/************************ DB tests_parameters ***********************/


//tabulka test_parameters_shocks
class CShocks
	{
	public:
	int TestFileYear, TestFileNumber, PreShockLevel, ShockNumberPerDirection;
	double ShockDistance;
	int NumberOfWarnLimitViolations, NumberOfAbortLimitViolations;
	double MassTestItems, MassTestFixture;
	int ConnectionWithSlipTable, PreTestLevel, CurveSave, CurveSaveShocks, ShockInverted, PosPreShock, NegPreShock, PosPostShock, NegPostShock, Standard, WarnLimit;

	CShocks();
	~CShocks();
	void PrintVectorShocks();
	void InsertToShocks(sqlite3* aDB);
	CShocks& FetchFromShocks(sqlite3* aDB, int aTestFileYear, int aTestFileNumber);
	friend int callbackShocks(void* aVector, int argc, char** argv, char** azColName);
	/*vychadza z kniznice p. Novaka*/ void InsertFrom_Shocks_ToTestFileShock(string aFile);
	/*vychadza z kniznice p. Novaka*/ CShocks& FetchFrom_TestFileShock_ToShocks(string aFile);
	string UpdateShocks(sqlite3* aDB, CShocks old);
	};


//tabulka shocks_tolerance_band
class CToleranceBand
	{
	public:
	int adTestFileYear, adTestFileNumber;
	double Time, AbortLimitPos, AbortLimitNeg;

	CToleranceBand();
	~CToleranceBand();
	void PrintVectorToleranceBand(vector <CToleranceBand>& aVec);
	void InsertToToleranceBand(sqlite3* aDB, vector <CToleranceBand>& aVec);
	vector <CToleranceBand> FetchFromToleranceBand(sqlite3* aDB, int aTestFileNumber, int aTestFileYear);
	friend int callbackToleranceBand(void* aVector, int argc, char** argv, char** azColName);
	void InsertFrom_ToleranceBand_ToTestFileShock(string aFile, vector <CToleranceBand>& aVec);
	};

#endif