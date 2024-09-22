#include "random.h"
#include <iostream>
using namespace std;

/************************ funkcie pre prevod dat z rozhrania do testovacieho suboru Random ***********************/

//vytvorenie testovacieho suboru
void CreateTestFile(string aDefaultFile, string aNewFile)
	{
	LPCSTR defaultFile = aDefaultFile.c_str();
	LPCSTR testFile = aNewFile.c_str();

	CopyFile(defaultFile, testFile, TRUE);
	}


//premenovanie na *.mdb subor
string RenameToMDB(string aFile)
	{
	string mdbFile = aFile.substr(0, aFile.find_last_of('.')) + ".mdb";
	int ret=rename(aFile.c_str(), mdbFile.c_str());

	return mdbFile;
	}


//premenovanie na *.rau subor
string RenameToRau(string aMDBFile)
	{
	string rauFile = aMDBFile.substr(0, aMDBFile.find_last_of('.')) + ".rau";
	int ret = rename(aMDBFile.c_str(), rauFile.c_str());

	return rauFile;
	}


//vytiahnutie casov testu z logbook
vector<string> FetchLogbookTimes(string aFile)
	{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	HWND desktopHandle = GetDesktopWindow();

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	string str = "Driver={Microsoft Access Driver (*.mdb)}; DBQ=" + aFile;
	SQLCHAR* sql_connect = (SQLCHAR*)str.c_str();

	SQLDriverConnect(hdbc, desktopHandle, sql_connect, SQL_NTS, OutConnStr, 255, &OutConnStrLen, SQL_DRIVER_NOPROMPT);

	string sql = "SELECT Info FROM Logbook WHERE Logbook.[Index] = 1";
	char c_strStartTime[256] = "";
	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DEFAULT, &c_strStartTime, 255, NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	sql = "SELECT Date FROM Logbook WHERE Logbook.[Info] = 'Stop Test time elapsed'";
	char c_strStopTime[256] = "";
	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DEFAULT, &c_strStopTime, 255, NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	vector<string> logbook_times;

	string start_time = c_strStartTime;
	string stop_time = c_strStopTime;

	start_time = start_time.erase(0, 6).insert(6, "20");
	logbook_times.push_back(start_time);

	if(!stop_time.empty())
		{
		tm start;

		start.tm_mday = stoi(start_time.substr(0, 2));
		start.tm_mon = stoi(start_time.substr(3, 2)) - 1;
		start.tm_year = stoi(start_time.substr(6, 4)) - 1900;
		start.tm_hour = stoi(start_time.substr(11, 2));
		start.tm_min = stoi(start_time.substr(14, 2));
		start.tm_sec = stoi(start_time.substr(17, 2));

		int days = stoi(stop_time.substr(0, 2));
		int hours = stoi(stop_time.substr(3, 2));
		int minutes = stoi(stop_time.substr(6, 2));
		int seconds = stoi(stop_time.substr(9, 2));

		int seconds_duration = days * 86400 + hours * 3600 + minutes * 60 + seconds;

		time_t start_epoch = mktime(&start);
		time_t end_epoch = start_epoch + seconds_duration;

		const int MAXLEN = 80;
		char s[MAXLEN];
		strftime(s, MAXLEN, "%d.%m.%Y %H:%M:%S", localtime(&end_epoch));

		logbook_times.push_back(s);

		return logbook_times;
		}

	logbook_times.push_back(stop_time);

	return logbook_times;
	}


vector<string> ReadConfigFile(string location)
	{
	ifstream file(location);
	vector<string> vec_out;
	string out;
	while(getline(file, out))
		vec_out.push_back(out);

	return vec_out;
	}


/************************ DB test_profiles ***********************/

		/************************ tabulka random_profiles ***********************/

// implicitny konstruktor objektu CRandomProfiles (vytvori 1 zaznam tabulky profiles_random)
CRandomProfiles::CRandomProfiles(): ProfileID(0), Frequency(1), Amplitude(1), WarnLimitPositive(1), WarnLimitNegative(1),
									AbortLimitPositive(1), AbortLimitNegative(1) {}


// destruktor objektu CRandomProfiles (zaznamu tabulky profiles_random)
CRandomProfiles::~CRandomProfiles() {}


