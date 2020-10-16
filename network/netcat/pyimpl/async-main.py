import socket
import argparse
import select
import os
import sys
import asyncio
import concurrent

parser = argparse.ArgumentParser(description='netcat')
parser.add_argument('--server', default=False, action='store_true')
parser.add_argument('--ip', default='127.0.0.1', type=str)
parser.add_argument('--port', default=5555, type=int)

args = parser.parse_args()


async def print_to_tty(reader: asyncio.StreamReader):
    while True:
        data = await reader.read(1024)
        if data:
            os.write(sys.stdout.fileno(), data)
        else:
            # cannot stop another coroutine
            # so just exit -_-
            sys.exit()


def blocking_io():
    return os.read(sys.stdin.fileno(), 1024)

        
async def read_from_tty(writer: asyncio.StreamWriter):
    loop = asyncio.get_running_loop()
    while True:
        with concurrent.futures.ThreadPoolExecutor() as pool:
            data = await loop.run_in_executor(
                pool, blocking_io)
            if data:
                writer.write(data)
                await writer.drain()
            else:
                writer.close()
                return


async def handle_netcat_reqeust(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
    await asyncio.gather(
        print_to_tty(reader),
        read_from_tty(writer),
    )


async def main():
    if args.server == True:
        server = await asyncio.start_server(handle_netcat_reqeust, args.ip, args.port)
        async with server:
            await server.serve_forever()
    else:
        reader, writer = await asyncio.open_connection(args.ip, args.port)
        await asyncio.gather(
            print_to_tty(reader),
            read_from_tty(writer),
        )


asyncio.run(main())