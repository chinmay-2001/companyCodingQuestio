#include <bits/stdc++.h>
using namespace std;
int n;
long long memo[20][11][2];
string num;

long long digitDp(int pos, int last, int tight)
{
    if (pos == n)
    {
        if (last == 10)
            return 0;
        else
            return 1;
    }

    if (!tight && memo[pos][last][tight] != -1)
    {
        return memo[pos][last][tight];
    }

    int limit = tight ? (num[pos] - '0') : 9;

    long long result = 0;

    for (int x = 0; x <= limit; x++)
    {
        int new_tight = tight && (x == limit);

        if (last == 10 and x == 0)
        {
            result += digitDp(pos + 1, 10, new_tight);
        }
        else if (last != x)
        {
            result += digitDp(pos + 1, x, new_tight);
        }
    }
    if (!tight)
        memo[pos][last][0] = result;
    return result;
}

long long count(long long x)
{

    string a = to_string(x);
    int len = a.length();
    // cout<<len<<endl;
    n = len;
    memset(memo, -1, sizeof(memo));
    num = a;
    return digitDp(0, 10, 1);
}

int main()
{
    long long l, r;
    cin >> l >> r;

    if (l == 0 && r == 0)
    {
        cout << 1 << endl;
    }
    else
    {

        long long a1 = count(l - 1);
        long long a2 = count(r);

        // cout<<"a1:"<<a1<<endl;
        // cout<<"a2:"<<a2<<endl;

        cout << a2 - a1 << endl;
    }
}