//tisk zaznamov CRandomProfiles, ktore su ulozene vo vektore 
void CRandomProfiles::PrintVectorRandomProfiles(vector <CRandomProfiles>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		cout << "ProfileID: " << aVec[i].ProfileID << endl;
		cout << "Frequency: " << aVec[i].Frequency << endl;
		cout << "Level: " << aVec[i].Amplitude << endl;
		cout << "WarnLimitPositive: " << aVec[i].WarnLimitPositive << endl;
		cout << "WarnLimitNegative: " << aVec[i].WarnLimitNegative << endl;
		cout << "AbortLimitPositive: " << aVec[i].AbortLimitPositive << endl;
		cout << "AbortLimitNegative: " << aVec[i].AbortLimitNegative << endl;
		cout << endl;
		}
	}


//vyplnenie zaznamu v DB test_profiles - tabulka profiles_random
void CRandomProfiles::InsertToRandomProfiles(sqlite3* aDB, vector <CRandomProfiles>& aVec)
	{

	for(size_t i = 0; i < aVec.size(); i++)
		{
		string sql = "INSERT INTO profiles_random VALUES (" + to_string(aVec[i].ProfileID) + "," + to_string(aVec[i].Frequency) + "," 
			+ to_string(aVec[i].Amplitude) + "," + to_string(aVec[i].WarnLimitPositive) + "," + to_string(aVec[i].WarnLimitNegative) + "," 
			+ to_string(aVec[i].AbortLimitPositive) + "," + to_string(aVec[i].AbortLimitNegative) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackRandomProfiles(void* aVector, int argc, char** argv, char** azColName)
	{
	vector <CRandomProfiles>* tmpVector = (vector <CRandomProfiles>*) aVector;
	CRandomProfiles tmpClass;

	tmpClass.ProfileID = stoi(argv[0]);
	tmpClass.Frequency = atof(argv[1]);
	tmpClass.Amplitude = atof(argv[2]);
	tmpClass.WarnLimitPositive = atof(argv[3]);
	tmpClass.WarnLimitNegative = atof(argv[4]);
	tmpClass.AbortLimitPositive = atof(argv[5]);
	tmpClass.AbortLimitNegative = atof(argv[6]);

	tmpVector->push_back(tmpClass);
	return 0;
	}


//ulozenie zaznamov z DB test_profiles - tabulka profiles_random, do rozhrania (do kontajneru s objektmi CRandomProfiles)
// podla ID profilu
vector <CRandomProfiles> CRandomProfiles::FetchFromRandomProfiles(sqlite3* aDB, int aProfileID)
	{
	string sql = "SELECT * FROM profiles_random WHERE adProfileId = " + to_string(aProfileID);

    vector <CRandomProfiles> tmp;
	sqlite3_exec(aDB, sql.c_str(), callbackRandomProfiles, &tmp, NULL);
	return tmp;
	}


//prevod dat z triedy CRandomProfiles do testovacieho suboru Random
/*vychadza z kniznice p. Novaka*/ 
void CRandomProfiles::InsertFrom_RandomProfiles_ToTestFileRandom(string aFile, vector <CRandomProfiles>& aVec)
	{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	HWND desktopHandle = GetDesktopWindow();

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	string str = "Driver={Microsoft Access Driver (*.mdb)}; DBQ= " + aFile;
	SQLCHAR* sql_connect = (SQLCHAR*)str.c_str();

	SQLDriverConnect(hdbc, desktopHandle, sql_connect, SQL_NTS, OutConnStr, 255, &OutConnStrLen, SQL_DRIVER_NOPROMPT);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	for(size_t i = 0; i < aVec.size(); i++)
		{

			//prepocet na testovacie hodnoty
			aVec[i].WarnLimitNegative = aVec[i].Amplitude * pow(10, (float)-aVec[i].WarnLimitNegative / 10);
			aVec[i].WarnLimitPositive = aVec[i].Amplitude * pow(10, (float)aVec[i].WarnLimitPositive / 10);
			aVec[i].AbortLimitNegative = aVec[i].Amplitude * pow(10, (float)-aVec[i].AbortLimitNegative / 10);
			aVec[i].AbortLimitPositive = aVec[i].Amplitude * pow(10, (float)aVec[i].AbortLimitPositive / 10);

			if(i < 2)		//prve dva zaznamy su v subore na pevno, tie sa iba updatuju
				{
				string sql = "UPDATE TargetCurve SET WarnLower_Y = " + to_string(aVec[i].WarnLimitNegative) + ", WarnUpper_Y = " + 
					to_string(aVec[i].WarnLimitPositive) + ", AbortLower_Y = " + to_string(aVec[i].AbortLimitNegative) + ", AbortUpper_Y = " + 
					to_string(aVec[i].AbortLimitPositive) + ", Y_Data = " + to_string(aVec[i].Amplitude) + ", X_Data = " + to_string(aVec[i].Frequency) 
					+ " WHERE TargetCurve.[Index] = " + to_string(i + 1);

				SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
				}

			else        //dalsie sa uz insertuju
				{
				string sql = "INSERT INTO TargetCurve VALUES (" + to_string(aVec[i].WarnLimitNegative) + ", " + to_string(aVec[i].WarnLimitPositive) 
					+ ", " + to_string(aVec[i].AbortLimitNegative) + ", " + to_string(aVec[i].AbortLimitPositive) + ", " + to_string(aVec[i].Amplitude) 
					+ ", " + to_string(aVec[i].Frequency) + ", " + to_string(i + 1) + ")";

				SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);

				string sql_number_of_rows = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(i + 1) + " WHERE SystemDataFloat.[Index] = 4";
				SQLExecDirect(hstmt, (SQLCHAR*)sql_number_of_rows.c_str(), SQL_NTS);
				}
		}

		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, henv);

		}
	
	

