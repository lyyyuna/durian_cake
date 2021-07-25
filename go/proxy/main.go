package main

import "net/http"

func main() {
	p := NewProxyServer()
	http.ListenAndServe(":8080", p)
}
