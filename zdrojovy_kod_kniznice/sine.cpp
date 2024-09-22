#include "sine.h"
#include <iostream>


/************************ funkcie pre prevod dat z rozhrania do testovacieho suboru Sine ***********************/
	

//premenovanie na *.sin subor
string RenameToSin(string aMDBFile)
	{
	string sinFile = aMDBFile.substr(0, aMDBFile.find_last_of('.')) + ".sin";
	int ret = rename(aMDBFile.c_str(), sinFile.c_str());

	return sinFile;
	}


/************************ DB test_profiles ***********************/

		/************************ tabulka sine_profiles ***********************/


// implicitny konstruktor objektu CSinusProfiles (vytvori 1 zaznam tabulky sine_profiles)
CSineProfiles::CSineProfiles(): ProfileID(0), Frequency(1), Acceleration(1), WarnLimitPositive(5), WarnLimitNegative(1),
								AbortLimitPositive(3), AbortLimitNegative(1) {}


// destruktor objektu CSinusProfiles (zaznamu tabulky sine_profiles)
CSineProfiles::~CSineProfiles() {}


//tisk zaznamov CSineProfiles, ktore su ulozene vo vektore 
void CSineProfiles::PrintVectorSineProfiles(vector <CSineProfiles>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		cout << "ProfileID: " << aVec[i].ProfileID << endl;
		cout << "Frequency: " << aVec[i].Frequency << endl;
		cout << "Acceleration: " << aVec[i].Acceleration << endl;
		cout << "WarnLimitPositive: " << aVec[i].WarnLimitPositive << endl;
		cout << "WarnLimitNegative: " << aVec[i].WarnLimitNegative << endl;
		cout << "AbortLimitPositive: " << aVec[i].AbortLimitPositive << endl;
		cout << "AbortLimitNegative: " << aVec[i].AbortLimitNegative << endl;
		cout << endl;
		}
	}


