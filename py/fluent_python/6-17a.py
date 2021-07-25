import functools

def clock(func):
    @functools.wraps(func)
    def clocked():
        pass
    return clocked


@clock
def factorial():
    '''hello, factorial'''
    pass


print(factorial.__name__, factorial.__doc__)