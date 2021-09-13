package bao

import "log"

func Recovery() HandlerFunc {
	return func(c *Context) {
		defer func() {
			if err := recover(); err != nil {
				log.Println(err)
			}
		}()

		c.Next()
	}
}
