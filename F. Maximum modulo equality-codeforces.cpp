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

    const int logn = 20;
    vector<vector<int>> st;
/* clang-format on */
int getAns(int l, int r)
{
    int k = __lg(r - l + 1);
    return __gcd(st[k][l], st[k][r - (1 << k) + 1]);
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
        int n, q;
        cin >> n >> q;
        vi v(n - 1);
        vi vec(n);
        f(i, 0, n) cin >> vec[i];

        for (int i = 1; i < vec.size(); i++)
        {
            v[i - 1] = abs(vec[i] - vec[i - 1]);
        }

        st.resize(logn, vector<int>(v.size(), 1));

        for (int i = 0; i < v.size(); i++)
        {
            st[0][i] = v[i];
        }

        f(i, 1, logn)
        {
            for (int j = 0; j + (1 << (i - 1)) < v.size(); j++)
            {
                st[i][j] = __gcd(st[i - 1][j], st[i - 1][j + (1 << (i - 1))]);
            }
        }

        while (q--)
        {
            int l, r;
            cin >> l >> r;
            if (l == r)
            {
                cout << 0 << " ";
            }
            else
            {
                --l;
                r -= 2;
                cout << getAns(l, r) << " ";
            }
        }
        cout << endl;
    }
    return 0;
}