//vyplnenie zaznamu v DB test_profiles - tabulka sine_profiles
void CSineProfiles::InsertToSineProfiles(sqlite3* aDB, vector <CSineProfiles>& aVec)
	{

	for(size_t i = 0; i < aVec.size(); i++)
		{

		string sql = "INSERT INTO sine_profiles VALUES (" + to_string(aVec[i].ProfileID) + "," + to_string(aVec[i].Frequency) + "," 
			+ to_string(aVec[i].Acceleration) + "," + to_string(aVec[i].WarnLimitPositive) + "," + to_string(aVec[i].WarnLimitNegative) + "," 
			+ to_string(aVec[i].AbortLimitPositive) + "," + to_string(aVec[i].AbortLimitNegative) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackSineProfiles(void* aVector, int argc, char** argv, char** azColName)
	{
	vector <CSineProfiles>* tmpVector = (vector <CSineProfiles>*) aVector;
	CSineProfiles tmpClass;

	tmpClass.ProfileID = stoi(argv[0]);
	tmpClass.Frequency = atof(argv[1]);
	tmpClass.Acceleration = atof(argv[2]);
	tmpClass.WarnLimitPositive = atof(argv[3]);
	tmpClass.WarnLimitNegative = atof(argv[4]);
	tmpClass.AbortLimitPositive = atof(argv[5]);
	tmpClass.AbortLimitNegative = atof(argv[6]);

	tmpVector->push_back(tmpClass);
	return 0;
	}


//ulozenie zaznamov z DB test_profiles - tabulka sine_profiles, do rozhrania (do kontajneru s objektmi CSineProfiles)
// podla ID profilu
vector <CSineProfiles> CSineProfiles::FetchFromSineProfiles(sqlite3* aDB, int aProfileID)
	{
	string sql = "SELECT * FROM sine_profiles WHERE profileId ==" + to_string(aProfileID) + ";";

	vector <CSineProfiles> tmp;
	sqlite3_exec(aDB, sql.c_str(), callbackSineProfiles, &tmp, NULL);
	return tmp;
	}


//prevod dat z triedy CSineProfiles do testovacieho suboru Sine
/*vychadza z kniznice p. Novaka*/
void CSineProfiles::InsertFrom_SineProfiles_ToTestFileSine(string aFile, vector <CSineProfiles>& aVec)
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
		//prepocet na testovacie hodnoty
		aVec[i].WarnLimitNegative = aVec[i].Acceleration * pow(10, (float)-aVec[i].WarnLimitNegative / 20);
		aVec[i].WarnLimitPositive = aVec[i].Acceleration * pow(10, (float)aVec[i].WarnLimitPositive / 20);
		aVec[i].AbortLimitNegative = aVec[i].Acceleration * pow(10, (float)-aVec[i].AbortLimitNegative / 20);
		aVec[i].AbortLimitPositive = aVec[i].Acceleration * pow(10, (float)aVec[i].AbortLimitPositive / 20);

		if(i < 2)		//prve dva zaznamy su v subore na pevno, tie sa iba updatuju
			{
			string sql = "UPDATE TargetCurve SET WarnLower_Y = " + to_string(aVec[i].WarnLimitNegative) + ", WarnUpper_Y = " 
				+ to_string(aVec[i].WarnLimitPositive) + ", AbortLower_Y = " + to_string(aVec[i].AbortLimitNegative) + ", AbortUpper_Y = " 
				+ to_string(aVec[i].AbortLimitPositive) + ", Y_Data = " + to_string(aVec[i].Acceleration) + ", X_Data = " 
				+ to_string(aVec[i].Frequency) + " WHERE TargetCurve.[Index] = " + to_string(i + 1);

			SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
			}

		else        //dalsie sa uz insertuju
			{
			string sql = "INSERT INTO TargetCurve VALUES (" + to_string(aVec[i].WarnLimitNegative) + ", " + to_string(aVec[i].WarnLimitPositive) + ", " 
				+ to_string(aVec[i].AbortLimitNegative) + ", " + to_string(aVec[i].AbortLimitPositive) + ", " + to_string(aVec[i].Acceleration) + ", " 
				+ to_string(aVec[i].Frequency) + ", " + to_string(i + 1) + ")";

			SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);

			string sql_number_of_rows = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(i + 1) + " WHERE SystemDataFloat.[Index] = 4 OR SystemDataFloat.[Index] = 200";
			SQLExecDirect(hstmt, (SQLCHAR*)sql_number_of_rows.c_str(), SQL_NTS);
			}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	

/************************ DB tests_parameters ***********************/

		/************************ tabulka sine_vibrations ***********************/

// implicitny konstruktor objektu CRandomVibrations (vytvori 1 zaznam tabulky random_vibrations)
CSineVibrations::CSineVibrations(): TestFileYear(0), TestFileNumber(0), AmplitudeType(2), SweepType(1), MultiChannelControl(1), StartFrequency(1),  
									NumberOfWarnLimitViolations(1), NumberOfAbortLimitViolations(1), PreTestLevel(1), ConnectionWithSlipTable(1),
									MassTestItems(0), MassTestFixture(0), TestDurationBoxes(1001), SweepVelocity(5),  
									TestTime(1), CurveSave(0), CurveSaveCycle(6), FrequencyOverload(0) {}


// destruktor objektu CSineVibrations (zaznamu tabulky sine_vibrations)
CSineVibrations::~CSineVibrations() {}


//tisk zaznamov CSineVibrations, ktore su ulozene vo vektore 
void CSineVibrations::PrintVectorSineVibrations()
	{
	cout << "TestFileYear: " << TestFileYear << endl;
	cout << "TestFileNumber: " << TestFileNumber << endl;
	cout << "AmplitudeType: " << AmplitudeType << endl;
	cout << "SweepType: " << SweepType << endl;
	cout << "MultiChannelControl: " << MultiChannelControl << endl;
	cout << "StartFrequency: " << StartFrequency << endl;
	cout << "NumberOfWarnLimitViolations: " << NumberOfWarnLimitViolations << endl;
	cout << "NumberOfAbortLimitViolations: " << NumberOfAbortLimitViolations << endl;
	cout << "PreTestLevel: " << PreTestLevel << endl;
	cout << "ConnectionWithSlipTable: " << ConnectionWithSlipTable << endl;
	cout << "MassTestItems: " << MassTestItems << endl;
	cout << "MassTestFixture: " << MassTestFixture << endl;
	cout << "TestDurationBoxes: " << TestDurationBoxes << endl;
	cout << "SweepVelocity: " << SweepVelocity << endl;
	cout << "TestTime: " << TestTime << endl;
	cout << fixed << setprecision(11) << "CurveSave: " << CurveSave << endl;
	cout << "CurveSaveCycle: " << CurveSaveCycle << endl;
	cout << "FrequencyOverload: " << FrequencyOverload << endl;
	cout << endl;
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka test_parameters_sine
void CSineVibrations::InsertToSineVibrations(sqlite3* aDB)
	{

	//nastavenie Curve Save na presny pocet desatinnych miest
	ostringstream stringObj;
	stringObj << fixed << setprecision(11);
	stringObj << CurveSave;
	string strCurveSave = stringObj.str();

	string sql = "INSERT INTO test_parameters_sine VALUES (" + to_string(TestFileYear) + "," + to_string(TestFileNumber) + ","
		+ to_string(AmplitudeType) + "," + to_string(SweepType) + "," + to_string(MultiChannelControl) + ","
		+ to_string(StartFrequency) + "," + to_string(NumberOfWarnLimitViolations) + ","
		+ to_string(NumberOfAbortLimitViolations) + "," + to_string(PreTestLevel) + "," + to_string(ConnectionWithSlipTable) + ","
		+ to_string(MassTestItems) + "," + to_string(MassTestFixture) + "," + to_string(TestDurationBoxes) + ","
		+ to_string(SweepVelocity) + "," + to_string(TestTime) + "," + strCurveSave + "," + to_string(CurveSaveCycle) + "," + to_string(FrequencyOverload) + ");";

	sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		
	}


// funkcia dolezita pre dotaz SELECT
int callbackSineVibrations(void* aClass, int argc, char** argv, char** azColName)
	{
	CSineVibrations* tmpClass = (CSineVibrations*) aClass;

	tmpClass->TestFileYear = stoi(argv[0]);
	tmpClass->TestFileNumber = stoi(argv[1]);
	tmpClass->AmplitudeType = stoi(argv[2]);
	tmpClass->SweepType = stoi(argv[3]);
	tmpClass->MultiChannelControl = stoi(argv[4]);
	tmpClass->StartFrequency = stoi(argv[5]);
	tmpClass->NumberOfWarnLimitViolations = stoi(argv[6]);
	tmpClass->NumberOfAbortLimitViolations = stoi(argv[7]);
	tmpClass->PreTestLevel = stoi(argv[8]);
	tmpClass->ConnectionWithSlipTable = stoi(argv[9]);
	tmpClass->MassTestItems = atof(argv[10]);
	tmpClass->MassTestFixture = atof(argv[11]);
	tmpClass->TestDurationBoxes = stoi(argv[12]);
	tmpClass->SweepVelocity = atof(argv[13]);
	tmpClass->TestTime = stoi(argv[14]);
	tmpClass->CurveSave = atof(argv[15]);
	tmpClass->CurveSaveCycle = stoi(argv[16]);
	tmpClass->FrequencyOverload = atof(argv[17]);
	return 0;
	}


//ulozenie zaznamu z DB tests_parameters - tabulka test_parameters_sine, do rozhrania
// podla ID profilu   alebo  TestNumber
CSineVibrations& CSineVibrations::FetchFromSineVibrations(sqlite3* aDB, int aTestYear, int aTestNumber)
	{
	string sql = "SELECT * FROM test_parameters_sine WHERE testFileNumber = " + to_string(aTestNumber) + " AND " + "testFileYear = " + to_string(aTestYear) + ";";
	sqlite3_exec(aDB, sql.c_str(), callbackSineVibrations, this, NULL);
	return *this;
	}


//prevod dat z triedy SineVibrations do testovacieho suboru Sine
/*vychadza z kniznice p. Novaka*/
void CSineVibrations::InsertFrom_SineVibrations_ToTestFileSine(string aFile)
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

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(AmplitudeType) + " WHERE SystemDataFloat.[Index] = 205";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(SweepType) + " WHERE SystemDataFloat.[Index] = 202";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MultiChannelControl) + " WHERE SystemDataFloat.[Index] = 204";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(StartFrequency) + " WHERE SystemDataFloat.[Index] = 206";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(NumberOfWarnLimitViolations) + " WHERE SystemDataFloat.[Index] = 215";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(NumberOfAbortLimitViolations) + " WHERE SystemDataFloat.[Index] = 216";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(PreTestLevel) + " WHERE SystemDataFloat.[Index] = 207";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ConnectionWithSlipTable) + " WHERE SystemDataFloat.[Index] = 218";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestItems) + " WHERE SystemDataFloat.[Index] = 209";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestFixture) + " WHERE SystemDataFloat.[Index] = 217";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(TestDurationBoxes) + " WHERE SystemDataFloat.[Index] = 212";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(SweepVelocity) + " WHERE SystemDataFloat.[Index] = 201";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(TestTime) + " WHERE SystemDataFloat.[Index] = 203";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	//nastavenie Curve Save na presny pocet desatinnych miest
	ostringstream stringObj;
	stringObj << fixed << setprecision(11);
	stringObj << CurveSave;
	string strCurveSave = stringObj.str();

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + strCurveSave + " WHERE SystemDataFloat.[Index] = 211";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(CurveSaveCycle * 2) + " WHERE SystemDataFloat.[Index] = 210";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


