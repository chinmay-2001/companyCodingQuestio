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
        int n;
        cin >> n;
        cin.ignore();
        string s;
        cin >> s;
        if (n == 1)
        {
            cout << 1 << endl;
            continue;
        }

        if (n == 2)
        {
            cout << 0 << endl;
            continue;
        }

        if (n % 2 == 0)
        {
            map<int, int> m1, m2;
            for (int i = 0; i < n; i += 2)
                m1[s[i] - 'a']++;
            for (int i = 1; i < n; i += 2)
                m2[s[i] - 'a']++;

            int mx1 = 0, mx2 = 0;
            for (int i = 0; i <= 26; i++)
                mx1 = max(mx1, m1[i]), mx2 = max(mx2, m2[i]);

            cout << (n / 2 - mx1) + (n / 2 - mx2) << endl;
        }
        else
        {
            vector<vector<ll>> p_odd(n + 1, vector<ll>(27, 0));
            vector<vector<ll>> p_even(n + 1, vector<ll>(27, 0));

            for (int i = 1; i <= n; i++)
            {
                ll ch = s[i - 1] - 'a';
                for (int j = 0; j < 26; j++)
                {
                    if (i % 2 == 0)
                    {
                        if (ch == j)
                        {
                            p_even[i][ch] += (p_even[i - 1][ch] + 1);
                        }
                        else
                        {
                            p_even[i][j] += p_even[i - 1][j];
                        }
                        p_odd[i][j] += p_odd[i - 1][j];
                    }
                    else
                    {
                        if (ch == j)
                        {
                            p_odd[i][ch] += (p_odd[i - 1][ch] + 1);
                        }
                        else
                        {
                            p_odd[i][j] += p_odd[i - 1][j];
                        }
                        p_even[i][j] += p_even[i - 1][j];
                    }
                }
            }

            int ans = INT_MAX;
            for (int i = 1; i <= n; i++)
            {
                if (i == 1)
                {
                    ll m1 = 0, m2 = 0;
                    for (int j = 0; j < 26; j++)
                    {
                        ll p1 = p_odd[n][j] - p_odd[1][j];
                        ll p2 = p_even[n][j] - p_even[1][j];
                        m1 = max(m1, p1);
                        m2 = max(m2, p2);
                    }

                    ans = min((n / 2 - m1) + (n / 2 - m2) + 1, ans);
                }
                else if (i == n)
                {
                    ll m1 = 0, m2 = 0;
                    for (int j = 0; j < 26; j++)
                    {
                        ll p1 = p_odd[n - 1][j];
                        ll p2 = p_even[n - 1][j];
                        m1 = max(m1, p1);
                        m2 = max(m2, p2);
                    }
                    ans = min((n / 2 - m1) + (n / 2 - m2) + 1, ans);
                }
                else
                {
                    ll m1 = 0, m2 = 0;
                    for (int j = 0; j < 26; j++)
                    {
                        ll p1 = p_odd[i - 1][j] + (p_even[n][j] - p_even[i][j]);
                        ll p2 = p_even[i - 1][j] + (p_odd[n][j] - p_odd[i][j]);
                        m1 = max(m1, p1);
                        m2 = max(m2, p2);
                    }
                    ans = min((n / 2 - m1) + (n / 2 - m2) + 1, ans);
                }
            }
            cout << ans << endl;
        }
    }
    return 0;
}