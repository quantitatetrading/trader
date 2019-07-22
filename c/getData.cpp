// g++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` c/pyGetData.cpp -lodbc -fpermissive -o /home/bwp/miniconda3/lib/python3.7/site-packages/quantitate`python3-config --extension-suffix`

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
#include <new>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
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

    getData(string sQuery, vector<string> columns){

        char* query = sQuery.c_str();

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
                                cout << "Error querying SQL Server" << endl;
                                completed();
                            } else {
                                bindColumns(columns);
                            }
                        }
                    }
                }
            }
        }
    }

    void bindColumns(vector<string> columns){

        for(int i = 0; i < columns.size(); i++){
            SQLBindCol(hstmt, i+1, SQL_C_CHAR, dataMap[columns.at(i)], dataLen, ptrMap[columns.at(i)]);
        }
    }

    bool next(){

        retcode = SQLFetch(hstmt);
        return (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) ? true : false;
    }

    double data(string item){
	    return atof(dataMap[item]);
    }

    void completed(){
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        SQLDisconnect(hdbc);  
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);  
        cout << "done" << endl;
    }
};

PYBIND11_MODULE(quantitate, m){
    py::class_<getData>(m, "getData")
        .def(py::init<string &, vector<string>>())
        .def("next", &getData::next)
        .def("data", &getData::data);
}