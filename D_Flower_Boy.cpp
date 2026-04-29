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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    int tc;
    cin >> tc;

    while (tc--)
    {
        int n, m;
        cin >> n >> m;
        vi a(n), b(m);

        for (int i = 0; i < n; i++)
            cin >> a[i];

        for (int i = 0; i < m; i++)
            cin >> b[i];

        vector<int> pre(m, -1), suf(m, -1);

        int ri = m - 1;
        int rj = n - 1;
        while (rj >= 0 and ri >= 0)
        {
            if (a[rj] >= b[ri])
            {
                suf[ri] = rj;
                ri--;
            }
            rj--;
        }

        int i = 0;
        int j = 0;
        while (j < n and i < m)
        {
            if (a[j] >= b[i])
            {
                pre[i] = j;
                i++;
            }
            j++;
        }
        if (m == 1)
        {
            if (i == m)
            {
                cout << 0 << endl;
            }
            else
            {
                cout << b[0] << endl;
            }
            continue;
        }

        if (i == m)
            cout << 0 << endl;
        else
        {
            int ans = INT_MAX;
            for (int k = 0; k < m; k++)
            {

                if (k == 0)
                {
                    int x = suf[k + 1];
                    if (x != -1)
                    {
                        ans = min(ans, b[k]);
                    }
                }
                else if (k == m - 1)
                {
                    int x = pre[k - 1];
                    if (x != -1)
                    {
                        ans = min(ans, b[k]);
                    }
                }
                else
                {
                    int x1 = suf[k + 1];
                    int x2 = pre[k - 1];
                    if (x1 != -1 and x2 != -1)
                    {
                        if (x1 > x2)
                        {
                            ans = min(ans, b[k]);
                        }
                    }
                }
            }

            if (ans == INT_MAX)
            {
                cout << -1 << endl;
                continue;
            }

            cout << ans << endl;
        }
    }
    return 0;
}