from time import sleep, strftime
from concurrent import futures


def display(*args):
    print(strftime('[%H:%M:%S]'), end=' ')
    print(*args)


def loiter(n):
    msg = '{}loiter({}): doing nothing for {}s...' 
    display(msg.format('\t'*n, n, n))
    sleep(n)
    msg = '{}loiter({}): done.' 
    display(msg.format('\t'*n, n))
    return n * 10


def main():
    display('script starting...')
    executor = futures.ThreadPoolExecutor(max_workers=3)
    results = executor.map(loiter, range(5))

    display(results)
    for i, res in enumerate(results):
        display('result {} {}'.format(i, res))


main()