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

int findMedian(vector<int> &v, int l, int r, int dir, int k)
{
    int n = v.size();
    int i;
    if (dir == 1)
    {
        set<int> s1, s2;
        for (i = l; i <= r and (s1.empty() || *s1.end() > k); i++)
        {
            int num = v[i];
            if (s1.empty() and s2.empty())
            {
                s1.insert(v[i]);
            }
            else
            {
                if (num > *s1.end())
                {
                    s2.insert(num);
                }
                else
                {
                    s1.insert(num);
                }

                while (s1.size() - s2.size() > 1)
                {
                    s2.insert(*s1.end());
                    s1.erase(s1.end());
                }

                while (s2.size() > s1.size())
                {
                    s1.insert(*s2.begin());
                    s2.erase(s2.begin());
                }
            }
        }
        i--;
        if ((i - l + 1) & 1)
        {
            while (i + 1 <= r and v[i + 1] > k)
            {
                i++;
            }
        }
    }
    else
    {
        set<int> s1, s2;
        for (i = r; i >= l; i--)
        {
            int num = v[i];
            if (s1.empty() and s2.empty())
            {
                s1.insert(v[i]);
            }
            else
            {
                if (num > *s1.end())
                {
                    s2.insert(num);
                }
                else
                {
                    s1.insert(num);
                }

                while (s1.size() - s2.size() > 1)
                {
                    s2.insert(*s1.end());
                    s1.erase(s1.end());
                }

                while (s2.size() > s1.size())
                {
                    s1.insert(*s2.begin());
                    s2.erase(s2.begin());
                }
            }
        }
        i++;
        if ((i - r + 1) & 1)
        {

            while (i - 1 >= l and v[i - 1] > k)
            {
                i--;
            }
        }
        cout << "i:" << i << endl;
    }
    return i;
}

bool first(vector<int> &v, int k, int n)
{
    int r1 = findMedian(v, 0, n - 1, 1, k);
    if (r1 < n - 1)
    {
        int r2 = findMedian(v, r1 + 1, n - 1, 1, k);
        if (r2 < n - 1)
            return true;
    }
    return false;
}

bool second(vector<int> &v, int k, int n)
{
    int r1 = findMedian(v, 0, n - 1, 1, k);
    if (r1 < n - 1)
    {
        int r2 = findMedian(v, n - 1, r1 + 1, 0, k);
        if (r1 > r2)
        {
            return true;
        }
    }
    return false;
}

bool third(vector<int> &v, int k, int n)
{
    int r1 = findMedian(v, n - 1, 0, 0, k);
    if (r1 > 0)
    {
        int r2 = findMedian(v, r1 - 1, 0, 0, k);
        if (r1 >= 0)
            return true;
    }
    return false;
}

bool solve(vector<int> v, int k)
{
    int n = v.size();
    return first(v, k, n) || second(v, k, n) || third(v, k, n);
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
        vector<int> v(n);
        for (int i = 0; i < n; i++)
            cin >> v[i];
        // cout << "solve:" << solve(v, k) << endl;
        solve(v, k) ? cout << "YES" << endl : cout << "NO" << endl;
    }
    return 0;
}