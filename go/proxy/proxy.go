package main

import (
	"io"
	"net"
	"net/http"
	"time"
)

type ProxyServer struct {
	tr *http.Transport
}

func NewProxyServer() *ProxyServer {
	return &ProxyServer{
		tr: &http.Transport{},
	}
}

func (p *ProxyServer) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	if r.Method == "CONNECT" {
		host := r.Host
		connToRemote, err := net.DialTimeout("tcp", host, 5*time.Second)
		if err != nil {
			panic(err)
		}

		hi, _ := w.(http.Hijacker)
		connFromClient, _, _ := hi.Hijack()

		connFromClient.Write([]byte("HTTP/1.1 200 Connection Established\r\n\r\n"))

		go io.Copy(connFromClient, connToRemote)
		go io.Copy(connToRemote, connFromClient)

		return
	}

	res, err := p.tr.RoundTrip(r)
	if err != nil {
		panic(err)
	}

	for k, vs := range res.Header {
		for _, v := range vs {
			w.Header().Set(k, v)
		}
	}

	w.WriteHeader(res.StatusCode)

	io.Copy(w, res.Body)
}
