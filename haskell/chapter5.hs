-- total À©Õ¹µÝ¹éÊµÏÖ
total :: Num a => [a] -> a
total [] = 0
total (x:xs) = x + total xs

-- 
total' :: Num a => [a] -> a -> a
total' [] n = n
total' (x:xs) n = total'

-- fibonacci
fibonacci :: (Num a) => a -> a
fibonacci 0 = 0
fibonacci 1 = 1
fibonacci n = fibonacci (n-1) + fibonacci (n-2)