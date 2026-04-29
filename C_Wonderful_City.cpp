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
ll INF=1e17;

/* clang-format on */

ll solve(int n, vector<vector<int>> vec, vector<int> c)
{
    vector<bool> v1(n, false), v2(n, false), v3(n, false);

    for (int i = 1; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (vec[i][j] + 1 == vec[i - 1][j])
                v1[i] = true;

            if (vec[i][j] == vec[i - 1][j])
                v2[i] = true;

            if (vec[i][j] == vec[i - 1][j] + 1)
                v3[i] = true;
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (v1[i] and v2[i] and v3[i])
            return INF;
    }

    vll dp1(n), dp2(n);
    dp1[0] = 0;
    dp2[0] = c[0];

    for (int i = 1; i < n; i++)
    {
        if (v1[i] and v2[i])
        {
            dp1[i] = dp2[i - 1];
            dp2[i] = INF;
        }
        else if (v2[i] and v3[i])
        {
            dp1[i] = INF;
            dp2[i] = c[i] + dp1[i - 1];
        }
        else if (v1[i] and v3[i])
        {
            dp1[i] = dp1[i - 1];
            dp2[i] = c[i] + dp2[i - 1];
        }
        else if (v1[i])
        {
            dp1[i] = min(dp1[i - 1], dp2[i - 1]);
            dp2[i] = c[i] + dp2[i - 1];
        }
        else if (v2[i])
        {
            dp1[i] = dp2[i - 1];
            dp2[i] = dp1[i - 1] + c[i];
        }
        else if (v3[i])
        {
            dp1[i] = dp1[i - 1];
            dp2[i] = min(dp1[i - 1], dp2[i - 1]) + c[i];
        }
        else
        {
            dp1[i] = min(dp1[i - 1], dp2[i - 1]);
            dp2[i] = min(dp1[i - 1], dp2[i - 1]) + c[i];
        }
    }
    return min(dp1[n - 1], dp2[n - 1]);
}

/* Main()  function */
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    int tc;
    cin >> tc;

    while (tc--)
    {
        int n;
        cin >> n;
        vector<vector<int>> row(n, vector<int>(n, 0));

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cin >> row[i][j];
            }
        }

        vector<int> cr(n), cc(n);
        for (int i = 0; i < n; i++)
            cin >> cr[i];

        for (int i = 0; i < n; i++)
            cin >> cc[i];

        ll ans = 0;
        ans += solve(n, row, cr);

        vector<vector<int>> col(n, vector<int>(n, 0));

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                col[i][j] = row[j][i];
            }
        }
        if (ans >= INF)
        {
            cout << -1 << endl;
            continue;
        }

        ans += solve(n, col, cc);

        if (ans >= INF)
        {
            cout << -1 << endl;
            continue;
        }

        cout << ans << endl;
    }
    return 0;
}