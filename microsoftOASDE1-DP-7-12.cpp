// https://docs.google.com/document/d/1cM6ykw9dvoXVawGSINq_OezgivdO0kKfsWA27XoMLIo/edit
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
// 1 2 4 6
/* Main()  function */
int main()
{
    int tc;
    cin>>tc;

    while(tc--){
    // -------------------- no of way to make sum k using 1 2 4 6 ----------------------

        //     int y;
        //     cin>>y;
        //     int dp[y+1];
        //     memset(dp,0,sizeof(dp));
        //     dp[0]=1;
        //     // dp[1]=1;
        //     for(int i=1;i<=y;i++){
        //         if(i-1>=0){
        //             dp[i]+=dp[i-1];
        //         }
        //         if(i-2>=0){
        //             dp[i]+=dp[i-2];
        //         }
        //         if(i-4>=0){
        //             dp[i]+=dp[i-4];
        //         }
        //         if(i-6>=0){
        //             dp[i]+=dp[i-6];
        //         }
        //     }
        //     cout<<dp[y]<<endl;
    
     // -------------------- no of way to make sum k using 1 2 4 6 BUT using 4 atmost 2 times ----------------------

        // int y;
        // cin>>y;
        // vector<vector<int>> dp(y+1,vector<int>(3));
        // dp[0][0]=1;
        // for(int i=1;i<=y;i++){
        //     for(int j=0;j<=2;j++){
        //         if(i-1>=0){
        //             dp[i][j]+=dp[i-1][j];
        //         }
        //         if(i-2>=0 ){
        //             dp[i][j]+=dp[i-2][j];
        //         }
        //         if(i-4>=0 and j==1){
        //             dp[i][j]+=dp[i-4][0];
        //         }
        //         if(i-4>=0 and j==2){
        //             dp[i][j]+=dp[i-4][1];
        //         }
        //         if(i-6>=0){
        //             dp[i][j]+=dp[i-6][j];
        //         }
        //     }          
        // }

        // cout<<dp[y][0]+dp[y][1]+dp[y][2]<<endl;

    // -------------------- no of way to make sum k using 1 2 4 6 BUT using 4 atmost 2 times and 6 atmost 2 times ----------------------

        int y;
        cin>>y;
        cout<<"y:"<<y<<endl;
        vector<vector<vector<int>>> dp(y+1,vector<vector<int>>(3,vector<int>(3,0)));
        dp[0][0][0]=1;

        if(y>0)dp[1][0][0]=1;
        for(int i=2;i<=y;i++){
            for(int j=0;j<=2;j++){
                for(int k=0;k<=2;k++){
                    if(i-1>=0){
                        dp[i][j][k]+=dp[i-1][j][k];
                    }
                    if(i-2>=0){
                        dp[i][j][k]+=dp[i-2][j][k];
                    }
                    if(i-4>=0 and j==1){
                        dp[i][j][k]+=dp[i-4][0][k];
                    }
                    if(i-4>=0 and j==2){
                        dp[i][j][k]+=dp[i-4][1][k];
                    }
                    if(i-6>=0 and k==1){
                        dp[i][j][k]+=dp[i-6][j][0];
                    }
                    if(i-6>=0 and k==2){
                        dp[i][j][k]+=dp[i-6][j][1];
                    }
                }
            }          
        }

        cout<<dp[y][0][0]+dp[y][1][0]+dp[y][2][0]+dp[y][1][0]+dp[y][1][1]+dp[y][1][2]+dp[y][2][0]+dp[y][2][1]+dp[y][2][2]<<endl;


    }
    return 0;
}