/************************ DB tests_parameters ***********************/

		/************************ tabulka random_vibrations ***********************/

// implicitny konstruktor objektu CRandomVibrations (vytvori 1 zaznam tabulky test_parameters_random)
CRandomVibrations::CRandomVibrations(): TestFileYear(0), TestFileNumber(0), Clipping(3), MultiChannelControl(0), MassTestItems(0), MassTestFixture(0),
										ConnectionWithSlipTable(1), WarnLimitWithLines(1), AbortLimitWithLines(1), DegreesOfFreedom(10), PreTestLevel(1),
										LevelShift(0), CurveSave(0/*1.401298e-45*/), CurveSaveTime(60), EnablePosValues(0), SelectionOfBandwidth(2000) {}


// destruktor objektu CRandomVibrations (zaznamu tabulky test_parameters_random)
CRandomVibrations::~CRandomVibrations() {}


//tisk zaznamov CRandomProfiles, ktore su ulozene vo vektore 
void CRandomVibrations::PrintVectorRandomVibrations()
	{
	cout << "TestFileYear: " << TestFileYear << endl;
	cout << "TestFileNumber: " << TestFileNumber << endl;
	cout << "Clipping: " << Clipping << endl;
	cout << "MultiChannelControl: " << MultiChannelControl << endl;
	cout << "MassTestItems: " << MassTestItems << endl;
	cout << "MassTestFixture: " << MassTestFixture << endl;
	cout << "ConnectionWithSlipTable: " << ConnectionWithSlipTable << endl;
	cout << "WarnLimitWithLines: " << WarnLimitWithLines << endl;
	cout << "AbortLimitWithLines: " << AbortLimitWithLines << endl;
	cout << "DegreesOfFreedom: " << DegreesOfFreedom << endl;
	cout << "PreTestLevel: " << PreTestLevel << endl;
	cout << "LevelShift: " << LevelShift << endl;
	cout << setprecision(7) << "CurveSave: " << CurveSave << endl;
	cout << "CurveSaveTime: " << CurveSaveTime << endl;
	cout << "EnablePosValues: " << EnablePosValues << endl;
	cout << "SelectionOfBandwidth: " << SelectionOfBandwidth << endl;
	cout << endl;
		
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka test_parameters_random
void CRandomVibrations::InsertToRandomVibrations(sqlite3* aDB)
	{

		//nastavenie Curve Save na presny pocet desatinnych miest
		ostringstream stringObj;
		stringObj << setprecision(7);
		stringObj << CurveSave;
		string strCurveSave = stringObj.str();

		string sql = "INSERT INTO test_parameters_random VALUES (" + to_string(TestFileYear) + "," + to_string(TestFileNumber) + "," 
			+ to_string(Clipping) + "," + to_string(MultiChannelControl) + "," + to_string(MassTestItems) + "," 
			+ to_string(MassTestFixture) + "," + to_string(ConnectionWithSlipTable) + "," + to_string(WarnLimitWithLines) + "," 
			+ to_string(AbortLimitWithLines) + "," + to_string(DegreesOfFreedom) + "," + to_string(PreTestLevel) + "," 
			+ to_string(LevelShift) + "," + strCurveSave + "," + to_string(CurveSaveTime) + "," 
			+ to_string(EnablePosValues) + "," + to_string(SelectionOfBandwidth) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		
	}


// funkcia dolezita pre dotaz SELECT
int callbackRandomVibrations(void* aClass, int argc, char** argv, char** azColName)
	{
	CRandomVibrations* tmp = (CRandomVibrations*) aClass;

	tmp->TestFileYear = stoi(argv[0]);
	tmp->TestFileNumber = stoi(argv[1]);
	tmp->Clipping = atof(argv[2]);
	tmp->MultiChannelControl = stoi(argv[3]);
	tmp->MassTestItems = atof(argv[4]);
	tmp->MassTestFixture = atof(argv[5]);
	tmp->ConnectionWithSlipTable = stoi(argv[6]);
	tmp->WarnLimitWithLines = stoi(argv[7]);
	tmp->AbortLimitWithLines = stoi(argv[8]);
	tmp->DegreesOfFreedom = stoi(argv[9]);
	tmp->PreTestLevel = stoi(argv[10]);
	tmp->LevelShift = stoi(argv[11]);
	tmp->CurveSave = atof(argv[12]);
	tmp->CurveSaveTime = stoi(argv[13]);
	tmp->EnablePosValues = stoi(argv[14]);
	tmp->SelectionOfBandwidth = stoi(argv[15]);

	return 0;
	}


//ulozenie zaznamu z DB tests_parameters - tabulka test_parameters_random, do rozhrania 
// podla ID profilu   alebo  TestNumber
CRandomVibrations& CRandomVibrations::FetchFromRandomVibrations(sqlite3* aDB, int TestYear, int TestNumber)
	{
	string sql = "SELECT * FROM test_parameters_random WHERE testFileNumber = " + to_string(TestNumber) + " AND testFileYear = " + to_string(TestYear) + ";";
	sqlite3_exec(aDB, sql.c_str(), callbackRandomVibrations, this, NULL);
	return *this;
	}


//prevod dat z triedy RandomVibrations do testovacieho suboru Random,  
//pocet riadkov v set level karte sa zada v metode InsertFrom_Levels_ToTestFileRandom()
/*vychadza z kniznice p. Novaka*/
void CRandomVibrations::InsertFrom_RandomVibrations_ToTestFileRandom(string aFile)
	{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	HWND desktopHandle = GetDesktopWindow();

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	string str = "Driver={Microsoft Access Driver (*.mdb)}; DBQ=" + aFile;
	SQLCHAR* sql_connect = (SQLCHAR*)str.c_str();

	SQLDriverConnect(hdbc, desktopHandle, sql_connect, SQL_NTS, OutConnStr, 255, &OutConnStrLen, SQL_DRIVER_NOPROMPT);
	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(WarnLimitWithLines) + " WHERE SystemDataFloat.[Index] = 133";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(AbortLimitWithLines) + " WHERE SystemDataFloat.[Index] = 134";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(SelectionOfBandwidth) + " WHERE SystemDataFloat.[Index] = 135";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MultiChannelControl) + " WHERE SystemDataFloat.[Index] = 136";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Clipping) + " WHERE SystemDataFloat.[Index] = 138";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(DegreesOfFreedom) + " WHERE SystemDataFloat.[Index] = 139";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LevelShift) + " WHERE SystemDataFloat.[Index] = 142";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestItems) + " WHERE SystemDataFloat.[Index] = 143";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(EnablePosValues) + " WHERE SystemDataFloat.[Index] = 144";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(PreTestLevel) + " WHERE SystemDataFloat.[Index] = 145";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(CurveSaveTime) + " WHERE SystemDataFloat.[Index] = 146";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}
	
	//nastavenie Curve Save na presny pocet desatinnych miest
	ostringstream stringObj;
	stringObj << setprecision(7);
	stringObj << CurveSave;
	string strCurveSave = stringObj.str();

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + strCurveSave + " WHERE SystemDataFloat.[Index] = 147";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestFixture) + " WHERE SystemDataFloat.[Index] = 148";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ConnectionWithSlipTable) + " WHERE SystemDataFloat.[Index] = 149";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);}
	
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	}


