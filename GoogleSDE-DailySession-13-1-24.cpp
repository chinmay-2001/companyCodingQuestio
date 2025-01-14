// https://docs.google.com/document/d/1919-CRP6pDb8XO0mdMP8VBwZSvYZJnJo3QEzc88eU58/edit?tab=t.0
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
        int n;
        cin >> n;
        vi v(n + 1, 0);
        f(i, 1, n + 1) cin >> v[i];
        vector<int> best(n + 1, 1);
        best[0] = 1;
        for (int i = 2; i <= n; i++)
        {
            if (v[i - 1] < v[i])
            {
                best[i] = best[i - 1] + 1;
            }
            else
            {
                best[i] = 1;
            }
        }
        // f(i, 1, n + 1) cout << best[i] << " ";
        // cout << endl;
        int ans = 1;

        // if it is allowed to have atmost one to decrease
        vector<int> bestChange(n + 1, 1);
        for (int i = 2; i <= n; i++)
        {
            int l = best[i];
            if (i - l - 1 < 0)
            {
                bestChange[i] = best[i];
            }
            else if (v[i - l - 1] + 1 < v[i - l + 1])
            {
                bestChange[i] = best[i - l - 1] + best[i] + 1;
            }
            else
            {
                bestChange[i] = best[i] + 1;
            }
        }

        for (int i = 1; i <= n; i++)
        {
            ans = max(bestChange[i], ans);
        }

        cout << ans << endl;
    }
    return 0;
}