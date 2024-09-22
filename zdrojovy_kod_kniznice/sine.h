#ifndef SINUS_H
#define SINUS_H

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

#include "snimace.h"	//kvoli parametru int Notching[8], ktory urcuje ci sa notching udaje zapisuju alebo nie

using namespace std;


/************************ funkcie na vytvorenie testovacieho suboru Sine ***********************/


//premenovanie na *.sin subor
string RenameToSin(string aMDBFile);


/************************ DB test_profiles ***********************/

	// tabulka sinus_profiles

class CSineProfiles
	{
	public:
	int ProfileID;
	double Frequency, Acceleration, WarnLimitPositive, WarnLimitNegative, AbortLimitPositive, AbortLimitNegative;

	CSineProfiles();
	~CSineProfiles();
	void PrintVectorSineProfiles(vector <CSineProfiles>& aVec);
	void InsertToSineProfiles(sqlite3* aDB, vector <CSineProfiles>& aVec);
	vector <CSineProfiles> FetchFromSineProfiles(sqlite3* aDB, int aProfileID);
	friend int callbackSineProfiles(void* aVector, int argc, char** argv, char** azColName);
	/*vychadza z kniznice p. Novaka*/ void InsertFrom_SineProfiles_ToTestFileSine(string aFile, vector <CSineProfiles>& aVec);
	};


/************************ DB tests_parameters ***********************/

	//tabulka test_parameters_sine
class CSineVibrations
	{
	public:
	int TestFileYear, TestFileNumber, AmplitudeType, SweepType, MultiChannelControl, StartFrequency, NumberOfWarnLimitViolations, NumberOfAbortLimitViolations,
		PreTestLevel, ConnectionWithSlipTable;
	double MassTestItems, MassTestFixture;
	int TestDurationBoxes;
	double SweepVelocity;
	int TestTime;
	double CurveSave;
	int	CurveSaveCycle;
	double FrequencyOverload;

	CSineVibrations();
	~CSineVibrations();
	void PrintVectorSineVibrations();
	void InsertToSineVibrations(sqlite3* aDB);
	CSineVibrations& FetchFromSineVibrations(sqlite3* aDB, int aTestYear, int aTestNumber);
	friend int callbackSineVibrations(void* aVector, int argc, char** argv, char** azColName);
	/*vychadza z kniznice p. Novaka*/ void InsertFrom_SineVibrations_ToTestFileSine(string aFile);
	/*vychadza z kniznice p. Novaka*/ CSineVibrations& FetchFrom_TestFileSine_ToSineVibrations(string aFile);
	string UpdateSineVibrations(sqlite3* aDB, CSineVibrations old);		//*this je novy zaznam
	};


	//tabulka notching
class CNotching
	{
	int TestYear, TestNumber, ChannelNumber;
	double Level, LowerFrequency, UpperFrequency;

	public:
		CNotching();
		~CNotching();
		void PrintVectorNotching(vector <CNotching>& aVec);
		void InsertToNotching(sqlite3* aDB, vector <CNotching>& aVec);
		vector <CNotching> FetchFromNotching(sqlite3* aDB, int aTestNumber);
		friend int callbackNotching(void* aVector, int argc, char** argv, char** azColName);
		/*vychadza z kniznice p. Novaka*/ void InsertFrom_Notching_ToTestFileSine(string aFile, vector <CNotching>& aVec, vector <CVibrationsCommon>& aSnimace);
	};
#endif