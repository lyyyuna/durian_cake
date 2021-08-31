package main

import (
	"baohttp/bao"
	"net/http"
)

func main() {
	r := bao.NewEngine()

	r.GET("/", func(c *bao.Context) {
		c.HTML(http.StatusOK, "hello")
	})

	r.GET("/hello", func(c *bao.Context) {
		c.JSON(http.StatusOK, bao.H{
			"1": "a",
			"2": "b",
		})
	})

	r.Run(":9999")
}
