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
        int no, d, z, m, n;
        cin >> no >> d >> z >> m >> n;
        vi v(no);
        f(i, 0, no) cin >> v[i];

        vi p1(no);
        vi p2(no);

        if (v[0] == d)
            p1[0] = 1;
        if (v[0] == z)
            p2[0] = 1;

        for (int i = 1; i < no; i++)
        {
            if (v[i] == d)
                p1[i] = p1[i - 1] + 1;
            else
                p1[i] = p1[i - 1];

            if (v[i] == z)
                p2[i] = p2[i - 1] + 1;
            else
                p2[i] = p2[i - 1];
        }

        // so if m/n == c1/c2 then c1*n == c2*m
        //   c1= p1[j]- p1[i-1] and c2=p2[js]- p2[i-1]
        //   p1[j]-p1[i-1]/p2[j]-p2[i-1]== m/n;
        //   n*p1[j]- n* p1[i-1] == m*p2[j]- m*p2[i-1]
        //   m*p2[i-1] -n*p1[i-1] ==  m*p2[j] -n*p1[j]

        unordered_map<int, int> ma;

        int cnt = 0;
        ma[0] = 1;
        for (int i = 0; i < no; i++)
        {
            int value = m * p2[i] - n * p1[i];
            cnt += ma[value];
            ma[value]++;
        }

        // We also have to count no of subarray where no of d and z are 0 because it will be counted by this subarray

        cout << cnt << endl;
    }
    return 0;
}

// how many array will be palidrome if %k is done