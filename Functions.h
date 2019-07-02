#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#ifndef UNICODE
#define UNICODE
#endif

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>
#include "sqlite3.h"
#include "windows.h"

// Called from getNumRows. stores number of rows in count
static int callback(void * count, int argc, char** argv, char** azColName)
{
	int * c = static_cast<int*>(count);
	*c = atoi(argv[0]);
	return 0;
}

// Checks for valid ID
bool validID(std::wstring str)
{
	bool valid = true;
	size_t len = str.length();
	if (len != 8)
		return false;
	int i = 0;
	while (i < len && valid)
	{
		if (str[i] < 48 || str[i] > 57)
			valid = false;
		i++;
	}
	return valid;
}

// ------------INVALID---------------
//bool checkForUser(std::wstring& str)
//{
//	std::wifstream in_file;
//	in_file.open("Users.txt");
//
//	bool found = false;
//	std::wstring line;
//	while (getline(in_file, line) && !found)
//	{
//		std::wstring temp = line.substr(0, 8);
//		if (temp == str)
//		{
//			str = line;
//			found = true;
//		}
//	}
//	return found;
//}

// Check if user exists within db
bool checkForUser(std::wstring& id, sqlite3* db)
{
	bool found = false;
	if (!sqlite3_open16(L"Users.db", &db))
	{
		sqlite3_stmt* statement;
		char* query = "select * from users";
		if (!sqlite3_prepare_v2(db, query, -1, &statement, NULL))
		{
			int res = 0;
			bool done = false;
			while (!done && !found)
			{
				res = sqlite3_step(statement);
				if (res == SQLITE_ROW)
				{
					std::wstring c = static_cast<const wchar_t*>(sqlite3_column_text16(statement, 0));
					if (found = id == c)
					{
						for (int i = 1; i < 4; i++)
						{
							const wchar_t *c = static_cast<const wchar_t*>(sqlite3_column_text16(statement, i));
							id += L" ";
							id += c;
						}
					}
				}
				if (res == SQLITE_DONE || res == SQLITE_ERROR)
					done = true;
			}
		}
	}
	sqlite3_close(db);
	return found;
}

// Write user info to login.txt with timestamp
void timeStamp(std::wstring& user)
{
	std::time_t now = std::time(0);
	char realNow[50];
	ctime_s(realNow, 50, &now);

	std::wofstream outFile;
	outFile.open("Logins.txt", std::wfstream::app);
	outFile << std::endl << realNow << user << "\n-------------------------------------------------------------------------";
	outFile.close();
}

// OLD FUNCTION - FOR TESTING PURPOSES ONLY
//void addUserToRegistry(const std::wstring& user)
//{
//	std::wofstream inFile;
//	inFile.open("Users.txt", std::fstream::app);
//
//	inFile << user << std::endl;
//}

// Add user to Users.db
void addUserToRegistry(const std::wstring& id, const std::wstring& lName,
	const std::wstring& fName, const std::wstring& branch, sqlite3* db)
{
	if (!sqlite3_open16(L"Users.db", &db))
	{
		sqlite3_stmt * stmt;
		std::wstring str = L"INSERT INTO users VALUES ('" + id + L"', '" + lName + L",', '" + fName + L"', '" + branch + L"');";
		int i = sqlite3_prepare16_v2(db, &str[0], -1, &stmt, NULL);
		if (i == SQLITE_OK)
		{
			sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);
}

// Returns number of lines for viewing logins
int getNumLines(const std::string& file)
{
	std::ifstream in;
	in.open(file + ".txt");
	int count = 0;
	std::string line;
	while (getline(in, line))
	{
		count++;
	}
	return count;
}

// gives number of rows for view users
int getNumRows(sqlite3* db)
{
	int numRows = 0;
	char* szErrMsg = 0;
	if (!sqlite3_open16(L"Users.db", &db))
		sqlite3_exec(db, "select count(*) from users", callback, &numRows, &szErrMsg);
	sqlite3_close(db);
	return numRows;
}

// Gets rows, stores in wstring array
void getRows(sqlite3* db, std::wstring str[])
{
	if (!sqlite3_open16(L"Users.db", &db))
	{
		sqlite3_stmt* stmt;
		if (!sqlite3_prepare16_v2(db, L"select * from users", -1, &stmt, NULL))
		{
			bool done = false;
			int i = 0;
			int res;
			while (!done)
			{
				res = sqlite3_step(stmt);
				if (res == SQLITE_ROW)
				{
					for (int j = 0; j < 4; j++)
					{
						const wchar_t* c = static_cast<const wchar_t*>(sqlite3_column_text16(stmt, j));
						str[i] += c;
						str[i] += L" ";
					}
				}
				if (res == SQLITE_DONE || res == SQLITE_ERROR)
					done = true;
				i++;
			}
		}
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);

}

// Stores the db to .txt file (for printing NOT FINISHED)
bool tableToTxt(sqlite3* db)
{
	if (!sqlite3_open16(L"Users.db", &db))
	{
		sqlite3_stmt* stmt;
		if (!sqlite3_prepare16_v2(db, L"Select * from users", -1, &stmt, NULL))
		{
			bool done = false;
			int res;
			std::wstring str;
			while (!done)
			{
				std::wofstream ofs;
				ofs.open(L"temp.txt", std::wofstream::out | std::wfstream::app);
				res = sqlite3_step(stmt);
				if (res == SQLITE_ROW)
				{
					for (int i = 0; i < 4; i++)
					{
						str += static_cast<const wchar_t*>(sqlite3_column_text16(stmt, i));
					}
					ofs << str << std::endl;
				}
				if (res == SQLITE_DONE || res == SQLITE_ERROR)
					done = true;
			}
		}
		sqlite3_finalize(stmt);
		return true;
	}
	sqlite3_close(db);
	return false;
}

#endif
