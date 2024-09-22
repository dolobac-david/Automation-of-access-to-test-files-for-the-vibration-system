#include "snimace.h"
#include <iostream>

/************************ DB tests_parameters ***********************/

		/************************ tabulka vibrations_common ***********************/


// implicitny konstruktor objektu CVibrationsCommon (vytvori 1 zaznam tabulky vibrations_common)
CVibrationsCommon::CVibrationsCommon(): TestFileYear(0), TestFileNumber(0)
	{
	for(size_t i = 0; i < 8; i++)
		{
		TypeOfInput[i] = 0;
		RelSensorID[i] = 0;
		Sensitivity[i] = 0;
		Unit[i] = 0;
		LowerLimit[i] = 0;
		UpperLimit[i] = 0;
		InfoText.push_back("Channel " + to_string(i + 1));
		Notching[i] = 0;
		}
	}


// destruktor objektu CVibrationsCommon (zaznamu tabulky vibrations_common)
CVibrationsCommon::~CVibrationsCommon() {}


//tisk zaznamov CVibrationsCommon, ktore su ulozene v rozhrani, vo vektore 
void CVibrationsCommon::PrintVibrationsCommon()
	{
	cout << "TestFileYear: " << TestFileYear << endl;
	cout << "TestFileNumber: " << TestFileNumber << endl;

	for(size_t j = 0; j < 8; j++)
		{
		cout << "Channel " + to_string(j + 1) + " TypeOfInput: " << TypeOfInput[j] << endl;

		if(TypeOfInput[j] != 0)
			{
			cout << "Channel " + to_string(j + 1) + " RelSensorID: " << RelSensorID[j] << endl;
			cout << "Channel " + to_string(j + 1) + " Sensitivity: " << Sensitivity[j] << endl;
			cout << "Channel " + to_string(j + 1) + " Unit: " << Unit[j] << endl;
			cout << "Channel " + to_string(j + 1) + " LowerLimit: " << LowerLimit[j] << endl;
			cout << "Channel " + to_string(j + 1) + " UpperLimit: " << UpperLimit[j] << endl;
			cout << "InfoText: " << InfoText[j] << endl;
			cout << "Channel " + to_string(j + 1) + " Notching: " << Notching[j] << endl;
			}

		cout << endl;
		}
	}


//vyplnenie zaznamu v DB tests_parameters - tabulka vibrations_common
void CVibrationsCommon::InsertToVibrationsCommon(sqlite3* aDB)
	{
	string sql = "INSERT INTO vibrations_common (testFileYear,testFileNumber) VALUES (" + to_string(TestFileYear) + ", " 
		+ to_string(TestFileNumber) + ");";

	sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);

	for(size_t j = 0; j < 8; j++)
		{

		string sql = "UPDATE vibrations_common SET ch" + to_string(j + 1) + "typeOfInput = " + to_string(TypeOfInput[j]) 
			+ " WHERE testFileNumber = " + to_string(TestFileNumber) + " AND testFileYear = " + to_string(TestFileYear);

		sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);

		if(TypeOfInput[j] != 0)
			{
			string strRelSensorID = to_string(RelSensorID[j]);
			string strSensitivity = to_string(Sensitivity[j]);
			string strUnit = to_string(Unit[j]);
			string strLowerLimit = to_string(LowerLimit[j]);
			string strUpperLimit = to_string(UpperLimit[j]);
			string strInfoText = InfoText[j];
			string strNotching = to_string(Notching[j]);

			string sql = "UPDATE vibrations_common SET relCh" + to_string(j + 1) + "sensorId = " + strRelSensorID + ", ch" + to_string(j + 1)
				+ "sensitivity = " + strSensitivity + ", ch" + to_string(j + 1) + "unit = " + strUnit + ", ch" + to_string(j + 1) + "lowerLimit = "
				+ strLowerLimit + ", ch" + to_string(j + 1) + "upperLimit = " + strUpperLimit + ", ch" + to_string(j + 1) + "infoText = '"
				+ strInfoText + "', ch" + to_string(j + 1) + "notching = " + strNotching + " WHERE testFileNumber = " + to_string(TestFileNumber) + " AND testFileYear = " + to_string(TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			}
		}
	}


