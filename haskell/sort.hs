insert :: Ord a => a -> [a] -> [a]
insert x [] = [x]
insert x (y:ys) 
        |  x < y = x:y:ys
        | otherwise = y : insert x ys

swaps :: Ord a => [a] -> [a]
swaps [] = []
swaps [x] = [x]
swaps (x1:x2:xs)
        | x1 > x2 = x2 : swaps(x1:xs)
        | otherwise = x1 : swaps(x2:xs)

bubbleSort :: Ord a => [a] -> [a]
bubbleSort xs
        | swaps xs == xs = xs       -- 没发生变化，就停止
        | otherwise = bubbleSort $ swaps xs

bubbleSort' :: Ord a=> [a] -> [a]
bubbleSort' [] = []
bubbleSort' xs = bubbleSort' initElem ++ [lastElem]
        where 
            swappedElem = swaps xs
            initElem = init swappedElem
            lastElem = last swappedElem

deleteFromOri :: Eq a => a -> [a] -> [a]
deleteFromOri _ [] = []
deleteFromOri x (y:ys)
        | x == y = ys
        | otherwise = y:deleteFromOri x ys

selectSort :: Ord a => [a] -> [a]
selectSort [] = []
selectSort xs = mini : selectSort xs'
        where 
            mini = minimum xs
            xs' = deleteFromOri mini xs


quickSort :: Ord a => [a] -> [a]
quickSort [] = []
quickSort [x:xs] = quickSort mini ++ [x] quickSort maxi
        where
            mini = filter (<x) xs
            maxi = filter (>=x) xs

merge :: Ord a => [a] -> [a] -> [a]
merge xs [] = xs
merge [] ys = ys
merge (x:xs) (y:ys)
        | x > y = y:merge (x:xs) ys
        | otherwise = x:merge xs (y:ys)

mergeSort :: Ord a => [a] -> [a]
mergeSort xs = merge (mergeSort x1) (mergeSort x2)
        where
            (x1, x2) = split xs
            split xs = (take mid xs, drop mid xs)
            mid = (length xs) `div` 2