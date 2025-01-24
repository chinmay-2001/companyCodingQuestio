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
        string s;
        cin>>s;
        //---------------find the no of substrings that are palindromes in range l to r----------------

        // int n=s.size();
        // vector<vector<int>> dp(n+1,vector<int>(n+1,0));
        // vector<vector<int>> dp1(n+1,vector<int>(n+1,0));
        // for(int i=0;i<n;i++){
        //     dp[i][i]=1;
        //     dp1[i][i]=1;
        // }
        // // cout<<1<<endl;
        // for(int i=0;i<n-1;i++){
        //     if(s[i]==s[i+1]){
        //         dp[i][i+1]=1;
        //     }
        //     dp1[i][i+1]=dp1[i][i]+dp1[i+1][i+1]+dp[i][i+1];
        // }
        // // cout<<2<<endl;

        // for(int i=3;i<=n;i++){
        //     for(int j=0;j<=n-i;j++){
        //         int k=j+i-1;
        //         if(s[j]==s[k] && dp[j+1][k-1]==1){
        //             dp[j][k]=1;
        //         }
        //         dp1[j][k]=dp1[j+1][k]+dp1[j][k-1]-dp1[j+1][k-1]+dp[j][k];
        //     }
        // }
        // // cout<<3<<endl;

        // int q;
        // cin>>q;
        // for(int i=0;i<q;i++){
        //     int a,b;
        //     cin>>a>>b;
        //     cout<<dp1[a][b]<<endl;
        // }

        //-------------Given a string please tell how many substring pairs are in the palindrome------------

        int n=s.size();
        vector<vector<int>> dp(n+1,vector<int>(n+1,0));
        vector<vector<int>> dp1(n+1,vector<int>(n+1,0));
        for(int i=0;i<n;i++){
            dp[i][i]=1;
            dp1[i][i]=1;
        }

        // cout<<1<<endl;

        for(int i=0;i<n-1;i++){
            if(s[i]==s[i+1]){
                dp[i][i+1]=1;
            }
            dp1[i][i+1]=dp1[i][i]+dp1[i+1][i+1]+dp[i][i+1];
        }
        // cout<<2<<endl;

        for(int i=3;i<=n;i++){
            for(int j=0;j<=n-i;j++){
                int k=j+i-1;
                if(s[j]==s[k] && dp[j+1][k-1]==1){
                    dp[j][k]=1;
                }
                dp1[j][k]=dp1[j+1][k]+dp1[j][k-1]-dp1[j+1][k-1]+dp[j][k];
            }
        }

        // cout<<3<<endl;

        int count=0;
        for(int i=0;i<n-1;i++){
            int x=0;
            for(int j=0;j<=i;j++){
                if(dp[j][i]==1){
                    x++;
                }
            }
            int y=dp1[i+1][n-1];
            // cout<<x<<" "<<y<<endl;
            count+=x*y;
        }
        cout<<count<<endl;

        //-------------Given a string please tell how many substring tuple([l1..r1],[l2…r2],[l3…r3]) are in the palindrome------------

        // int n=s.size();
        // vector<vector<int>> dp(n+1,vector<int>(n+1,0));
        // vector<vector<int>> dp1(n+1,vector<int>(n+1,0));
        
        // for(int i=0;i<n;i++){
        //     dp[i][i]=1;
        //     dp1[i][i]=1;
        // }

        // // cout<<1<<endl;

        // for(int i=0;i<n-1;i++){
        //     if(s[i]==s[i+1]){
        //         dp[i][i+1]=1;
        //     }
        //     dp1[i][i+1]=dp1[i][i]+dp1[i+1][i+1]+dp[i][i+1];
        // }
        // // cout<<2<<endl;

        // for(int i=3;i<=n;i++){
        //     for(int j=0;j<=n-i;j++){
        //         int k=j+i-1;
        //         if(s[j]==s[k] && dp[j+1]==dp[k-1]){
        //             dp[j][k]=1;
        //         }
        //         dp1[j][k]=dp1[j+1][k]+dp1[j][k-1]-dp1[j+1][k-1]+dp[j][k];
        //     }
        // }

        // // cout<<3<<endl;

        // vector<vector<int>> dp3(n+1,0);
        // int count=0;
        // for(int i=0;i<n-1;i++){
        //     int x=0;
        //     for(int j=0;j<=i;j++){
        //         if(dp[j][i]==1){
        //             x++;
        //         }
        //     }
        //     int y=dp1[i+1][n-1];
        //     // cout<<x<<" "<<y<<endl;
        //     dp3[i]=x*y;
        //     count+=x*y;
        // }
        // cout<<count<<endl;

        
    }
    return 0;
}