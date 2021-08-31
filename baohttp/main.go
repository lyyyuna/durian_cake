package main

import (
	"baohttp/bao"
	"fmt"
	"net/http"
)

func main() {
	r := bao.NewEngine()
	r.GET("/", func(rw http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(rw, "url.path = %v \n", r.URL.Path)
	})

	r.Run(":9999")
}
