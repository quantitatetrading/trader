import numpy as np
import quantitate

class tradingApp:
    def __init__(self, datasets):
        SQLStatement = self.createSQLStatement(datasets[0]['name'], datasets[0]['tickers'], datasets[0]['items'])

        self.data = quantitate.getData(SQLStatement, datasets[0]['items'], datasets[0]['tickers'])

    def createSQLStatement(self, dataset, tickers, items):

        if open not in items:
            items.insert(0, 'open') 

        # itemsFromTables = "[%s].timestamp, " % tickers[0]
        itemsFromTables = ""
        tables = ""
        timestamps = "" if len(tickers) == 1 else "WHERE "

        for i, ticker in enumerate(tickers):

            itemsFromTables += ", ".join(["[%s].[%s]" % (ticker, item) for item in items])
            tables += ticker + dataset + " AS [" + ticker + "]"
            if i != 0: timestamps += "[" + ticker + "].timestamp = [" + tickers[i-1] + "].timestamp"
            if i != len(tickers) - 1:
                itemsFromTables += ", "
                tables += ", "
                if i != 0: timestamps += " AND "
		

        SQLStatement = 'SELECT %s FROM %s %s;' % (itemsFromTables, tables, timestamps)

        return SQLStatement


    def backtest(self, algorithmClass):

        algorithm = algorithmClass()

        algorithm.init(self.data)

        while self.data.next():
            algorithm.algo(self.data, self.data.data())

        algorithm.end(self.data)

if __name__ == '__main__':
    datasets =[{
        'name': 'daily',
        'tickers': ['AAPL', 'AMZN'],#, 'A', 'MSFT', 'ORCL', 'BA', 'RTN', 'NOC', 'LMT'],
        'items': ['open','high','low','close','volume']
    }]
    
    class algorithm:

        def init(self, trader):
            trader.buy("AAPL", 0.2)
            trader.buy("AMZN", 0.8)

        def algo(self, trader, data):
            pass

        def end(self, trader):
            print(trader.roi())

    trader = tradingApp(datasets)
    trader.backtest(algorithm)
