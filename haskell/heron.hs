-- 海伦公式
s :: Double -> Double -> Double -> Double
s a b c = let p = (a+b+c)/2 in sqrt (p*(p-a)*(p-b)*(p-c))

-- 判断一个数是否是2
isTwo :: Int -> Bool
isTwo x = if x == 2 then True else False

-- invalid month
month :: Int -> Int
month n = case n of
    1 -> 31
    2 -> 28
    3 -> 43
    _ -> error "invalid month"
    
-- 取绝对值
abs' :: (Num a, Ord a) => a -> a
abs' n 
    | n > 0 = n
    | otherwise = -n