//vytiahnutie nastaveni z testovacieho suboru Sine
/*vychadza z kniznice p. Novaka*/
CSineVibrations& CSineVibrations::FetchFrom_TestFileSine_ToSineVibrations(string aFile)
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
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "205").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &AmplitudeType, sizeof(AmplitudeType), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "202").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &SweepType, sizeof(SweepType), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "204").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &MultiChannelControl, sizeof(MultiChannelControl), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "206").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &StartFrequency, sizeof(StartFrequency), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "215").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &NumberOfWarnLimitViolations, sizeof(NumberOfWarnLimitViolations), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "216").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &NumberOfAbortLimitViolations, sizeof(NumberOfAbortLimitViolations), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "207").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &PreTestLevel, sizeof(PreTestLevel), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "218").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &ConnectionWithSlipTable, sizeof(ConnectionWithSlipTable), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "209").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestItems, sizeof(MassTestItems), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "217").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestFixture, sizeof(MassTestFixture), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "212").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &TestDurationBoxes, sizeof(TestDurationBoxes), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "201").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &SweepVelocity, sizeof(SweepVelocity), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "203").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &TestTime, sizeof(TestTime), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "211").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_DOUBLE, &CurveSave, sizeof(CurveSave), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "210").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &CurveSaveCycle, sizeof(CurveSaveCycle), NULL);
	CurveSaveCycle = CurveSaveCycle / 2;
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);


	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}


