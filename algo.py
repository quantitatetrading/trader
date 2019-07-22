import numpy as np
import quantitate

class tradingApp:
    def __init__(self, datasets):
        SQLStatement, columns = self.createSQLStatement(datasets[0]['name'], datasets[0]['tickers'], datasets[0]['items'])

        print(SQLStatement, columns)

        self.data = quantitate.getData(SQLStatement, columns)

    def createSQLStatement(self, dataset, tickers, items):

        itemsFromTables = "[%s].timestamp, " % tickers[0]
        tables = ""
        timestamps = ""

        self.abbreviationToItem = {
            'O': 'open',
            'H': 'high',
            'L': 'low',
            'C': 'close',
            'V': 'volume'
        }

	

        for i, ticker in enumerate(tickers):

            itemsFromTables += ", ".join(["[%s].[%s]" % (ticker, self.abbreviationToItem[item]) for item in items])
            tables += ticker + dataset + " AS [" + ticker + "]"
            if i != 0: timestamps += "[" + ticker + "].timestamp = [" + tickers[i-1] + "].timestamp"
            if i != len(tickers) - 1:
                itemsFromTables += ", "
                tables += ", "
                if i != 0: timestamps += " AND "
		

        SQLStatement = 'SELECT %s FROM %s WHERE %s;' % (itemsFromTables, tables, timestamps)

        return SQLStatement, itemsFromTables.replace('[','').replace(']', '').split(', ')


    def backtest(self, algorithm):
        while self.data.next():
            algorithm(self.data.data)

if __name__ == '__main__':
    datasets = [
    {
        'name': 'daily',
        'tickers': ['AAPL', 'AMZN'],#] 'A', 'MSFT', 'ORCL', 'BA', 'RTN', 'NOC', 'LMT'],
        'items': 'OHCLV'
    }
    ]

    def algorithm(data):
        print(data("AAPL.high"))

    trader = tradingApp(datasets)
    trader.backtest(algorithm)
