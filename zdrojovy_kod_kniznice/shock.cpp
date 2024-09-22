#include "shock.h"
#include <iostream>


/************************ funkcie pre testovaci subor Shock ***********************/

//premenovanie na *.shk subor
string RenameToShk(string aMDBFile)
	{
	string shkFile = aMDBFile.substr(0, aMDBFile.find_last_of('.')) + ".shk";
	int ret = rename(aMDBFile.c_str(), shkFile.c_str());

	return shkFile;
	}


/************************ DB test_profiles ***********************/

		/************************ tabulka profiles_shock ***********************/


// implicitny konstruktor objektu CShockProfiles (vytvori 1 zaznam tabulky profiles_shock)
CShockProfiles::CShockProfiles(): ProfileID(0), CurveType(1), Amplitude(9000), Width(5.58) {}


// destruktor objektu CShockProfiles (zaznamu tabulky profiles_shock)
CShockProfiles::~CShockProfiles() {}


//tisk zaznamov CShockProfiles, ktore su ulozene vo vektore 
void CShockProfiles::PrintVectorShockProfiles()
	{
	cout << "ProfileID: " << ProfileID << endl;
	cout << "CurveType: " << CurveType << endl;
	cout << "Amplitude: " << Amplitude << endl;
	cout << "Width: " << Width << endl;
	cout << endl;
	}


//vyplnenie zaznamu v DB test_profiles - tabulka profiles_shock
void CShockProfiles::InsertToShockProfiles(sqlite3* aDB)
	{
	string sql = "INSERT INTO profiles_shocks VALUES (" + to_string(ProfileID) + "," + to_string(CurveType) + ","
		+ to_string(Amplitude) + "," + to_string(Width) + ");";

	sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
	}


// funkcia dolezita pre dotaz SELECT
int callbackShockProfiles(void* aClass, int argc, char** argv, char** azColName)
	{
	CShockProfiles* tmpClass = (CShockProfiles*)aClass;

	tmpClass->ProfileID = stoi(argv[0]);
	tmpClass->CurveType = stoi(argv[1]);
	tmpClass->Amplitude = stoi(argv[2]);
	tmpClass->Width = atof(argv[3]);

	return 0;
	}


//ulozenie zaznamov z DB test_profiles - tabulka profiles_shock, do rozhrania (do kontajneru s objektmi CShockProfiles)
// podla ID profilu
CShockProfiles& CShockProfiles::FetchFromShockProfiles(sqlite3* aDB, int aProfileID)
	{
	string sql = "SELECT * FROM profiles_shocks WHERE profileId = " + to_string(aProfileID);

	sqlite3_exec(aDB, sql.c_str(), callbackShockProfiles, this, NULL);
	return *this;
	}


//prevod dat z triedy CShockProfiles do testovacieho suboru Shock
/*vychadza z kniznice p. Novaka*/
void CShockProfiles::InsertFrom_ShockProfiles_ToTestFileShock(string aFile)
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

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(CurveType) + " WHERE SystemDataFloat.[Index] = 103";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Amplitude) + " WHERE SystemDataFloat.[Index] = 105";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Width) + " WHERE SystemDataFloat.[Index] = 106";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }


	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


/************************ DB tests_parameters ***********************/

		/************************ tabulka test_parameters_shocks ***********************/


// implicitny konstruktor objektu CShocks (vytvori 1 zaznam tabulky test_parameters_shocks)
CShocks::CShocks(): TestFileYear(0), TestFileNumber(1), PreShockLevel(1), ShockNumberPerDirection(1), 
					ShockDistance(55), NumberOfWarnLimitViolations(1), NumberOfAbortLimitViolations(1), MassTestItems(0), MassTestFixture(0),
					ConnectionWithSlipTable(0), PreTestLevel(1), CurveSave(-431599600), CurveSaveShocks(101), ShockInverted(1), PosPreShock(3), NegPreShock(1),
					PosPostShock(1), NegPostShock(1), Standard(2), WarnLimit(3) {}


// destruktor objektu CShocks (zaznamu tabulky test_parameters_shocks)
CShocks::~CShocks() {}


