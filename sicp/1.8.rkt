#lang scheme

(define (square x) (* x x))

(define (improve y x)
    (/ (+ (/ x (square y)) (* 2 y)) 3))

(define (good-enough? guess x)
    (= (improve guess x) guess))

(define (sqrt-iter guess x)
    (if (good-enough? guess x)
        guess
        (sqrt-iter (improve guess x) x)))

(define (sqrt x)
    (sqrt-iter 1.1 x))

(sqrt 5)