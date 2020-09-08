# trader

A backtesting application built to work with both Quantitate's own datasets as well as your own.

## Getting Started

This program was built to work with Ubuntu 18.04

Install the ODBC Driver 17 by executing the following commands

```
sudo su
curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add -
curl https://packages.microsoft.com/config/ubuntu/18.04/prod.list > /etc/apt/sources.list.d/mssql-release.list
#replace 18.04 with your Ubuntu version
exit

sudo apt-get update
sudo ACCEPT_EULA=Y apt-get install -y msodbcsql17 mssql-tools odbcinst1debian2 unixodbc unixodbc-dev
```

Then add the Quantitate database DSN by adding the following to `/etc/odbc.ini`

```
[mssql]
Description = "Test for Microsoft SQL Server 2017 using ODBC 2013 driver"
Driver = /opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.3.so.1.1
Database = stockData
Server = quantitatesql.database.windows.net
Port = 1433
```
Make sure the driver filepath is correct, be sure to change the version to the one installed

Test the database connection by running `isql mssql QuantitateAdmin SomeRandomPassword1`

Then install pybind11 by performing `pip install pybind11`

Now you can compile the c++ scripts by executing 
```
g++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` c/getData.cpp -lodbc -fpermissive -o quantitate`python3-config --extension-suffix`
```
You can now test the installation by running the python script: `python algo.py`

