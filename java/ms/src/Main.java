import java.util.Scanner;
import java.io.*;
import java.util.*;
import java.math.BigInteger;


public class Main {
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int T = in.nextInt();
        System.out.println(T);
        BigInteger sum = new BigInteger("0");
        int index = 0;
        while(in.hasNext()) {	
        	index++;
        	BigInteger N = in.nextBigInteger();
            for (BigInteger A = new BigInteger("1"); A.compareTo(N) != 0; A.add(BigInteger.ONE))
                for (BigInteger B = new BigInteger("1"); B.compareTo(N) != 0; B.add(BigInteger.ONE))
                    for (BigInteger C = new BigInteger("1"); C.compareTo(N) != 0; C.add(BigInteger.ONE)) {      
                    	if (isSame(A, B, C) == 0)
                    		continue;
                    	BigInteger tmp = new BigInteger("0");
                    	BigInteger tmp1 = N.add(BigInteger.ONE).subtract(A);
                    	BigInteger tmp2 = N.add(BigInteger.ONE).subtract(B);
                    	BigInteger tmp3 = N.add(BigInteger.ONE).subtract(C);
                    	tmp = tmp1.multiply(tmp2).multiply(tmp3);
                    	sum = sum.add(tmp);
                    	sum = sum.mod(new BigInteger("1000000000000000000"));
                    }
            
            System.out.print("Case ");
            System.out.print(index);
            System.out.print(": ");
            System.out.println(sum);
        }
    }
    
    public static int isSame(BigInteger a, BigInteger b, BigInteger c)
    {
        if (a.compareTo(b) == 0 && b.compareTo(c) == 0)
            return 1;
        if (a.compareTo(b) == 0)
            return 0;
        if (b.compareTo(c) == 0)
            return 0;
        if (b.compareTo(a) == 0)
            return 0;

        return 1;
    }
}