//vytiahnutie nastaveni z testovacieho suboru Random
/*vychadza z kniznice p. Novaka*/
CRandomVibrations& CRandomVibrations::FetchFrom_TestFileRandom_ToRandomVibrations(string aFile)
	{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	HWND desktopHandle = GetDesktopWindow();

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	string str = "Driver={Microsoft Access Driver (*.mdb)}; DBQ=" + aFile;
	SQLCHAR* sql_connect = (SQLCHAR*)str.c_str();

	SQLDriverConnect(hdbc, desktopHandle, sql_connect, SQL_NTS, OutConnStr, 255, &OutConnStrLen, SQL_DRIVER_NOPROMPT);
	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = ";


	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "133").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &WarnLimitWithLines, sizeof(WarnLimitWithLines), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "134").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &AbortLimitWithLines, sizeof(AbortLimitWithLines), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "135").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &SelectionOfBandwidth, sizeof(SelectionOfBandwidth), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "136").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &MultiChannelControl, sizeof(MultiChannelControl), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "138").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &Clipping, sizeof(Clipping), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "139").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &DegreesOfFreedom, sizeof(DegreesOfFreedom), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "142").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &LevelShift, sizeof(LevelShift), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "143").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestItems, sizeof(MassTestItems), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "144").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &EnablePosValues, sizeof(EnablePosValues), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "145").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &PreTestLevel, sizeof(PreTestLevel), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "146").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &CurveSaveTime, sizeof(CurveSaveTime), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "147").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &CurveSave, sizeof(CurveSave), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "148").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestFixture, sizeof(MassTestFixture), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "149").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &ConnectionWithSlipTable, sizeof(ConnectionWithSlipTable), NULL); 
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}


