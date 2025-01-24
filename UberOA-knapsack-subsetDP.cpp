// https : // docs.google.com/document/d/1GYwco4OuNB3RNbSFbw7Qn3XNAd1K8f0zGvehMkaN7GY/edit?tab=t.0
#include <bits/stdc++.h>
using namespace std;

/* clang-format off */

/* TYPES  */
#define ll long long
#define pii pair<int, int>
#define pll pair<long long, long long>
#define vi vector<int>
#define vll vector<long long>
#define mii map<int, int>
#define si set<int>
#define sc set<char>

/* FUNCTIONS */
#define f(i,s,e) for(long long int i=s;i<e;i++)
#define cf(i,s,e) for(long long int i=s;i<=e;i++)
#define rf(i,e,s) for(long long int i=e-1;i>=s;i--)
#define pb push_back
#define eb emplace_back

/* PRINTS */
template <class T>
void print_v(vector<T> &v) { cout << "{"; for (auto x : v) cout << x << ","; cout << "\b}"; }

/* UTILS */
#define MOD 1000000007
#define PI 3.1415926535897932384626433832795
#define read(type) readInt<type>()
ll min(ll a,int b) { if (a<b) return a; return b; }
ll min(int a,ll b) { if (a<b) return a; return b; }
ll max(ll a,int b) { if (a>b) return a; return b; }
ll max(int a,ll b) { if (a>b) return a; return b; }
ll gcd(ll a,ll b) { if (b==0) return a; return gcd(b, a%b); }
ll lcm(ll a,ll b) { return a/gcd(a,b)*b; }
string to_upper(string a) { for (int i=0;i<(int)a.size();++i) if (a[i]>='a' && a[i]<='z') a[i]-='a'-'A'; return a; }
string to_lower(string a) { for (int i=0;i<(int)a.size();++i) if (a[i]>='A' && a[i]<='Z') a[i]+='a'-'A'; return a; }
bool prime(ll a) { if (a==1) return 0; for (int i=2;i<=round(sqrt(a));++i) if (a%i==0) return 0; return 1; }
void yes() { cout<<"YES\n"; }
void no() { cout<<"NO\n"; }

/*  All Required define Pre-Processors and typedef Constants */
typedef long int int32;
typedef unsigned long int uint32;
typedef long long int int64;
typedef unsigned long long int  uint64;

/* clang-format on */

/* Main()  function */
int main()
{

    int tc;
    cin >> tc;

    while (tc--)
    {
        int n, p, q, r;
        cin >> n >> p >> q >> r;
        vi v(n);
        f(i, 0, n) cin >> v[i];

        vector<vector<vector<vector<int>>>> dp(n + 1, vector<vector<vector<int>>>(p + 1, vector<vector<int>>(q + 1, vector<int>(r + 1, -1e9))));

        dp[1][0][0][0] = v[0];
        dp[0][0][0][0] = 0;
        dp[1][1][0][0] = 0;

        for (int i = 1; i <= n; i++)
        {
            for (int j = 0; j <= p; j++)
            {
                for (int k = 0; k <= q; k++)
                {
                    for (int l = 0; l <= r; l++)
                    {
                        // delete first element
                        if (j - 1 >= 0)
                        {
                            dp[i][j][k][l] = max(dp[i][j][k][l], dp[i - 1][j - 1][k][l]);
                        }

                        // take first element
                        dp[i][j][k][l] = max(dp[i][j][k][l], v[i - 1] + dp[i - 1][j][k][l]);

                        // delete first two element
                        if (i - 2 >= 0 and k - 1 >= 0)
                        {
                            dp[i][j][k][l] = max(dp[i][j][k][l], dp[i - 2][j][k - 1][l]);
                        }

                        // delete first three element
                        if (i - 3 >= 0 and l - 1 >= 0)
                        {
                            dp[i][j][k][l] = max(dp[i][j][k][l], dp[i - 3][j][k][l - 1]);
                        }
                    }
                }
            }
        }

        cout << dp[n][p][q][r] << endl;
    }
    return 0;
}