//tisk zaznamov CSineVibrations, ktore su ulozene vo vektore 
void CShocks::PrintVectorShocks()
	{
	cout << "TestFileYear: " << TestFileYear << endl;
	cout << "TestFileNumber: " << TestFileNumber << endl;
	cout << "PreShockLevel: " << PreShockLevel << endl;
	cout << "ShockNumberPerDirection: " << ShockNumberPerDirection << endl;
	cout << "ShockDistance: " << ShockDistance << endl;
	cout << "NumberOfWarnLimitViolations: " << NumberOfWarnLimitViolations << endl;
	cout << "NumberOfAbortLimitViolations: " << NumberOfAbortLimitViolations << endl;
	cout << "MassTestItems: " << MassTestItems << endl;
	cout << "MassTestFixture: " << MassTestFixture << endl;
	cout << "ConnectionWithSlipTable: " << ConnectionWithSlipTable << endl;
	cout << "PreTestLevel: " << PreTestLevel << endl;
	cout << setprecision(9) << "CurveSave: " << CurveSave << endl;
	cout << "CurveSaveShocks: " << CurveSaveShocks << endl;
	cout << "ShockInverted: " << ShockInverted << endl;
	cout << "PosPreShock: " << PosPreShock << endl;
	cout << "NegPreShock: " << NegPreShock << endl;
	cout << "PosPostShock: " << PosPostShock << endl;
	cout << "NegPostShock: " << NegPostShock << endl;
	cout << "Standard: " << Standard << endl;
	cout << "WarnLimit: " << WarnLimit << endl;
	cout << endl;	
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka test_parameters_shocks
void CShocks::InsertToShocks(sqlite3* aDB)
	{
		//nastavenie Curve Save na presny pocet desatinnych miest
		ostringstream stringObj;
		stringObj << setprecision(9);
		stringObj << CurveSave;
		string strCurveSave = stringObj.str();

		string sql = "INSERT INTO test_parameters_shocks VALUES (" + to_string(TestFileYear) + "," + to_string(TestFileNumber) + ","
			+ to_string(PreShockLevel) + "," + to_string(ShockNumberPerDirection) + "," + to_string(ShockDistance) + "," + to_string(NumberOfWarnLimitViolations) + ","
			+ to_string(NumberOfAbortLimitViolations) + "," + to_string(MassTestItems) + "," + to_string(MassTestFixture) + ","
			+ to_string(ConnectionWithSlipTable) + "," + to_string(PreTestLevel) + "," + strCurveSave + ","
			+ to_string(CurveSaveShocks) + "," + to_string(ShockInverted) + "," + to_string(PosPreShock) + "," 
			+ to_string(NegPreShock) + "," + to_string(PosPostShock) + "," + to_string(NegPostShock) + "," 
			+ to_string(Standard) + "," + to_string(WarnLimit) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
	}


// funkcia dolezita pre dotaz SELECT
int callbackShocks(void* aClass, int argc, char** argv, char** azColName)
	{
	CShocks* tmpClass = (CShocks*) aClass;

	tmpClass->TestFileYear = stoi(argv[0]);
	tmpClass->TestFileNumber = stoi(argv[1]);
	tmpClass->PreShockLevel = stoi(argv[2]);
	tmpClass->ShockNumberPerDirection = stoi(argv[3]);
	tmpClass->ShockDistance = atof(argv[4]);
	tmpClass->NumberOfWarnLimitViolations = stoi(argv[5]);
	tmpClass->NumberOfAbortLimitViolations = stoi(argv[6]);
	tmpClass->MassTestItems = atof(argv[7]);
	tmpClass->MassTestFixture = atof(argv[8]);
	tmpClass->ConnectionWithSlipTable = stoi(argv[9]);
	tmpClass->PreTestLevel = stoi(argv[10]);
	tmpClass->CurveSave = stoi(argv[11]);
	tmpClass->CurveSaveShocks = stoi(argv[12]);
	tmpClass->ShockInverted = stoi(argv[13]);
	tmpClass->PosPreShock = stoi(argv[14]);
	tmpClass->NegPreShock = stoi(argv[15]);
	tmpClass->PosPostShock = stoi(argv[16]);
	tmpClass->NegPostShock = stoi(argv[17]);
	tmpClass->Standard = stoi(argv[18]);
	tmpClass->WarnLimit = stoi(argv[19]);

	return 0;
	}


//ulozenie zaznamov z DB tests_parameters - tabulka test_parameters_shocks, do rozhrania (do kontajneru s objektmi CShocks)
// podla TestFileNumber a TestFileYear
CShocks& CShocks::FetchFromShocks(sqlite3* aDB, int aTestFileYear, int aTestFileNumber)
	{
	string sql = "SELECT * FROM test_parameters_shocks WHERE testFileNumber = " + to_string(aTestFileNumber) + " AND testFileYear = " + to_string(aTestFileYear) + ";";
	sqlite3_exec(aDB, sql.c_str(), callbackShocks, this, NULL);
	return *this;
	}


//prevod dat z triedy CShocks do testovacieho suboru Shock
/*vychadza z kniznice p. Novaka*/
void CShocks::InsertFrom_Shocks_ToTestFileShock(string aFile)
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

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(PreShockLevel) + " WHERE SystemDataFloat.[Index] = 114";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ShockNumberPerDirection) + " WHERE SystemDataFloat.[Index] = 110";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ShockDistance) + " WHERE SystemDataFloat.[Index] = 109";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(NumberOfWarnLimitViolations) + " WHERE SystemDataFloat.[Index] = 102";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(NumberOfAbortLimitViolations) + " WHERE SystemDataFloat.[Index] = 101";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestItems) + " WHERE SystemDataFloat.[Index] = 111";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(MassTestFixture) + " WHERE SystemDataFloat.[Index] = 115";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ConnectionWithSlipTable) + " WHERE SystemDataFloat.[Index] = 116";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(PreTestLevel) + " WHERE SystemDataFloat.[Index] = 100";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	//nastavenie Curve Save na presny pocet desatinnych miest
	ostringstream stringObj;
	stringObj << setprecision(9);
	stringObj << CurveSave;
	string strCurveSave = stringObj.str();

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + strCurveSave + " WHERE SystemDataFloat.[Index] = 113";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(CurveSaveShocks) + " WHERE SystemDataFloat.[Index] = 112";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(ShockInverted) + " WHERE SystemDataFloat.[Index] = 108";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Standard) + " WHERE SystemDataFloat.[Index] = 104";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(WarnLimit) + " WHERE SystemDataFloat.[Index] = 107";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	{string sql = "UPDATE User_Pre_PostPulses SET PrePositive = " + to_string(PosPreShock) + ", PreNegative = " + to_string(NegPreShock)
		+ ", PostPositive = " + to_string(PosPostShock) + ", PostNegative = " + to_string(NegPostShock)  
		+ " WHERE User_Pre_PostPulses.[CurveType] = 'frei definiert' ";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


//vytiahnutie nastaveni z testovacieho suboru Shock
/*vychadza z kniznice p. Novaka*/
CShocks& CShocks::FetchFrom_TestFileShock_ToShocks(string aFile)
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
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "114").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &PreShockLevel, sizeof(PreShockLevel), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "110").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &ShockNumberPerDirection, sizeof(ShockNumberPerDirection), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "109").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &ShockDistance, sizeof(ShockDistance), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "102").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &NumberOfWarnLimitViolations, sizeof(NumberOfWarnLimitViolations), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "101").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &NumberOfAbortLimitViolations, sizeof(NumberOfAbortLimitViolations), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "111").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestItems, sizeof(MassTestItems), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "115").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_DOUBLE, &MassTestFixture, sizeof(MassTestFixture), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "116").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &ConnectionWithSlipTable, sizeof(ConnectionWithSlipTable), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "100").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &PreTestLevel, sizeof(PreTestLevel), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "113").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_INTEGER, &CurveSave, sizeof(CurveSave), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "112").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &CurveSaveShocks, sizeof(CurveSaveShocks), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "108").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &ShockInverted, sizeof(ShockInverted), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "104").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &Standard, sizeof(Standard), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	SQLExecDirect(hstmt, (SQLCHAR*)(sql_select + "107").c_str(), SQL_NTS);
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_SLONG, &WarnLimit, sizeof(WarnLimit), NULL);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	string sql = "SELECT PrePositive,PreNegative,PostPositive,PostNegative FROM User_Pre_PostPulses WHERE User_Pre_PostPulses.[CurveType] = 'frei definiert' ";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
	while(SQLFetch(hstmt) == SQL_SUCCESS)
		{
		SQLGetData(hstmt, 1, SQL_C_SLONG, &PosPreShock, sizeof(PosPreShock), NULL);
		SQLGetData(hstmt, 2, SQL_C_SLONG, &NegPreShock, sizeof(NegPreShock), NULL);
		SQLGetData(hstmt, 3, SQL_C_SLONG, &PosPostShock, sizeof(PosPostShock), NULL);
		SQLGetData(hstmt, 4, SQL_C_SLONG, &PosPreShock, sizeof(PosPreShock), NULL);
		}
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);


	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}

