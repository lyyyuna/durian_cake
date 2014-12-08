-- total À©Õ¹µÝ¹éÊµÏÖ
total :: Num a => [a] -> a
total [] = 0
total (x:xs) = x + total xs

-- wei digui
total' :: Num a => [a] -> a -> a
total' [] n = n
total' (x:xs) n = total' xs x+n

-- fibonacci
fibonacci :: (Num a, Eq a) => a -> a
fibonacci 0 = 0
fibonacci 1 = 1
fibonacci n = fibonacci (n-1) + fibonacci (n-2)

fibstep :: Num a => (a,a) -> (a,a)
fibstep (u,v) = (v,u+v)

fibPair :: (Num a, Eq a) => a -> (a,a)
fibPair 0 = (0,1)
fibPair n = fibstep(fibPair(n-1))

fibPairFst :: (Num a, Eq a) => a -> a
fibPairFst n = fst $ fibPair n

--
fibonacci' :: (Num a, Eq a, Enum a) => a -> [a]
fibonacci' n = map fibPairFst [1..n]

--
fibonacci'' :: Int -> [Int]
fibonacci'' n = take n $ map fst $ iterate fibstep (0,1)


-- golden factor
golden :: Fractional a => Int -> [a]
golden n = take n $ map (\(x,y)  -> x/y) (iterate fibstep (0,1))


-- num to rome
romeNotation :: [String]
romeNotation = ["M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"]

romeAmount :: [Int]
romeAmount = [1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1]

pair :: [(Int, String)]
pair = zip romeAmount romeNotation

subtrahend :: Int -> (Int, String)
subtrahend n = head $ dropWhile (\(a, _) -> a > n) pair

convert :: Int -> (Int, String)
convert 0 = (0, "")
convert n = let (i,st) = subtrahend n in
            let (i',st') = convert(n-i) in (i', st ++ st')
			
-- fix point
fix :: (a->a) -> a
fix f = f (fix f)			

factorial :: Int -> Int
factorial = fix (\f n -> if (n==0) then 1 else n * f(n-1))

-- newton sqrt
squareroot :: Int -> Double -> Double
squareroot 0 x = x
squareroot n x = (squareroot (n-1) x + x / squareroot (n-1) x) / 2

fix' :: (t -> t -> Bool) -> (t -> t) -> t -> t
fix' c f x 
	| c x (f x) = x
	| otherwise = fix' c f (f x)
	
newton :: Fractional a => a -> a -> a	
newton c t = (c/t + t) / 2.0

mysqrt :: Double -> Double
mysqrt c = fix' (\a b -> a-b < 0.00000000000001) (newton c) c


-- lazyshorter
lazyshorter :: [a] -> [a] -> Bool
lazyshorter xs ys = short xs ys
	where 
		short [] ys = True
		short xs [] = False
		short (x:xs) (y:ys) = short xs ys