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
        vi a(n);
        vi b(n);
        vi c(n);
        f(i, 0, n) cin >> a[i];
        f(i, 0, n) cin >> b[i];
        f(i, 0, n) cin >> c[i];

        vector<int> dpa(n + 1);
        vector<int> dpb(n + 1);
        vector<int> dpc(n + 1);

        dpa[1] = a[0];
        dpb[1] = b[0];
        dpc[1] = c[0];
        dpa[2] = a[1] + max(dpa[1], dpb[1], dpc[1]);
        dpb[2] = b[1] + max(dpa[1], dpb[1], dpc[1]);
        dpc[2] = c[1] + max(dpa[1], dpb[1], dpc[1]);

        for (int i = 3; i < n; i++)
        {
            dpa[i] = max(a[i - 1] + max(dpb[i - 1], dpc[i - 1]), a[i - 1] + a[i - 1], max(dpb[i - 2], dpc[i - 2]));
            dpb[i] = max(b[i - 1] + max(dpa[i - 1], dpc[i - 1]), b[i - 1] + b[i - 1], max(dpa[i - 2], dpc[i - 2]));
            dpc[i] = max(c[i - 1] + max(dpb[i - 1], dpa[i - 1]), c[i - 1] + c[i - 1], max(dpa[i - 2], dpb[i - 2]));
        }
        cout << max(dpa[n], max(dpb[n], dpc[n])) << endl;
    }
    return 0;
}