//update zaznamu v tabulke test_parameters_random, podla toho ci sa hodnoty po teste zmenili
string CRandomVibrations::UpdateRandomVibrations(sqlite3* aDB, CRandomVibrations old)
	{

	if(Clipping != old.Clipping || MultiChannelControl != old.MultiChannelControl || MassTestItems != old.MassTestItems || 
	   MassTestFixture != old.MassTestFixture || ConnectionWithSlipTable != old.ConnectionWithSlipTable || 
	   WarnLimitWithLines != old.WarnLimitWithLines || AbortLimitWithLines != old.AbortLimitWithLines || DegreesOfFreedom != old.DegreesOfFreedom || 
	   PreTestLevel != old.PreTestLevel || LevelShift != old.LevelShift || CurveSave != old.CurveSave || CurveSaveTime != old.CurveSaveTime || 
	   EnablePosValues != old.EnablePosValues || SelectionOfBandwidth != old.SelectionOfBandwidth)
		{
		
		//nastavenie Curve Save na presny pocet desatinnych miest
		ostringstream stringObj;
		stringObj << setprecision(7);
		stringObj << CurveSave;
		string strCurveSave = stringObj.str();

		string sql = "UPDATE test_parameters_random SET clipping = " + to_string(Clipping) + "," + " multiChannelControl = "
			+ to_string(MultiChannelControl) + "," + " massTestItems = " + to_string(MassTestItems) + "," + " massTestFixture = "
			+ to_string(MassTestFixture) + "," + " connectionWithSlipTable = " + to_string(ConnectionWithSlipTable) + ","
			+ " warnLimitWithLines = " + to_string(WarnLimitWithLines) + "," + " abortLimitWithLines = " + to_string(AbortLimitWithLines)
			+ "," + " degreesOfFreedom = " + to_string(DegreesOfFreedom) + "," + " pretestLevel = " + to_string(PreTestLevel) + ","
			+ " levelShift = " + to_string(LevelShift) + "," + " curveSave = " + strCurveSave + "," + " curveSaveTime = "
			+ to_string(CurveSaveTime) + "," + " enablePosValues = " + to_string(EnablePosValues) + "," + " selectionOfBandwidth = "
			+ to_string(SelectionOfBandwidth) + " WHERE testFileNumber = " + to_string(old.TestFileNumber) + " AND testFileYear = " + to_string(old.TestFileYear);

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);

		return "Zmena v test_parameters_random; ";
		}
		
	return "";
	}


