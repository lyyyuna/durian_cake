class Cheese():
    def __init__(self, kind):
        self.kind = kind

    def __repr__(self) -> str:
        return 'Cheese(%r)' % self.kind


import weakref

stock = weakref.WeakValueDictionary()
catalog = [Cheese('red'), Cheese('green'), Cheese('blue'), Cheese('yellow')]

for cheese in catalog:
    stock[cheese.kind] = cheese

print(sorted(stock.keys()))

del catalog

print(sorted(stock.keys()))

del cheese

print(sorted(stock.keys()))