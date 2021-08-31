package bao

import (
	"fmt"
	"log"
	"net/http"
)

type Engine struct {
	router map[string]http.HandlerFunc
}

func NewEngine() *Engine {
	return &Engine{
		router: make(map[string]http.HandlerFunc),
	}
}

func (e *Engine) addRoute(method string, pattern string, handler http.HandlerFunc) {
	key := method + "-" + pattern
	log.Printf("route add: %v - %v", method, pattern)
	e.router[key] = handler
}

func (e *Engine) GET(pattern string, handler http.HandlerFunc) {
	e.addRoute("GET", pattern, handler)
}

func (e *Engine) POST(pattern string, handler http.HandlerFunc) {
	e.addRoute("POST", pattern, handler)
}

func (e *Engine) Run(addr string) error {
	return http.ListenAndServe(addr, e)
}

func (e *Engine) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	key := req.Method + "-" + req.URL.Path

	if handler, ok := e.router[key]; ok {
		handler(w, req)
	} else {
		fmt.Fprintf(w, "404: %v", req.URL)
	}
}
