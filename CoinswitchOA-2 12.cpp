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
        int n,k,m;
        cin>>n>>k>>m;
        vi nums(n);
        f(i,0,n)cin>>nums[i];
     
        long long dp[n+1][k+1][m+1];
        memset(dp, 0, sizeof(dp));
        dp[0][0][0]=1;

       for(int i=1;i<=n;i++){
            for(int j=0;j<k;j++){
                for(int z=0;z<=m;z++){ 
                    dp[i][j][z]+=dp[i-1][j][z]%MOD;
                    if(z>0){
                        long long v1=nums[i-1]%k;
                        if(v1<=j){
                            long long l=j-v1;
                            dp[i][j][z]=(dp[i][j][z]%MOD+dp[i-1][l][z-1]%MOD)%MOD;
                        }else{
                            long long l=(k-v1+j);
                            dp[i][j][z]=(dp[i][j][z]%MOD+dp[i-1][l][z-1]%MOD)%MOD;
                        }
                        // int l=(j-v1+k)%k;
                        // if (z>0) dp[i][j][z]=(dp[i][j][z]+dp[i-1][l][z-1])%MOD;
                    }
                }
            }
        }
   
        cout<<dp[n][0][m]<<endl;

    }
    return 0;
}

   // int dp[n+1][k+1];
        // memset(dp, 0, sizeof(dp));

        // dp[0][0]=1;
        // for(int i=1;i<=n;i++){
        //     //selection null as 1
        //     dp[i][0]=1;
        // }

        //Given “N” coins; each ranging from 0 to N-1 -> find the number of ways to pick a subset so that the sum is “M”. 
        // for(int i=1;i<=n;i++){
        //     for(int j=1;j<=k;j++){
        //         dp[i][j]=dp[i-1][j];
        //         if(nums[i-1]<=j){
        //             dp[i][j]+=dp[i-1][j-nums[i-1]];
        //         }
        //     }
        // }
        // cout<<dp[n][k]<<endl;

        //Given “N” coins; each ranging from 0 to N-1 -> find the number of ways to pick a subset so that the sum is divisible by “M”. 

    //     int dp[n+1][k+1];
    //     memset(dp, 0, sizeof(dp));
    //     dp[0][0]=1;

    //    for(int i=1;i<=n;i++){
    //         for(int j=0;j<k;j++){
    //             dp[i][j]=dp[i-1][j];
    //             int v1=nums[i]%k;
    //             if(v1<=j){
    //                 int l=j-v1;
    //                 dp[i][j]+=dp[i-1][l];
    //             }else{
    //                 int l=k-v1+j;
    //                 dp[i][j]+=dp[i-1][l];
    //             }
    //         }
    //     }
    //     for(int i=0;i<=n;i++){
    //         for(int j=0;j<k;j++){
    //             cout<<dp[i][j]<<" ";
    //         }
    //         cout<<endl;
    //     }
    //     cout<<dp[n][0]<<endl;

    //Given “N” coins; each ranging from 0 to N-1 -> find the number of ways to pick a subset of size “m” so that the sum is divisible by “k”.

         // for(int i=0;i<=n;i++){
        //     for(int j=0;j<k;j++){
        //         for(int z=0;z<=m;z++){
        //             cout<<dp[i][j][z]<<" ";
        //         }
        //     }
        //     cout<<endl;
        // }
