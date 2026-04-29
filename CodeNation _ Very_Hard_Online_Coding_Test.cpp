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

ll dp[5005][5005];
ll diff[500000 + 5];

ll kk(ll y, ll g)
{

    ll vv = 0;
    vv = abs(diff[g] - diff[y]);
    return vv;
}

int main()
{

    ll n;
    cin >> n;
    ll k;
    cin >> k;
    ll b[n + 1] = {0};
    ll i = 1;
    while (i <= n)
    {
        cin >> b[i];
        i++;
    }

    ll p1 = n / k;     //....g
    ll p2 = n / k + 1; //...g+1...
    ll k2 = 0;         // count of g..basically x..
    ll k5 = 0;         // count of g+1...basically y..
    i = 1;
    while (i <= k)
    {
        ll vv = (n - i) / k;
        vv++;
        if (vv == p1)
        {
            k2++;
        }
        if (vv == p2)
        {
            k5++;
        }
        i++;
    }
    cout << k2 << " " << k5;
    cout << "\n";

    sort(b + 1, b + n + 1);

    i = 2;
    while (i <= n)
    {
        ll gg = abs(b[i] - b[i - 1]);
        diff[i] = diff[i - 1] + gg; // diff is p-s[]
        i++;
    }

    f(i, 0, n) cout << diff[i] << " ";
    cout << endl;
    i = 1;
    while (i <= k2)
    {

        ll sum = i * p1 + 0 * p2;
        ll g = sum;
        ll y = (sum - p1 + 1);
        cout << "y:" << y << " g:" << g << endl;
        dp[i][0] = dp[i - 1][0] + kk(y, g); // base_case...

        i++;
    }

    i = 1;
    while (i <= k5)
    {

        ll sum = 0 * p1 + i * p2;
        ll g = sum;
        ll y = (sum - p2 + 1);

        dp[0][i] = dp[0][i - 1] + kk(y, g); // base_case...

        i++;
    }

    i = 0;
    while (i <= k2)
    {
        ll j = 0;
        while (j <= k5)
        {

            ll sum = i * p1 + j * p2;

            if (sum != 0)
            {
                ll v5 = 1e18;
                if (j >= 1)
                {
                    // focus on j part....

                    ll g = sum;
                    ll y = (sum - p2 + 1);
                    v5 = dp[i][j - 1] + kk(y, g);
                }
                ll v8 = 1e18;

                if (i >= 1)
                {

                    ll g = sum;
                    ll y = (sum - p1 + 1);
                    v8 = dp[i - 1][j] + kk(y, g);
                }

                dp[i][j] = min(v5, v8);
            }

            j++;
        }
        i++;
    }

    cout << dp[k2][k5];

    /*

    15 4
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

5
-5 -5 3 3 3



    */

    return 0;
}
// #include <bits/stdc++.h>
// using namespace std;

// /* clang-format off */

// /* TYPES  */
// #define ll long long
// #define pii pair<int, int>
// #define pll pair<long long, long long>
// #define vi vector<int>
// #define vll vector<long long>
// #define mii map<int, int>
// #define si set<int>
// #define sc set<char>

// /* FUNCTIONS */
// #define f(i,s,e) for(long long int i=s;i<e;i++)
// #define cf(i,s,e) for(long long int i=s;i<=e;i++)
// #define rf(i,e,s) for(long long int i=e-1;i>=s;i--)
// #define pb push_back
// #define eb emplace_back

// /* PRINTS */
// template <class T>
// void print_v(vector<T> &v) { cout << "{"; for (auto x : v) cout << x << ","; cout << "\b}"; }

// /* UTILS */
// #define MOD 1000000007
// #define PI 3.1415926535897932384626433832795
// #define read(type) readInt<type>()
// ll min(ll a,int b) { if (a<b) return a; return b; }
// ll min(int a,ll b) { if (a<b) return a; return b; }
// ll max(ll a,int b) { if (a>b) return a; return b; }
// ll max(int a,ll b) { if (a>b) return a; return b; }
// ll gcd(ll a,ll b) { if (b==0) return a; return gcd(b, a%b); }
// ll lcm(ll a,ll b) { return a/gcd(a,b)*b; }
// string to_upper(string a) { for (int i=0;i<(int)a.size();++i) if (a[i]>='a' && a[i]<='z') a[i]-='a'-'A'; return a; }
// string to_lower(string a) { for (int i=0;i<(int)a.size();++i) if (a[i]>='A' && a[i]<='Z') a[i]+='a'-'A'; return a; }
// bool prime(ll a) { if (a==1) return 0; for (int i=2;i<=round(sqrt(a));++i) if (a%i==0) return 0; return 1; }
// void yes() { cout<<"YES\n"; }
// void no() { cout<<"NO\n"; }

// /*  All Required define Pre-Processors and typedef Constants */
// typedef long int int32;
// typedef unsigned long int uint32;
// typedef long long int int64;
// typedef unsigned long long int  uint64;

// /* clang-format on */

// /* Main()  function */
// int main()
// {
//     ios::sync_with_stdio(false);
//     cin.tie(nullptr);
//     cout.tie(nullptr);
//     int tc;
//     cin >> tc;

//     while (tc--)
//     {
//         int n, b;
//         cin >> n >> b;
//         vector<int> v(n);
//         for (int i = 0; i < n; i++)
//         {
//             cin >> v[i];
//         }

//         int g1 = n / b;
//         int g2 = n / b + 1;
//         int c1 = 0;
//         int c2 = 0;
//         for (int i = 1; i <= b; i++)
//         {
//             int x = (((n - i) / b) + 1);
//             if (x == g1)
//             {
//                 c1++;
//             }
//             else if (x == g2)
//             {
//                 c2++;
//             }
//         }

//         sort(v.begin(), v.end());

//         vector<int> diff(n);
//         for (int i = 2; i <= n; i++)
//         {
//             int d = (v[i - 1] - v[i - 2]);
//             diff[i] = diff[i - 1] + d;
//         }

//         vector<vector<int>> dp(c1 + 1, vector<int>(c2 + 1, 0));

//         for (int i = 1; i <= c1; i++)
//         {
//             for (int j = 1; j <= c2; j++)
//             {
//                 int a1 = dp[i - 1][j] + ();
//                 int a2 = dp[i][j - 1] + mex();
//                 dp[i][j] = min(a1, a2);
//             }
//         }

//         cout << dp[c1][c2] << endl;
//     }
//     return 0;
// }