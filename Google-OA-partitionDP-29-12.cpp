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
    cin>>tc;

    while(tc--){
        int n;
        cin>>n;
        vi a(n);
        vi b(n);
        f(i,0,n)cin>>a[i];
        f(i,0,n)cin>>b[i];
        
        // vector<int> dp(n+1,1e9);
        // dp[0]=0;
        // dp[1]=a[0];
        // for(int i=2;i<=n;i++){
        //     int j=i;
        //     int smon=0;
        //     while(j>=1){
        //         smon+=b[j-1];
        //         if(i==j){
        //             int micr=a[j-1];
        //             dp[i]=min(dp[i],micr+dp[j-1]);
        //         }else{
        //             dp[i]=min(dp[i],smon+dp[j-1]);
        //         }
        //         j--;
        //     }
        // }
        // cout<<dp[n]<<endl;

        //------------------------------//

        vector<vector<int>> dp(n+1,vector<int>(4,0));

        dp[1][0]=a[0];
        dp[1][2]=1e9;
        dp[1][1]=b[0];

        for(int i=2;i<=n;i++){
            int v1=a[i-1]+min(dp[i-1][0],dp[i-1][2]);
            int v2=b[i-1]+b[i-2]+min(dp[i-2][0],dp[i-2][1]);
            int v3=b[i-1]+min(dp[i-1][0],dp[i-1][1]);

            dp[i][0]=v1;
            dp[i][1]=v3;
            dp[i][2]=v2;
        }
        cout<<min(dp[n][0],dp[n][2])<<endl;
    }
    return 0;
}