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
int possible(vector<int> &a, int k, int mid)
{
    int n = a.size() - 1;
    int sum = 0;
    for (int i = 2; i <= n; i++)
    {
        int d = abs(a[i] - a[i - 1]);
        if (d / mid == 0)
        {
            sum += ((d / mid) - 1);
        }
        else
        {
            sum += (d / mid);
        }

        if (sum > k)
            return false;
    }
    return true;
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
        int n, k;
        cin >> n >> k;

        vector<int> a(n + 1);
        for (int i = 1; i <= n; i++)
        {
            cin >> a[i];
        }

        vector<int> pre(n + 1, 0), suf(n + 1, 0);
        for (int i = 2; i <= n; i++)
        {
            int d = max(pre[i - 1], abs(a[i - 1] - a[i]));
            pre[i] = d;
        }

        for (int i = n - 1; i >= 0; i--)
        {
            int d = max(suf[i + 1], abs(a[i + 1] - a[i]));
            suf[i] = d;
        }
        
        int ans = INT_MAX;
        for (int i = 2; i <= n; i++)
        {
            int d = abs(a[i] - a[i - 1]);

            int newD = d % 2 == 1 ? ((d / 2) + 1) : (d / 2);

            int maxGap = max({pre[i - 1], suf[i], newD});

            ans = min(maxGap, ans);
        }

        cout << ans << endl;

        //-------------------- Find with  k stick added----------------------------

        int l = 0, r = 1e9;
        int ans = INT_MAX;
        while (l <= r)
        {
            int mid = (l + r) / 2;
            if (possible(a, k, mid))
            {
                ans = mid;
                r = mid - 1;
            }   
            else
            {
                l = mid + 1;
            }
        }
        cout << ans << endl;
    }
    return 0;
}