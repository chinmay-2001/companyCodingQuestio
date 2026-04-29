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

vector<ll> maxdepth;
vector<ll> nodeAtDepth;
/* clang-format on */
int dfs(int node, vector<bool> &vis, vector<int> adj[], int lv)
{
    int maxi = lv;

    for (auto v : adj[node])
    {
        if (vis[v] == false)
        {
            vis[v] = true;
            maxi = max(dfs(v, vis, adj, lv + 1), maxi);
        }
    }
    nodeAtDepth[lv] += 1;
    maxdepth[maxi] += 1;
    return maxi;
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
        vector<int> arr[n + 1];
        nodeAtDepth.clear();
        maxdepth.clear();
        maxdepth.resize(n + 1, 0);
        nodeAtDepth.resize(n + 1, 0);

        for (int i = 0; i < n - 1; i++)
        {
            int v, u;
            cin >> u >> v;
            arr[u].push_back(v);
            arr[v].push_back(u);
        }
        vector<bool> vis(n + 1, 0);
        vis[1] = 1;
        int depth = dfs(1, vis, arr, 1);

        f(i, 1, depth + 1)
        {
            nodeAtDepth[i] += nodeAtDepth[i - 1];
            maxdepth[i] += maxdepth[i - 1];
        }

        int ans = INT_MAX;
        for (int i = 1; i <= depth; i++)
        {
            ll sum = maxdepth[i - 1] + (nodeAtDepth[depth] - nodeAtDepth[i]);
            ans = min(sum, ans);
        }
        cout << ans << endl;
    }
    return 0;
}