import asyncio
import argparse
import struct

parser = argparse.ArgumentParser(description="ttcp")
parser.add_argument('--number', default=5, type=int)
parser.add_argument('--length', default=4096, type=int)
parser.add_argument('--server', default=False, action='store_true')
parser.add_argument('--ip', default='127.0.0.1', type=str)
parser.add_argument('--port', default=5555, type=int)

args = parser.parse_args()


SessionMessage = struct.Struct(">ii")
LengthMessage = struct.Struct(">i")


async def handle_ttcp_request(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
    session: bytes = await reader.readexactly(SessionMessage.size)
    session_number, session_length = SessionMessage.unpack(session)

    print("session number", session_number)
    print("session length", session_length)

    for i in range(session_number):
        length_byte: bytes = await reader.readexactly(LengthMessage.size)
        length, = LengthMessage.unpack(length_byte)
        # print(length)
        if length != session_length:
            print('length not compatible', length)
        await reader.readexactly(length)
        writer.write(LengthMessage.pack(length))
        await writer.drain()


async def send_request():
    reader, writer = await asyncio.open_connection(args.ip, args.port)
    writer.write(SessionMessage.pack(args.number, args.length))
    await writer.drain()

    payload = bytearray((b"0123456789ABCDEF"[i % 16] for i in range(args.length)))
    for i in range(args.number):
        # print(args.length)
        writer.write(LengthMessage.pack(args.length))
        writer.write(payload)
        await writer.drain()
        await reader.readexactly(LengthMessage.size)


async def main():
    if args.server == True:
        server = await asyncio.start_server(handle_ttcp_request, args.ip, args.port)
        async with server:
            await server.serve_forever()
    else:
        await send_request()


asyncio.run(main())