/************************ DB tests_parameters ***********************/

		/************************ tabulka levels ***********************/


// implicitny konstruktor objektu CLevels (vytvori 1 zaznam tabulky levels)
CLevels::CLevels(): TestYear(0), TestNumber(1), Level(-5), Time(69) {}


// destruktor objektu CLevels (zaznamu tabulky levels)
CLevels::~CLevels() {}


//tisk zaznamov CLevels, ktore su ulozene v rozhrani, vo vektore 
void CLevels::PrintVectorLevels(vector <CLevels>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		cout << "TestYear: " << aVec[i].TestYear << endl;
		cout << "TestNumber: " << aVec[i].TestNumber << endl;
		cout << "Level: " << aVec[i].Level << endl;
		cout << "Time: " << aVec[i].Time << endl;
		cout << endl;
		}
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka levels
void CLevels::InsertToLevels(sqlite3* aDB, vector <CLevels>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		string sql = "INSERT INTO random_levels VALUES (" + to_string(aVec[i].TestYear) + "," + to_string(aVec[i].TestNumber) + "," + to_string(aVec[i].Level) 
			+ "," + to_string(aVec[i].Time) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackLevels(void* aVector, int argc, char** argv, char** azColName)
	{
	vector <CLevels>* tmpVector = (vector <CLevels>*) aVector;
	CLevels tmpClass;

	tmpClass.TestYear = stoi(argv[0]);
	tmpClass.TestNumber = stoi(argv[1]);
	tmpClass.Level = stoi(argv[2]);
	tmpClass.Time = stoi(argv[3]);

	tmpVector->push_back(tmpClass);

	return 0;
	}


//ulozenie zaznamov z DB tests_parameters - tabulka levels, do rozhrania (do kontajneru s objektmi CLevels)
// vybera sa podla TestYear a TestNumber (zatial iba TestNumber)
vector <CLevels> CLevels::FetchFromLevels(sqlite3* aDB, int aTestYear, int aTestNumber)
	{
	vector <CLevels> tmp;

	string sql = "SELECT * FROM random_levels WHERE adTestFileYear = " + to_string(aTestYear) + " AND " + "adTestFileNumber = " + to_string(aTestNumber);
	sqlite3_exec(aDB, sql.c_str(), callbackLevels, &tmp, NULL);
	return tmp;
	}


//prevod dat z triedy CLevels do testovacieho suboru Random 
/*vychadza z kniznice p. Novaka*/
void CLevels::InsertFrom_Levels_ToTestFileRandom(string aFile, vector <CLevels>& aVec)
	{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLCHAR OutConnStr[255];
	SQLSMALLINT OutConnStrLen;
	HWND desktopHandle = GetDesktopWindow();

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	string str = "Driver={Microsoft Access Driver (*.mdb)}; DBQ=" + aFile;
	SQLCHAR* sql_connect = (SQLCHAR*)str.c_str();

	SQLDriverConnect(hdbc, desktopHandle, sql_connect, SQL_NTS, OutConnStr, 255, &OutConnStrLen, SQL_DRIVER_NOPROMPT);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	for(size_t i = 0; i < aVec.size(); i++)
		{
		
		string sql_time = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[aVec.size() - 1 - i].Time) + " WHERE SystemDataFloat.[Index] = " + to_string(112 - i);
		SQLExecDirect(hstmt, (SQLCHAR*)sql_time.c_str(), SQL_NTS);

		string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[aVec.size() - 1 - i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(132 - i);
		SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

		}

	string sql_n_rows= "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec.size()) + " WHERE SystemDataFloat.[Index] = 137" ;
	SQLExecDirect(hstmt, (SQLCHAR*)sql_n_rows.c_str(), SQL_NTS);

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
