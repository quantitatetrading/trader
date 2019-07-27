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

class DB {
    public:
        SQLHENV henv;  
        SQLHDBC hdbc;  
        SQLHSTMT hstmt;
        SQLRETURN retcode;

        SQLCHAR * OutConnStr = (SQLCHAR * )malloc(255);  
        SQLSMALLINT * OutConnStrLen = (SQLSMALLINT *)malloc(255);  
    
    SQLHSTMT connect(SQLCHAR* query){

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
                                disconnect();
                            } else {
                                return hstmt;
                            }
                        }
                    }
                }
            }
        }
    }

    void disconnect(){
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        SQLDisconnect(hdbc);  
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);  
    }

};

class getData {
    public:

        #define dataLen 20

        SQLHSTMT hstmt;
        SQLRETURN retcode;

    	map<string, SQLREAL> dataMap;
        map<string, float> priceMap;
        map<string, float> positionMap;
        map<string, SQLLEN> ptrMap;

        float roi = 1.;

        vector<string> items;
        vector<string> tickers;

    getData(string sQuery, vector<string> userItems, vector<string> userTickers){

        tickers = userTickers;
        items = userItems;

        char* query = sQuery.c_str();

        hstmt = DB().connect(query);

        bindColumns();
        SQLFetch(hstmt);
        updatePrices();

    }

    void bindColumns(){

        string column;
        string ticker;

        int itemsLen = items.size();

        for(int i = 0; i < tickers.size(); i++){

            ticker = tickers.at(i);

            positionMap[ticker] = 0;

            for(int j = 0; j < itemsLen; j++){
                
                column = ticker + '.' + items.at(j);
                SQLBindCol(hstmt, i*itemsLen+j+1, SQL_C_FLOAT, (SQLPOINTER)&dataMap[column], dataLen, ptrMap[column]);

            }
        }
    }

    void updatePrices(){

        string ticker;

        for(int i = 0; i < tickers.size(); i++){

            ticker = tickers.at(i);

            priceMap[ticker] = dataMap[ticker + ".open"];
        }
    }

    bool next(){


        retcode = SQLFetch(hstmt);
        
        roi = roi * increase();

        updatePrices();

        if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
            return true;
        } else {
            DB().disconnect();
            return false;
        }

    }


    float increase(){

        float total = 1;
        float increase;
        float totalReturn = 1;
        string ticker;

        for(int i = 0; i < tickers.size(); i++){
            ticker = tickers.at(i);

            increase = dataMap[ticker + ".open"]/priceMap[ticker] - 1;
            total += positionMap[ticker] * increase;
            totalReturn += positionMap[ticker] * increase;
        }

        
        for(int i = 0; i < tickers.size(); i++){
            ticker = tickers.at(i);
            positionMap[ticker] = positionMap[ticker] * (dataMap[ticker + ".open"] / priceMap[ticker]) / total;
        }

        return totalReturn;
    }

    map<string, float> data(){
	    return dataMap;
    }

    void buy(string ticker, float position){
        positionMap[ticker] = position;
    }

    float returnOnInvestment(){
        return roi;
    }

    map<string, float> prices(){
        return priceMap;
    }

    map<string, float> positions(){
        return positionMap;
    }
};

PYBIND11_MODULE(quantitate, m){
    py::class_<getData>(m, "getData")
        .def(py::init<string &, vector<string>, vector<string>>())
        .def("next", &getData::next)
        .def("data", &getData::data)
        .def("roi", &getData::returnOnInvestment)
        .def("prices", &getData::prices)
        .def("buy", &getData::buy)
        .def("positions", &getData::positions);
}