// funkcia dolezita pre dotaz SELECT
int callbackVibrationsCommon(void* aClass, int argc, char** argv, char** azColName)
	{
	CVibrationsCommon* tmp = (CVibrationsCommon*)aClass;

	tmp->TestFileYear = stoi(argv[0]);
	tmp->TestFileNumber = stoi(argv[1]);

	for(size_t i = 0; i < 8; i++)
		{
		tmp->TypeOfInput[i] = stoi(argv[i + 2 + 0]);

		if(tmp->TypeOfInput[i] != 0)
			{
			tmp->RelSensorID[i] = stoi(argv[i + 2 + 8]);
			tmp->Sensitivity[i] = atof(argv[i + 2 + 16]);
			tmp->Unit[i] = stoi(argv[i + 2 + 24]);
			tmp->LowerLimit[i] = atof(argv[i + 2 + 32]);
			tmp->UpperLimit[i] = atof(argv[i + 2 + 40]);
			tmp->InfoText[i] = argv[i + 2 + 48];
			tmp->Notching[i] = stoi(argv[i + 2 + 56]);
			}
		}

	return 0;
	}


//ulozenie zaznamov z DB tests_parameters - tabulka vibrations_common, do rozhrania (do kontajneru s objektmi CVibrationsCommon)
// vybera sa podla TestNumber
CVibrationsCommon& CVibrationsCommon::FetchFromVibrationsCommon(sqlite3* aDB, int aTestNumber, int aTestYear)
	{
	string sql = "SELECT * FROM vibrations_common WHERE testFileNumber = " + to_string(aTestNumber) + " AND testFileYear = " + to_string(aTestYear);
	sqlite3_exec(aDB, sql.c_str(), callbackVibrationsCommon, this, NULL);
	return *this;
	}


