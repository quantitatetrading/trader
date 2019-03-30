from IPython.display import display, Javascript

def graph(data):
    display(Javascript('graph.data = ' + str(data)))