//update zaznamu v tabulke test_parameters_sine, podla toho ci sa hodnoty po teste zmenili
string CSineVibrations::UpdateSineVibrations(sqlite3* aDB, CSineVibrations old)
	{
	if(AmplitudeType != old.AmplitudeType || SweepType != old.SweepType || MultiChannelControl != old.MultiChannelControl ||
	   StartFrequency != old.StartFrequency || NumberOfWarnLimitViolations != old.NumberOfWarnLimitViolations ||
	   NumberOfAbortLimitViolations != old.NumberOfAbortLimitViolations || PreTestLevel != old.PreTestLevel || ConnectionWithSlipTable != old.ConnectionWithSlipTable ||
	   MassTestItems != old.MassTestItems || MassTestFixture != old.MassTestFixture || TestDurationBoxes != old.TestDurationBoxes || SweepVelocity != old.SweepVelocity ||
	   TestTime != old.TestTime || CurveSave != old.CurveSave || CurveSaveCycle != old.CurveSaveCycle)
		{

		//nastavenie Curve Save na presny pocet desatinnych miest
		ostringstream stringObj;
		stringObj << fixed << setprecision(11);
		stringObj << CurveSave;
		string strCurveSave = stringObj.str();
		
		string sql = "UPDATE test_parameters_sine SET amplitudeType = " + to_string(AmplitudeType) + "," + " sweepType = "
			+ to_string(SweepType) + "," + " multiChannelControl = " + to_string(MultiChannelControl) + "," + " startFrequency = "
			+ to_string(StartFrequency) + "," + " numberOfWarnLimitViolations = " + to_string(NumberOfWarnLimitViolations) + ","
			+ " numberOfAbortLimitViolations = " + to_string(NumberOfAbortLimitViolations) + "," + " pretestLevel = " + to_string(PreTestLevel)
			+ "," + " connectionWithSlipTable = " + to_string(ConnectionWithSlipTable) + "," + " massTestItems = " + to_string(MassTestItems) + ","
			+ " massTestFixture = " + to_string(MassTestFixture) + "," + " testDurationBoxes = " + to_string(TestDurationBoxes) + "," + " sweepVelocity = "
			+ to_string(SweepVelocity) + "," + " testTimePerAxis = " + to_string(TestTime) + "," + " curveSave = "
			+ strCurveSave + "," + " curveSaveCycle = " + to_string(CurveSaveCycle) + " WHERE testFileNumber = " + to_string(old.TestFileNumber) + " AND " + "testFileYear = " + to_string(old.TestFileYear);

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);

		return "Zmena v test_parameters_sine; ";
		}

	return "";
	}


