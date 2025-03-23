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

void gdfs(vector<int> a[], vector<int> &vis, int node, int c)
{
    // cout << "h" << endl;
    vis[node] = c;
    for (int v : a[node])
    {
        if (vis[v] == 0)
        {
            gdfs(a, vis, v, c);
        }
    }
}

int fdfs(vector<int> a[], vector<int> &va, vector<int> &fb, int node, int c)
{
    va[node] = c;
    int res = 0;
    for (int v : a[node])
    {
        if (va[v] == 0)
        {
            if (fb[v] != c)
                res++;
            else
                res += fdfs(a, va, fb, v, c);
        }
    }
    return res;
}

/* Main()  function */
int main()
{
    int tc;
    cin >> tc;

    while (tc--)
    {
        int n, m1, m2;
        cin >> n >> m1 >> m2;
        vector<int> a1[n];
        vector<int> a2[n];

        for (int i = 0; i < m1; i++)
        {
            int u, v;
            cin >> u >> v;
            a1[--u].push_back(--v);
            a1[v].push_back(u);
        }

        for (int i = 0; i < m2; i++)
        {
            int u, v;
            cin >> u >> v;
            a2[--u].push_back(--v);
            a2[v].push_back(u);
        }

        vector<int> va(n + 1, 0);
        vector<int> vb(n + 1, 0);
        int ans = 0;
        for (int i = 0; i < n; i++)
        {
            if (vb[i] == 0)
            {
                gdfs(a2, vb, i, i + 1);
            }

            if (va[i] == 0)
            {
                ans += fdfs(a1, va, vb, i, vb[i]);
                if (i + 1 > vb[i])
                    ans++;
            }
        }
        cout << ans << endl;
    }
    return 0;
}