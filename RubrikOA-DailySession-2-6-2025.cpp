// https://docs.google.com/document/d/1mFVTsyw4iUFrYuakleaqmiNK7Lysu7LCEL8D86qS-V4/edit?tab=t.0

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


const int MAXN = 100000;
ll fact[MAXN + 1], ifact[MAXN + 1];

// Function to compute (base^exp) % MOD using Binary Exponentiation
ll power(ll base, ll exp, ll mod) {
    ll result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

void precompute() {
    fact[0] = ifact[0] = 1;
    for (int i = 1; i <= MAXN; i++) {
        fact[i] = (fact[i - 1] * i) % MOD;
    }
    ifact[MAXN] = power(fact[MAXN], MOD - 2, MOD); // Fermat's theorem
    for (int i = MAXN - 1; i >= 1; i--) {
        ifact[i] = (ifact[i + 1] * (i + 1)) % MOD;
    }
}

ll nCr(int n, int r) {
    if (r > n || r < 0) return 0;
    return (fact[n] * ifact[r] % MOD) * ifact[n - r] % MOD;
}

/* clang-format on */
// R
int bfs(int node, vector<int> adj[], vector<int> &vis)
{
    queue<int> que;
    que.push(node);
    vis[node] = 1;
    int cnt = 0;
    while (!que.empty())
    {
        int v = que.front();
        que.pop();
        cnt++;
        for (auto p : adj[v])
        {
            if (vis[p] == 0)
            {
                que.push(p);
                vis[p] = 1;
            }
            else
            {
                // return 0;//

                // real version  return k
                return -1;
            }
        }
    }
    return cnt;
}
/* Main()  function */

int main()
{
    int tc;
    cin >> tc;
    precompute();
    while (tc--)
    {
        int n, k;
        cin >> n >> k;

        vi d(n);
        f(i, 0, n) cin >> d[i];

        vi adj[n + 1];

        for (int i = 1; i <= n; i++)
        {
            if (d[i - 1] != -1)
            {
                adj[i].push_back(d[i - 1]);
            }
        }

        vi vis(n + 1, 0);
        int ans = 1;
        for (int i = 1; i <= n; i++)
        {
            if (vis[i] == 0)
            {
                int count = bfs(i, adj, vis);
                // cout << "count:" << count << endl;
                if (count == -1)
                {
                    ans *= k;
                }
                else
                {
                    // ans = (ans * nCr(k, count));
                    ans = (ans * nCr(k + count - 1, count - 1));
                }
            }
        }
        cout << ans << endl;
    }
    return 0;
}