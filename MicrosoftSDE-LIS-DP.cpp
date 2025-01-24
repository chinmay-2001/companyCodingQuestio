// https://www.desiqna.in/17528/microsoft-sde-ctc-45lpa-oa-coding-questions-set-129
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
        cin.ignore();
        vector<string> v(n + 1);
        int rg = 0, gr = 0, rr = 0, gg = 0;
        f(i, 0, n)
        {
            cin >> v[i];
            if (v[i] == "RG")
            {
                rg++;
            }
            else if (v[i] == "RR")
            {
                rr++;
            }
            else if (v[i] == "GR")
            {
                gr++;
            }
            else if (v[i] == "GG")
            {
                gg++;
            }
        }
        vector<int> dp(n + 1, 1);

        // -------O(n^2) solution------
        // for (int i = 2; i <= n; i++)
        // {
        //     int j = i - 1;
        //     string s1 = v[i - 1];
        //     while (j >= 1)
        //     {
        //         string s2 = v[j - 1];
        //         if (s1[0] == s2[1])
        //         {
        //             dp[i] = max(dp[i], 1 + dp[j]);
        //         }
        //         j--;
        //     }
        // }
        // for (int i = 0; i <= n; i++)
        // {
        //     cout << dp[i] << " ";
        // }
        // cout << endl;
        // cout << *max_element(dp.begin(), dp.end()) << endl;

        //-----------O(n) solution---------------

        // int r = 0;
        // int s = 0;
        // for (int i = 0; i < n; i++)
        // {
        //     string str = v[i];
        //     if (str[0] == 'R')
        //     {
        //         if (str[1] == 'R')
        //         {
        //             r = 1 + r;
        //         }
        //         else
        //         {
        //             s = 1 + r;
        //         }
        //     }
        //     else
        //     {
        //         if (str[1] == 'R')
        //         {
        //             r = 1 + s;
        //         }
        //         else
        //         {
        //             s = 1 + s;
        //         }
        //     }
        // }
        // cout << max(r, s) << endl;

        //--------------Real question solution----------------

        int ans = 0;
        if (rg != 0 and gr == 0)
        {
            ans = gg + rr + 1;
        }
        else if (gr != 0 and rg == 0)
        {
            ans = gg + rr + 1;
        }
        else if (rg == 0 and gr == 0)
        {
            ans = max(rr, gg);
        }
        else
        {
            if (rg > gr)
            {
                ans = gg + rr + 2 * gr + 1;
            }
            else if (gr > rg)
            {
                ans = rr + gg + 2 * rg + 1;
            }
            else if (gr == rg)
            {
                ans = gr + rr + rg + gg;
            }
        }
        cout << ans << endl;
    }
    return 0;
}