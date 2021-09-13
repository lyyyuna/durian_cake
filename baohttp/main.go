package main

import (
	"baohttp/bao"
	"fmt"
	"log"
	"net/http"
	"time"
)

func onlyForV2() bao.HandlerFunc {
	return func(c *bao.Context) {
		t := time.Now()
		log.Printf("one middle ware: %v", t)
	}
}

func main() {
	r := bao.NewEngine()

	r.GET("/index", func(c *bao.Context) {
		c.HTML(http.StatusOK, "hello")
	})

	r.GET("/panic", func(c *bao.Context) {
		d := []string{}
		fmt.Println(d[1])
	})

	v1 := r.Group("/v1")
	v1.Use(onlyForV2())
	{
		v1.GET("/", func(c *bao.Context) {
			c.HTML(http.StatusOK, "v1")
		})

		v1.GET("/hello", func(c *bao.Context) {
			c.String(http.StatusOK, "hello %v, you are at %v\n", c.Query("name"), c.Path)
		})
	}

	v2 := r.Group("/v2")
	{
		v2.GET("/hello/:name", func(c *bao.Context) {
			c.String(http.StatusOK, "hello %s, you're at %s\n", c.Param("name"), c.Path)
		})
		v2.POST("/login", func(c *bao.Context) {
			c.JSON(http.StatusOK, bao.H{
				"username": c.PostForm("username"),
				"password": c.PostForm("password"),
			})
		})
	}

	r.Run(":9999")
}
