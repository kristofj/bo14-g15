/*************************************************************************
 *  Compilation:  javac Hanoi.java
 *  Execution:    java Hanoi N
 *  
 *  Solves the Towers of Hanoi problem on N discs. The discs are labeled
 *  in increasing order of size from 1 to N and the poles are labeled
 *  A, B, and C.
 *
 *************************************************************************/

public class Hanoi {

    // move n smallest discs from one pole to another, using the temp pole
    public static void hanoi(int n, String from, String temp, String to) {
        if (n == 0) return;
        hanoi(n-1, from, to, temp);
        System.out.println("Move disc " + n + " from " + from + " to " + to);
        hanoi(n-1, temp, from, to);
    }

    public static void main(String[] args) {
        int N = Integer.parseInt(args[0]);
        hanoi(N, "A", "B", "C");
    }
}

