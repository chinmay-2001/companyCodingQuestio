#include <bits/stdc++.h>
using namespace std;

#define ll long long

int countSetBits(ll x)
{
    int count = 0;
    while (x)
    {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int t;
    cin >> t;
    while (t--)
    {
        ll n, x;
        cin >> n >> x;

        if (n == 1)
        {
            cout << (x == 0 ? -1 : x) << "\n";
        }
        else if (x == 0)
        {
            cout << (n % 2 == 0 ? n : n + 3) << "\n";
        }
        else
        {
            int u = countSetBits(x);
            if (u >= 2 || n % 2 == 1)
            {
                if (n - u < 0)
                {
                    cout << x << "\n";
                }
                else
                {
                    if ((n - u) % 2 == 0)
                    {
                        cout << n - u + x << "\n";
                    }
                    else
                    {
                        cout << n - u + 1 + x << "\n";
                    }
                }
            }
            else
            {

                // u==1-> 2^0,2^1,........ and n is even; //RRRRR
                if (x > 1)
                {
                    cout << (n - 2 + x + 1 + 1) << "\n";
                }
                else
                {
                    cout << (n - 2 + 5) << "\n";
                }
            }
        }
    }
    return 0;
}


import java.util.*;

public class Main {
    static int solve(int N, int M, int K, int L, int R, String S, String specials) {
        Set<Character> special = new HashSet<>();
        for (char c : specials.toCharArray())
            special.add(c);

        int[] prefix = new int[N + 1];
        for (int i = 1; i <= N; i++) {
            prefix[i] = prefix[i - 1] + (special.contains(S.charAt(i - 1)) ? 1 : 0);
        }

        int result = 0;
        int low = 1, high = 1;

        for (int start = 1; start <= N; start++) {
            // Move low to the first position where count >= L
            while (low <= N && prefix[low] - prefix[start - 1] < L) low++;
            // Move high to the last position where count <= R
            while (high <= N && prefix[high] - prefix[start - 1] <= R) high++;

            // Add all valid end positions for this start
            if (low <= N)
                result += (high - low);

            // If start moves ahead, ensure low and high move too
            if (low < start) low = start;
            if (high < start) high = start;
        }

        return result;
    }

    public static void main(String[] args) {
        int N = 5;
        String S = "abcde";
        String specials = "bd";
        int L = 1, R = 2;

        System.out.println(solve(N, 0, 0, L, R, S, specials));  // Example output
    }
}

why this work