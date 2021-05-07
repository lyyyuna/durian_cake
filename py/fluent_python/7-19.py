import functools

@functools.lru_cache()
def factorial(n):
    if n < 2:
        return n
    return factorial(n-1) + factorial(n-2)


print(factorial(100))