/************************ DB tests_parameters ***********************/

		/************************ tabulka notching ***********************/

// implicitny konstruktor objektu CNotching (vytvori 1 zaznam tabulky notching)
CNotching::CNotching(): TestYear(0), TestNumber(1), ChannelNumber(1), Level(1), LowerFrequency(50), UpperFrequency(70) {}


// destruktor objektu CNotching (zaznamu tabulky notching)
CNotching::~CNotching() {}


//tisk zaznamov CNotching, ktore su ulozene vo vektore 
void CNotching::PrintVectorNotching(vector <CNotching>& aVec)
	{

	for(size_t i = 0; i < aVec.size(); i++)
		{
		cout << "TestYear: " << aVec[i].TestYear << endl;
		cout << "TestNumber: " << aVec[i].TestNumber << endl;
		cout << "ChannelNumber: " << aVec[i].ChannelNumber << endl;
		cout << "Level: " << aVec[i].Level << endl;
		cout << "LowerFrequency: " << aVec[i].LowerFrequency << endl;
		cout << "UpperFrequency: " << aVec[i].UpperFrequency << endl;
		cout << endl;
		}
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka notching
void CNotching::InsertToNotching(sqlite3* aDB, vector <CNotching>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		string sql = "INSERT INTO notching VALUES (" + to_string(aVec[i].TestYear) + "," + to_string(aVec[i].TestNumber) + ","
			+ to_string(aVec[i].ChannelNumber) + "," + to_string(aVec[i].Level) + "," + to_string(aVec[i].LowerFrequency) + ","
			+ to_string(aVec[i].UpperFrequency) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackNotching(void* aVector, int argc, char** argv, char** azColName)
	{
	vector <CNotching>* tmpVector = (vector <CNotching>*) aVector;
	CNotching tmpClass;

	tmpClass.TestYear = stoi(argv[0]);
	tmpClass.TestNumber = stoi(argv[1]);
	tmpClass.ChannelNumber = stoi(argv[2]);
	tmpClass.Level = atof(argv[3]);
	tmpClass.LowerFrequency = atof(argv[4]);
	tmpClass.UpperFrequency = atof(argv[5]);

	tmpVector->push_back(tmpClass);
	return 0;
	}


//ulozenie zaznamov z DB tests_parameters - tabulka notching, do rozhrania (do kontajneru s objektmi CNotching)
//vyber podla TestNumber
vector <CNotching> CNotching::FetchFromNotching(sqlite3* aDB, int aTestNumber)
	{
	vector <CNotching> tmp;

	string sql = "SELECT * FROM notching WHERE testNumber ==" + to_string(aTestNumber) + ";";
	sqlite3_exec(aDB, sql.c_str(), callbackNotching, &tmp, NULL);
	return tmp;
	}


//prevod dat z triedy CNotching do testovacieho suboru Sine, zavisi na DB tests_parameters - tabulke vibrations_common - stlpec Type of Input a Notching
//stlpce Type of Input a Notching budu urcovat, ci sa parametre z tabulky notching prenesu do test. suboru alebo nie
/*vychadza z kniznice p. Novaka*/
void CNotching::InsertFrom_Notching_ToTestFileSine(string aFile, vector <CNotching>& aVec, vector <CVibrationsCommon>& aSnimace)
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

	int* tmpTypeOfInput = aSnimace[0].GetTypeOfInput();
	int* tmpNotching = aSnimace[0].GetNotching();
	int offset1 = 0, offset2 = 0, offset3 = 0, offset4 = 0, offset5 = 0, offset6 = 0, offset7 = 0, offset8 = 0;

	double var = 2.36942782761724e-38;
	ostringstream stringObj;
	stringObj << setprecision(15);
	stringObj << var;
	string strNotching = stringObj.str();

	{string string = "UPDATE SystemDataFloat SET SystemDataFloat = " + strNotching + " WHERE SystemDataFloat.[Index] = 213 OR SystemDataFloat.[Index] = 214";
	SQLExecDirect(hstmt, (SQLCHAR*)string.c_str(), SQL_NTS);}

	for(size_t i = 0; i < aVec.size(); i++)
		{

		if(*(tmpTypeOfInput + aVec[i].ChannelNumber - 1) != 0)
			{

			if(*(tmpNotching + aVec[i].ChannelNumber - 1) != 0)
				{

				if(aVec[i].ChannelNumber == 1)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(182 + offset1);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(187 + offset1);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(192 + offset1);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset1++;
					}

				if(aVec[i].ChannelNumber == 2)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(167 + offset2);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(172 + offset2);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(177 + offset2);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset2++;
					}

				if(aVec[i].ChannelNumber == 3)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(152 + offset3);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(157 + offset3);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(162 + offset3);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset3++;
					}

				if(aVec[i].ChannelNumber == 4)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(137 + offset4);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(142 + offset4);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(147 + offset4);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset4++;
					}

				if(aVec[i].ChannelNumber == 5)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(122 + offset5);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(127 + offset5);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(132 + offset5);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset5++;
					}

				if(aVec[i].ChannelNumber == 6)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(107 + offset6);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(112 + offset6);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(117 + offset6);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset6++;
					}

				if(aVec[i].ChannelNumber == 7)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(92 + offset7);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(97 + offset7);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(102 + offset7);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset7++;
					}

				if(aVec[i].ChannelNumber == 8)
					{
					string sql_level = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].Level) + " WHERE SystemDataFloat.[Index] = " + to_string(77 + offset8);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_level.c_str(), SQL_NTS);

					string sql_lowFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].LowerFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(82 + offset8);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_lowFreq.c_str(), SQL_NTS);

					string sql_upperFreq = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(aVec[i].UpperFrequency) + " WHERE SystemDataFloat.[Index] = " + to_string(87 + offset8);
					SQLExecDirect(hstmt, (SQLCHAR*)sql_upperFreq.c_str(), SQL_NTS);
					offset8++;
					}
				}
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, henv);

	}	