//update zaznamu v tabulke test_parameters_shocks, podla toho ci sa hodnoty po teste zmenili
string CShocks::UpdateShocks(sqlite3* aDB, CShocks old)
	{
	if(PreShockLevel != old.PreShockLevel || ShockNumberPerDirection != old.ShockNumberPerDirection || ShockDistance != old.ShockDistance ||
	   NumberOfWarnLimitViolations != old.NumberOfWarnLimitViolations || NumberOfAbortLimitViolations != old.NumberOfAbortLimitViolations || 
	   PreTestLevel != old.PreTestLevel || ConnectionWithSlipTable != old.ConnectionWithSlipTable ||MassTestItems != old.MassTestItems || 
	   MassTestFixture != old.MassTestFixture || ConnectionWithSlipTable != old.ConnectionWithSlipTable || PreTestLevel != old.PreTestLevel || 
	   CurveSave != old.CurveSave || CurveSaveShocks != old.CurveSaveShocks || ShockInverted != old.ShockInverted || PosPreShock != old.PosPreShock || 
	   NegPreShock != old.NegPreShock || PosPostShock != old.PosPostShock || NegPostShock != old.NegPostShock || Standard != old.Standard || 
	   WarnLimit != old.WarnLimit)
		{

		//nastavenie Curve Save na presny pocet desatinnych miest
		ostringstream stringObj;
		stringObj << fixed << setprecision(9);
		stringObj << CurveSave;
		string strCurveSave = stringObj.str();

		string sql = "UPDATE test_parameters_shocks SET preShockLevel = " + to_string(PreShockLevel) + "," + " shockNumberPerDirection = "
			+ to_string(ShockNumberPerDirection) + "," + " shockDistance = " + to_string(ShockDistance) + "," + " numberOfWarnLimitViolations = "
			+ to_string(NumberOfWarnLimitViolations) + "," + " numberOfAbortLimitViolations = " + to_string(NumberOfAbortLimitViolations) + ","
			+ " massTestItems = " + to_string(MassTestItems) + "," + " massTestFixture = " + to_string(MassTestFixture)
			+ "," + " connectionWithSlipTable = " + to_string(ConnectionWithSlipTable) + "," + " pretestLevel = " + to_string(PreTestLevel) + ","
			+ " curveSave = " + to_string(CurveSave) + "," + " curveSaveShocks = " + to_string(CurveSaveShocks) + "," + " shockInverted = "
			+ to_string(ShockInverted) + "," + " posPreShock = " + to_string(PosPreShock) + "," + " negPostShock = "
			+ to_string(NegPostShock) + "," + " posPostShock = " + to_string(PosPostShock) + "," + " negPostShock = " + to_string(NegPostShock) + "," 
			+ " standard = " + to_string(Standard) + "," + " warnLimit = " + to_string(WarnLimit) + " WHERE testFileNumber = " + to_string(old.TestFileNumber) + " AND testFileYear = " + to_string(old.TestFileYear);

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);

		return "Zmena v test_parameters_shocks; ";
		}

	return "";
	}


