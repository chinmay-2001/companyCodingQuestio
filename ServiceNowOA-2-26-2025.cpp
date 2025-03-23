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

void traversal(vector<int> adj[], int node, vector<int> &subtree, vector<bool> &vis, vector<int> &parent, vector<int> &dp)
{
    for (auto v : adj[node])
    {
        if (vis[v] == false)
        {
            vis[v] = true;
            parent[v] = node;
            traversal(adj, v, subtree, vis, parent, dp);
        }
    }

    int maxi = 0, sum = 0, maxnode = -1;

    for (auto v : adj[node])
    {
        if (parent[node] != v)
        {
            sum += subtree[v];
            if (subtree[v] > maxi)
            {
                maxi = subtree[v];
                maxnode = v;
            }
            vis[v] = true;
            subtree[node] += subtree[v];
        }
    }

    if (sum == 0)
    {
        subtree[node] = 1;
        dp[node] = 0;
        return;
    }

    if (maxi <= sum / 2)
    {
        dp[node] = sum / 2;
    }
    else
    {
        int rem = sum - maxi;
        int leftOver = maxi - rem;
        int finalLeftOver = leftOver - 2 * dp[maxnode];

        if (finalLeftOver >= 0)
        {
            dp[node] = (sum - finalLeftOver) / 2;
        }
        else
        {
            dp[node] = sum / 2;
        }
    }
    subtree[node] += 1;
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
        vector<int> adj[n + 1];
        for (int i = 0; i < n - 1; i++)
        {
            int x, y;
            cin >> x >> y;
            adj[x].push_back(y);
            adj[y].push_back(x);
        }

        vector<int> subtree(n + 1, 0);
        vector<bool> vis(n + 1, false);
        vector<int> dp(n + 1, 0);
        vector<int> parent(n + 1, 0);
        vis[0] = true;
        traversal(adj, 0, subtree, vis, parent, dp);
        cout << dp[0] << endl;
    }
    return 0;
}