//prevod dat z triedy CVibrationsCommmon do testovacieho suboru Random 
void CVibrationsCommon::InsertFrom_VibrationsCommon_ToTestFileRandom(string aFile)
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

	//vyplnenie 1 sady nastaveni do testovacieho suboru Random
	for(size_t i = 0; i < 8; i++)
		{
		//Type of Input sa zada pre kazdy snimac
				{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(TypeOfInput[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(28 - i);
				SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

				// dalsie nastavenia snimacov sa zadavaju iba ak snimac nema Type of Input nastavene na 0 (off)
				if(TypeOfInput[i] != 0)
					{
							{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Sensitivity[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(36 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

							//podla type of input sa vyplnaju UpperLimit a LowerLimit na ine miesta v testovacom subore Random
							//type of input == control or measure
							if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(44 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(60 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							//type of input == abort 
							if(TypeOfInput[i] == 1)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(76 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(84 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							string sql = "UPDATE SystemDataText SET SystemDataText = '" + InfoText[i] + "' WHERE SystemDataText.[Index] = " + to_string(16 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
					}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


//vytiahnutie nastaveni snimacov z testovacieho suboru Random
CVibrationsCommon& CVibrationsCommon::FetchFrom_TestFileRandom_ToVibrationsCommon(string aFile)
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

	for(size_t i = 0; i < 8; i++)
		{

		{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(28 - i);
		SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
		SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
		SQLFetch(hstmt);
		SQLGetData(hstmt, 1, SQL_C_SLONG, &TypeOfInput[i], sizeof(TypeOfInput[i]), NULL);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);}

		// dalsie nastavenia snimacov sa vytiahnu iba ak snimac nema Type of Input nastavene na 0 (off)
		if(TypeOfInput[i] != 0)
			{

			{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(36 - i);
			SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
			SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
			SQLFetch(hstmt);
			SQLGetData(hstmt, 1, SQL_C_DOUBLE, &Sensitivity[i], sizeof(Sensitivity[i]), NULL);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }


			//podla type of input sa vytiahnu UpperLimit a LowerLimit z inych miest v testovacom subore Random
			//type of input == control or measure
			if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
				{

				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(44 - i);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(60 - i);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

				}

			//type of input == abort 
			if(TypeOfInput[i] == 1)
				{
				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(76 - i);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); 
				UpperLimit[i] = UpperLimit[i] / 1000; }

				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(84 - i);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }
				LowerLimit[i] = LowerLimit[i] / 1000;}

			{string sql_select = "SELECT SystemDataText FROM SystemDataText WHERE SystemDataText.[Index] = " + to_string(16 - i);
			char c_strInfoText[256] = "hello";
			SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
			SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
			SQLFetch(hstmt);
			SQLGetData(hstmt, 1, SQL_C_DEFAULT, &c_strInfoText, 255, NULL);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			InfoText[i] = c_strInfoText;
			}
			
			}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}


//prevod dat z triedy CVibrationsCommmon do testovacieho suboru Sine 
void CVibrationsCommon::InsertFrom_VibrationsCommon_ToTestFileSine(string aFile)
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

	//vyplnenie 1 sady nastaveni do testovacieho suboru Sine
	for(size_t i = 0; i < 8; i++)
		{
		//Type of Input sa zada pre kazdy snimac
				{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(TypeOfInput[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(28 - i);
				SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

				// dalsie nastavenia snimacov sa zadavaju iba ak snimac nema Type of Input nastavene na 0 (off)
				if(TypeOfInput[i] != 0)
					{
							{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Sensitivity[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(36 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

							{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Unit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(44 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

							//podla type of input sa vyplnaju UpperLimit a LowerLimit na ine miesta v testovacom subore Sine
							//type of input == control or measure
							if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(52 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(60 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							//type of input == abort 
							if(TypeOfInput[i] == 1)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(68 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(76 - i);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							string sql = "UPDATE SystemDataText SET SystemDataText = '" + InfoText[i] + "' WHERE SystemDataText.[Index] = " + to_string(22 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
					}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


//vytiahnutie nastaveni snimacov z testovacieho suboru Sine
CVibrationsCommon& CVibrationsCommon::FetchFrom_TestFileSine_ToVibrationsCommon(string aFile)
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

	for(size_t i = 0; i < 8; i++)
		{

				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(28 - i);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_SLONG, &TypeOfInput[i], sizeof(TypeOfInput[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

				// dalsie nastavenia snimacov sa vytiahnu iba ak snimac nema Type of Input nastavene na 0 (off)
				if(TypeOfInput[i] != 0)
					{

							{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(36 - i);
							SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
							SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
							SQLFetch(hstmt);
							SQLGetData(hstmt, 1, SQL_C_DOUBLE, &Sensitivity[i], sizeof(Sensitivity[i]), NULL);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

							{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(44 - i);
							SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
							SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
							SQLFetch(hstmt);
							SQLGetData(hstmt, 1, SQL_C_SLONG, &Unit[i], sizeof(Unit[i]), NULL);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

							//podla type of input sa vytiahnu UpperLimit a LowerLimit z inych miest v testovacom subore Random
							//type of input == control or measure
							if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
								{

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(52 - i);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(60 - i);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

								}

							//type of input == abort 
							if(TypeOfInput[i] == 1)
								{
										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(68 - i);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
										UpperLimit[i] = UpperLimit[i] / 1000; }

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(76 - i);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }
										LowerLimit[i] = LowerLimit[i] / 1000;
								}

							{string sql_select = "SELECT SystemDataText FROM SystemDataText WHERE SystemDataText.[Index] = " + to_string(22 - i);
							char c_strInfoText[256] = "hello";
							SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
							SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
							SQLFetch(hstmt);
							SQLGetData(hstmt, 1, SQL_C_DEFAULT, &c_strInfoText, 255, NULL);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
							InfoText[i] = c_strInfoText;
							}

					}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}


//prevod dat z triedy CVibrationsCommmon do testovacieho suboru Shock 
void CVibrationsCommon::InsertFrom_VibrationsCommon_ToTestFileShock(string aFile)
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

	//vyplnenie 1 sady nastaveni do testovacieho suboru Shock
	for(size_t i = 0; i < 8; i++)
		{
		//Type of Input sa zada pre kazdy snimac
				{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(TypeOfInput[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(85 - i * 9);
				SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

				// dalsie nastavenia snimacov sa zadavaju iba ak snimac nema Type of Input nastavene na 0 (off)
				if(TypeOfInput[i] != 0)
					{
							{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(Sensitivity[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(86 - i * 9);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }


							//podla type of input sa vyplnaju UpperLimit a LowerLimit na ine miesta v testovacom subore Shock a s inym prepoctom 
							//type of input == control or measure
							if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(90 - i * 9);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i]) + " WHERE SystemDataFloat.[Index] = " + to_string(89 - i * 9);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							//type of input == abort 
							if(TypeOfInput[i] == 1)
								{
										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(UpperLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(88 - i * 9);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }

										{string sql = "UPDATE SystemDataFloat SET SystemDataFloat = " + to_string(LowerLimit[i] * 1000) + " WHERE SystemDataFloat.[Index] = " + to_string(87 - i * 9);
										SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS); }
								}

							string sql = "UPDATE SystemDataText SET SystemDataText = '" + InfoText[i] + "' WHERE SystemDataText.[Index] = " + to_string(22 - i);
							SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
					}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}


//vytiahnutie nastaveni snimacov z testovacieho suboru Shock
CVibrationsCommon& CVibrationsCommon::FetchFrom_TestFileShock_ToVibrationsCommon(string aFile)
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

	for(size_t i = 0; i < 8; i++)
		{

				{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(85 - i * 9);
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
				SQLFetch(hstmt);
				SQLGetData(hstmt, 1, SQL_C_SLONG, &TypeOfInput[i], sizeof(TypeOfInput[i]), NULL);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

				// dalsie nastavenia snimacov sa vytiahnu iba ak snimac nema Type of Input nastavene na 0 (off)
				if(TypeOfInput[i] != 0)
					{

							{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(86 - i * 9);
							SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
							SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
							SQLFetch(hstmt);
							SQLGetData(hstmt, 1, SQL_C_DOUBLE, &Sensitivity[i], sizeof(Sensitivity[i]), NULL);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }


							//podla type of input sa vytiahnu UpperLimit a LowerLimit z inych miest v testovacom subore Random
							//type of input == control or measure
							if(TypeOfInput[i] == 2 || TypeOfInput[i] == 3)
								{

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(90 - i * 9);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(89 - i * 9);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }

								}

							//type of input == abort 
							if(TypeOfInput[i] == 1)
								{
										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(88 - i * 9);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &UpperLimit[i], sizeof(UpperLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
										UpperLimit[i] = UpperLimit[i] / 1000; }

										{string sql_select = "SELECT SystemDataFloat FROM SystemDataFloat WHERE SystemDataFloat.[Index] = " + to_string(87 - i * 9);
										SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
										SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
										SQLFetch(hstmt);
										SQLGetData(hstmt, 1, SQL_C_DOUBLE, &LowerLimit[i], sizeof(LowerLimit[i]), NULL);
										SQLFreeHandle(SQL_HANDLE_STMT, hstmt); }
										LowerLimit[i] = LowerLimit[i] / 1000;
								}

							{string sql_select = "SELECT SystemDataText FROM SystemDataText WHERE SystemDataText.[Index] = " + to_string(22 - i);
							char c_strInfoText[256] = "hello";
							SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
							SQLExecDirect(hstmt, (SQLCHAR*)sql_select.c_str(), SQL_NTS);
							SQLFetch(hstmt);
							SQLGetData(hstmt, 1, SQL_C_DEFAULT, &c_strInfoText, 255, NULL);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
							InfoText[i] = c_strInfoText;
							}

					}
		}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return *this;
	}


//update tabulky snimacov vibrations_common po zmene nastaveni po vykonanom teste 
//sinus == true ak sa jedna o Sine testovaci subor
string CVibrationsCommon::UpdateVibrationsCommon(sqlite3* aDB, CVibrationsCommon oldRecord, bool sinus)
	{
	bool isChanged = 0;
	for(size_t i = 0; i < 8; i++)
		{	
		if(TypeOfInput[i] != oldRecord.TypeOfInput[i])
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "typeOfInput = " + to_string(TypeOfInput[i])
				+ " WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}

		if(Sensitivity[i] != oldRecord.Sensitivity[i])
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "sensitivity = " + to_string(Sensitivity[i])
				+ " WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}

		if(sinus && (Unit[i] != oldRecord.Unit[i]))
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "unit = " + to_string(Unit[i])
				+ " WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}

		if(LowerLimit[i] != oldRecord.LowerLimit[i])
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "lowerLimit = " + to_string(LowerLimit[i])
				+ " WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}

		if(UpperLimit[i] != oldRecord.UpperLimit[i])
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "upperLimit = " + to_string(UpperLimit[i])
				+ " WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}

		if(InfoText[i] != oldRecord.InfoText[i])
			{
			string sql = "UPDATE vibrations_common SET ch" + to_string(i + 1) + "infoText = '" + InfoText[i]
				+ "' WHERE testFileNumber = " + to_string(oldRecord.TestFileNumber) + " AND testFileYear = " + to_string(oldRecord.TestFileYear);

			sqlite3_exec(aDB, sql.c_str(), NULL, 0, NULL);
			isChanged = 1;
			}
		}

	if(isChanged)
		return "Zmena v tabulce snimacu; ";

	else
		return "";
	}