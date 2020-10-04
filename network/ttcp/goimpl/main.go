package main

import (
	"encoding/binary"
	"flag"
	"log"
	"net"
	"strings"
)

var (
	server bool
	ipport string
	number int32
	length int32
)

func init() {
	flag.BoolVar(&server, "s", false, "")
	flag.StringVar(&ipport, "h", "127.0.0.1:5555", "")
	flag.Parse()

	number = 1024
	length = 8096
}

func main() {
	if server == true {
		log.Println(server, ipport)
		ttcpServer(ipport)
	} else {
		log.Println(server, ipport)
		ttcpClient(ipport)
	}
}

func ttcpServer(ipport string) {
	l, err := net.Listen("tcp", ipport)
	if err != nil {
		log.Fatal(err)
	}
	defer l.Close()

	for {
		conn, err := l.Accept()
		if err != nil {
			log.Fatal(err)
		}
		go func(c net.Conn) {
			defer c.Close()
			var sessionLen int32
			var sessionNum int32
			binary.Read(c, binary.BigEndian, &sessionNum)
			binary.Read(c, binary.BigEndian, &sessionLen)

			log.Println("session number ", sessionNum)
			log.Println("session length ", sessionLen)

			recvBuf := make([]byte, sessionLen)
			var recvLen int32
			for i := 0; i < int(sessionNum); i++ {
				binary.Read(c, binary.BigEndian, &recvLen)
				if recvLen != sessionLen {
					log.Fatal("recv length not compatible with session length", recvLen)
				}
				binary.Read(c, binary.BigEndian, recvBuf)
				binary.Write(c, binary.BigEndian, &sessionLen)
			}
			log.Println("session close")
		}(conn)
	}

}

func ttcpClient(ipport string) {
	conn, err := net.Dial("tcp", ipport)
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	log.Println("session, ", number, length)
	binary.Write(conn, binary.BigEndian, &number)
	binary.Write(conn, binary.BigEndian, &length)

	numMap := strings.Split("0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F", ",")
	output := make([]byte, length)
	for i := 0; i < int(length); i++ {
		output[i] = []byte(numMap[i%16])[0]
	}
	binary.Write(conn, binary.BigEndian, &length)
	binary.Write(conn, binary.BigEndian, output)

	ttcpClientRecv(conn, output, length)

	log.Println("total bytes ", number*length)
}

func ttcpClientRecv(conn net.Conn, output []byte, length int32) {
	var count int32
	count = 1
	var recvLength int32

	binary.Read(conn, binary.BigEndian, &recvLength)
	if recvLength != length {
		log.Fatal("length no compatible")
	}

	if count < number {
		binary.Write(conn, binary.BigEndian, &length)
		binary.Write(conn, binary.BigEndian, output)
		count++
	} else {
		return
	}
}
