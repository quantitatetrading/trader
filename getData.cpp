// g++ -g getData.cpp -fpermissive -lodbc -o getData  && ./getData
#include <iostream>
#include <string>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <bits/stdc++.h>
#include <iterator> 

using namespace std;

class getData {
    public:
        #define dataLen 20

        SQLHENV henv;  
        SQLHDBC hdbc;  
        SQLHSTMT hstmt;
        SQLRETURN retcode;

        SQLCHAR * OutConnStr = (SQLCHAR * )malloc(255);  
        SQLSMALLINT * OutConnStrLen = (SQLSMALLINT *)malloc(255);  

    	map<string, SQLCHAR[dataLen]> dataMap;
        map<string, SQLLEN> ptrMap;

    getData(SQLCHAR* query, string columns[], int columnsLen){

        retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);  
  
        // Set the ODBC version environment attribute
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  
            retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);   
        
            // Allocate connection handle  
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  
                retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);   
        
                // Set login timeout to 5 seconds  
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  
                    SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);  
        
                    // Connect to data source  
                    retcode = SQLConnect(hdbc, (SQLCHAR*) "mssql", SQL_NTS, (SQLCHAR*) "QuantitateAdmin", 15, (SQLCHAR*) "SomeRandomPassword1", 19);  
        
                    // Allocate statement handle  
                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  

                        cout << "Success!" << endl;

                        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

                        // Process data  
                        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  

                            if (SQL_SUCCESS != SQLExecDirect(hstmt, query, SQL_NTS)) {
                                cout << "Error querying SQL Server";
                                completed();
                            } else {
                                bindColumns(columns, columnsLen);
                            }
                        }
                    }
                }
            }
        }
    }

    void bindColumns(string columns[], int columnsLen){

        for(int i = 0; i < columnsLen; i++){
            SQLBindCol(hstmt, i+1, SQL_C_CHAR, dataMap[columns[i]], dataLen, ptrMap[columns[i]]);
        }
    }

    bool next(){

        retcode = SQLFetch(hstmt);

        return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) ? true : false;
    }

    SQLCHAR* data(string item){
	    return dataMap[item];
    }

    void completed(){
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        SQLDisconnect(hdbc);  
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);  
        cout << "done" << endl;
    }



};

int main(){

    SQLCHAR* query = "SELECT [AAPL].timestamp, [AAPL].[open], [AAPL].[high], [AAPL].[close], [AAPL].[low], [AAPL].[volume], [AMZN].[open], [AMZN].[high], [AMZN].[close], [AMZN].[low], [AMZN].[volume] FROM AAPLdaily AS [AAPL], AMZNdaily AS [AMZN] WHERE [AMZN].timestamp = [AAPL].timestamp;";
    string columns[] = {"AAPL.timestamp", "AAPL.open", "AAPL.high", "AAPL.close", "AAPL.low", "AAPL.volume", "AMZN.open", "AMZN.high", "AMZN.close", "AMZN.low", "AMZN.volume"};
    int columnsLen = 11;

    getData test(query, columns, 11);

    while(test.next()){
        cout << test.data("AMZN.high") << endl;
    }

    test.completed();

}