/************************ DB tests_parameters ***********************/

		/************************ tabulka shocks_tolerance_band ***********************/


// implicitny konstruktor objektu CToleranceBand (vytvori 1 zaznam tabulky shocks_tolerance_band)
CToleranceBand::CToleranceBand(): adTestFileYear(0), adTestFileNumber(1), Time(10), AbortLimitPos(2), AbortLimitNeg(58) {}


// destruktor objektu CToleranceBand (zaznamu tabulky shocks_tolerance_band)
CToleranceBand::~CToleranceBand() {}


//tisk zaznamov ToleranceBand, ktore su ulozene vo vektore 
void CToleranceBand::PrintVectorToleranceBand(vector <CToleranceBand>& aVec)
	{

	for(size_t i = 0; i < aVec.size(); i++)
		{
		cout << "adTestFileYear: " << aVec[i].adTestFileYear << endl;
		cout << "adTestFileNumber: " << aVec[i].adTestFileNumber << endl;
		cout << "Time: " << aVec[i].Time << endl;
		cout << "AbortLimitPos: " << aVec[i].AbortLimitPos << endl;
		cout << "AbortLimitNeg: " << aVec[i].AbortLimitNeg << endl;
		cout << endl;
		}
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka shocks_tolerance_band
void CToleranceBand::InsertToToleranceBand(sqlite3* aDB, vector <CToleranceBand>& aVec)
	{
	for(size_t i = 0; i < aVec.size(); i++)
		{
		string sql = "INSERT INTO shocks_tolerance_band VALUES (" + to_string(aVec[i].adTestFileYear) + "," + to_string(aVec[i].adTestFileNumber) + ","
			+ to_string(aVec[i].Time) + "," + to_string(aVec[i].AbortLimitPos) + "," + to_string(aVec[i].AbortLimitNeg) + ");";

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackToleranceBand(void* aVector, int argc, char** argv, char** azColName)
	{
	vector <CToleranceBand>* tmpVector = (vector <CToleranceBand>*) aVector;
	CToleranceBand tmpClass;

	tmpClass.adTestFileYear = stoi(argv[0]);
	tmpClass.adTestFileNumber = stoi(argv[1]);
	tmpClass.Time = atof(argv[2]);
	tmpClass.AbortLimitPos = atof(argv[3]);
	tmpClass.AbortLimitNeg = atof(argv[4]);

	tmpVector->push_back(tmpClass);
	return 0;
	}


//ulozenie zaznamov z DB tests_parameters - tabulka shocks_tolerance_band, do rozhrania (do kontajneru s objektmi CToleranceBand)
//vyber podla TestNumber, vysledky su usporiadane vzostupne podla stlpca time 
vector <CToleranceBand> CToleranceBand::FetchFromToleranceBand(sqlite3* aDB, int aTestFileNumber, int aTestFileYear)
	{
	vector <CToleranceBand> tmp;

	string sql = "SELECT * FROM shocks_tolerance_band WHERE adTestFileNumber = " + to_string(aTestFileNumber) + " AND adTestFileYear = " + to_string(aTestFileYear) +  " ORDER BY time ASC";
	sqlite3_exec(aDB, sql.c_str(), callbackToleranceBand, &tmp, NULL);
	return tmp;
	}
	

//prevod dat z Tolerance Band do testovacieho suboru Shock
//pred insertovanim zaznamov sa vymazu zaznamy povodnej tabulky 
/*vychadza z kniznice p. Novaka*/
void CToleranceBand::InsertFrom_ToleranceBand_ToTestFileShock(string aFile, vector <CToleranceBand>& aVec)
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

	{string sql = "DELETE FROM UserToleranceBand";
	SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

	for(size_t i = 0; i < aVec.size(); i++)
		{	
			
		{string sql = "INSERT INTO UserToleranceband VALUES (5, 'frei definiert', 3, 'free', " + to_string(aVec[i].Time) + ", " 
			+ to_string(aVec[i].AbortLimitPos) + ", " + to_string(-aVec[i].AbortLimitNeg) + ", " + to_string(i + 1) + ")";
		SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
			
		}


	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}