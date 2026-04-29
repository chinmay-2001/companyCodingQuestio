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
vector<string> vec = {"BG", "BR", "BY", "GR", "GY", "RY"};
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    int tc;
    cin >> tc;

    while (tc--)
    {
        int n, q;
        cin >> n >> q;
        vector<string> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        vector<vector<int>> mp(6);
        for (int i = 0; i < n; i++)
        {
            if (a[i] == "BG")
                mp[0].push_back(i);
            if (a[i] == "BR")
                mp[1].push_back(i);
            if (a[i] == "BY")
                mp[2].push_back(i);
            if (a[i] == "GR")
                mp[3].push_back(i);
            if (a[i] == "GY")
                mp[4].push_back(i);
            if (a[i] == "RY")
                mp[5].push_back(i);
        }

        while (q--)
        {
            int u, v;
            cin >> u >> v;
            u--;
            v--;
            set<int> s;
            for (auto ch : a[u])
                s.insert(ch);
            for (auto ch : a[v])
                s.insert(ch);
            if (s.size() < 4)
            {
                cout << abs(u - v) << endl;
                continue;
            }

            if (u > v)
                swap(u, v);

            int l = -1, r = n;
            for (int i = 0; i < 6; i++)
            {
                if (vec[i] == a[u] || vec[i] == a[v])
                    continue;

                auto it1 = lower_bound(mp[i].begin(), mp[i].end(), u);

                if (it1 != mp[i].begin())
                {
                    it1--;
                    l = max(l, *it1);
                }

                auto it2 = upper_bound(mp[i].begin(), mp[i].end(), u);
                if (it2 != mp[i].end())
                {
                    r = min(r, *it2);
                }
            }
            if (l == -1 and r == n)
            {
                cout << -1 << endl;
            }
            else if (l == -1)
            {
                cout << abs(r - u) + abs(v - r) << endl;
            }
            else if (r == n)
            {
                cout << abs(l - u) + abs(v - l) << endl;
            }
            else
            {
                cout << min(abs(r - u) + abs(v - r), abs(l - u) + abs(v - l)) << endl;
            }
        }
    }
    return 0;
}