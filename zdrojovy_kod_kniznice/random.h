#ifndef RANDOM_H
#define RANDOM_H

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
#include <fstream>
#include <ctime>

using namespace std;

/************************ funkcie na vytvorenie testovacieho suboru Random ***********************/

//vytvorenie testovacieho suboru
void CreateTestFile(string aDefaultFile, string aNewFile);


//premenovanie na *.mdb subor
string RenameToMDB(string aFile);


//premenovanie na *.rau subor
string RenameToRau(string aMDBFile);


//funkcia na vytiahnutie casov testu z logbook
vector<string> FetchLogbookTimes(string aFile);

//funkcia na citanie config suboru
vector<string> ReadConfigFile(string location);

/************************ DB test_profiles ***********************/

	// tabulka random_profiles

	class CRandomProfiles
		{
		public:
		int ProfileID;
		double Frequency, Amplitude, WarnLimitPositive, WarnLimitNegative, AbortLimitPositive, AbortLimitNegative;

		CRandomProfiles();
		~CRandomProfiles();
		void PrintVectorRandomProfiles(vector <CRandomProfiles>& aVec);
		void InsertToRandomProfiles(sqlite3* aDB, vector <CRandomProfiles>& aVec);
		vector <CRandomProfiles> FetchFromRandomProfiles(sqlite3* aDB, int aProfileID);
		friend int callbackRandomProfiles(void* aVector, int argc, char** argv, char** azColName);
		/*vychadza z kniznice p. Novaka*/ void InsertFrom_RandomProfiles_ToTestFileRandom(string aFile, vector <CRandomProfiles>& aVec);
		};


/************************ DB tests_parameters ***********************/

	//tabulka random_vibrations		
	class CRandomVibrations
		{
		public:
		int TestFileYear, TestFileNumber;
		double Clipping;
		int MultiChannelControl;
		double MassTestItems, MassTestFixture;
		int ConnectionWithSlipTable, WarnLimitWithLines, AbortLimitWithLines, DegreesOfFreedom, PreTestLevel, LevelShift;
		double CurveSave;
		int CurveSaveTime, EnablePosValues, SelectionOfBandwidth;

		public:
			CRandomVibrations();
			~CRandomVibrations();
			void PrintVectorRandomVibrations();
			void InsertToRandomVibrations(sqlite3* aDB);
			CRandomVibrations& FetchFromRandomVibrations(sqlite3* aDB, int TestYear, int TestNumber);
			friend int callbackRandomVibrations(void* aClass, int argc, char** argv, char** azColName);
			/*vychadza z kniznice p. Novaka*/ void InsertFrom_RandomVibrations_ToTestFileRandom(string aFile);
			/*vychadza z kniznice p. Novaka*/ CRandomVibrations& FetchFrom_TestFileRandom_ToRandomVibrations(string aFile);
			string UpdateRandomVibrations(sqlite3* aDB, CRandomVibrations old);		//*this je novy zaznam 
		};


	//tabulka level
	class CLevels
		{
		public:
		int TestYear, TestNumber, Level, Time;

		CLevels();
		~CLevels();
		void PrintVectorLevels(vector <CLevels>& aVec);
		void InsertToLevels(sqlite3* aDB, vector <CLevels>& aVec);
		vector <CLevels> FetchFromLevels(sqlite3* aDB, int aTestYear, int aTestNumber);
		friend int callbackLevels(void* aVector, int argc, char** argv, char** azColName);
		/*vychadza z kniznice p. Novaka*/ void InsertFrom_Levels_ToTestFileRandom(string aFile, vector <CLevels>& aVec);
		};
#endif