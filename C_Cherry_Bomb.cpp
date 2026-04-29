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
        int n, k;
        cin >> n >> k;
        vi a(n), b(n);

        for (int i = 0; i < n; i++)
            cin >> a[i];
        for (int i = 0; i < n; i++)
            cin >> b[i];
        int req = -1;
        int all = 1;
        int np = 0;
        for (int i = 0; i < n; i++)
        {
            if (b[i] != -1)
            {
                all = 0;
                int newReq = a[i] + b[i];
                if (req == -1)
                {
                    req = newReq;
                }
                if (newReq != req)
                {
                    np = 1;
                    break;
                }
            }
        }

        if (np)
        {
            cout << 0 << endl;
        }
        else
        {
            if (all)
            {

                int mini = *min_element(a.begin(), a.end());
                int maxi = *max_element(a.begin(), a.end());
                int r1 = mini + k;
                int a = r1 - maxi + 1;
                if (a >= 0)
                {
                    cout << a << endl;
                }
                else
                {
                    cout << 0 << endl;
                }
            }
            else
            {
                int maxi = INT_MIN;
                int mini = INT_MAX;
                for (int i = 0; i < n; i++)
                {
                    if (b[i] == -1)
                    {
                        maxi = max(maxi, a[i]);
                        mini = min(mini, a[i]);
                    }
                }

                if (maxi > req)
                {
                    cout << 0 << endl;
                }
                else
                {
                    if (req - mini <= k)
                    {
                        cout << 1 << endl;
                    }
                    else
                    {
                        cout << 0 << endl;
                    }
                }
            }
        }
    }
    return 0;
}