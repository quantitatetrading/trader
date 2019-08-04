FROM ubuntu:18.04
USER root

EXPOSE 1433

#install dependencies

RUN apt-get update \
    && apt-get install -y --allow-unauthenticated python3 python3-pip sudo wget curl software-properties-common\
    && curl https://packages.microsoft.com/config/ubuntu/18.04/prod.list > /etc/apt/sources.list.d/mssql-release.list \
    && wget -qO- https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add - \
    && apt-get update --allow-unauthenticated \
    && ACCEPT_EULA=Y apt-get install -y msodbcsql17 mssql-tools odbcinst1debian2 unixodbc unixodbc-dev \
    && pip3 install pybind11 numpy


WORKDIR /app
COPY . /app

#add DSN
ENV ODBCdriver /opt/microsoft/msodbcsql17/lib64/$(ls /opt/microsoft/msodbcsql17/lib64/)
RUN echo '[mssql]\
\nDescription = Test for Microsoft SQL Server 2017 using ODBC 2013 driver\
\nDriver = /opt/microsoft/msodbcsql17/lib64/'$(ls /opt/microsoft/msodbcsql17/lib64)'\
\nDatabase = stockData \
\nServer = quantitatesql.database.windows.net \
\nPort = 1433 \
' >> /etc/odbc.ini

#compile

RUN g++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` c/pyGetData.cpp -lodbc -fpermissive -o quantitate`python3-config --extension-suffix`

CMD ["python3", "algo.py"]