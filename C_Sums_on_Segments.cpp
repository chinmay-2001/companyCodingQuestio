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
int maxSubarraySum(vector<int> &v)
{
    int msum = 0;
    int sum = 0;
    for (int i = 0; i < v.size(); i++)
    {
        int curr = v[i];
        sum = max(curr, sum + curr);
        msum = max(msum, sum);
    }
    return msum;
}

int maxSum(vector<int> &v)
{
    int sum = 0;
    int msum = 0;
    for (int i = 0; i < v.size(); i++)
    {
        sum += v[i];
        msum = max(msum, sum);
    }
    return msum;
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
        vi v(n);
        int idx = -1;
        f(i, 0, n)
        {
            cin >> v[i];
            if (v[i] != -1 and v[i] != 1)
            {
                idx = i;
            }
        }

        vi v1, v2, v3, v4;

        if (idx != -1)
        {
            for (int i = 0; i < idx; i++)
            {
                v1.push_back(v[i]);
                v2.push_back(-1 * v[i]);
            }

            for (int i = idx + 1; i < n; i++)
            {
                v3.push_back(v[i]);
                v4.push_back(-1 * v[i]);
            }

            int mx1 = 0, mn1 = 0, mx2 = 0, mn2 = 0;
            mx1 = maxSubarraySum(v1);
            mn1 = -maxSubarraySum(v2);
            mx2 = maxSubarraySum(v3);
            mn2 = -maxSubarraySum(v4);

            set<int> s;

            for (int i = mn1; i <= mx1; i++)
            {
                s.insert(i);
            }

            for (int i = mn2; i <= mx2; i++)
            {
                s.insert(i);
            }

            int mxp1 = 0, mnp1 = 0, mxp2 = 0, mnp2 = 0;
            reverse(v1.begin(), v1.end());
            reverse(v2.begin(), v2.end());
            mxp1 = maxSum(v1);
            mnp1 = -maxSum(v2);
            mxp2 = maxSum(v3);
            mnp2 = -maxSum(v4);

            int x = v[idx];
            // cout << "x:" << x << endl;

            int m1 = x + mxp1 + mxp2;
            int m2 = x + mnp1 + mnp2;
            for (int i = m2; i <= m1; i++)
            {
                s.insert(i);
            }

            cout << s.size() << endl;
            for (auto it = s.begin(); it != s.end(); it++)
            {
                cout << *it << " ";
            }
            cout << endl;
        }
        else
        {
            int mx = maxSubarraySum(v);
            vector<int> rv;
            for (int i = 0; i < n; i++)
            {
                rv.push_back(-v[i]);
            }
            int mn = -maxSubarraySum(rv);
            cout << mx - mn + 1 << endl;
            for (int i = mn; i <= mx; i++)
            {
                cout << i << " ";
            }
            cout << endl;
        }
    }
    return 0;
}