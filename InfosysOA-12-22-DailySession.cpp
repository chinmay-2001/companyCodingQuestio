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
int check(int mid,int idx,vector<vector<int>>& pre,int k){
    ll num=0;
    for(int i=0;i<32;i++){
        ll diff;
        if(idx!=0){
            diff=pre[i][mid]-pre[i][idx-1];
        }else{
            diff=pre[i][mid];
        }
        if(diff==mid-idx+1){
            num+=pow(2,i);
        }
    }
    // cout<<"l:"<<idx<<" r:"<<mid<<" k:"<<k<<endl;
    // cout<<"num:"<<num<<endl;
    if(num<k)return true;
    return false;
}
/* Main()  function */
int main()
{
    int tc;
    cin>>tc;

    while(tc--){
        int n,q;
        cin>>n>>q;
        vi nums(n);
        for(int i=0;i<n;i++){
            cin>>nums[i];
        }

        vector<vector<int>> pre(32,vector<int>(n,0));
        for(int i=0;i<n;i++){
            int num=nums[i];
            int s=0;
            while(num>0){
                pre[s][i]=num%2;
                num/=2;
                s++;
            }
        }
    
        // for(int i=0;i<32;i++){
        //     for(int j=0;j<n;j++){
        //         cout<<pre[i][j]<<" ";
        //     }
        //     cout<<endl;
        // }

        for(int i=0;i<32;i++){
            for(int j=1;j<n;j++){
                pre[i][j]+=pre[i][j-1];
            }
        }
        // cout<<endl;
        // for(int i=0;i<32;i++){
        //     for(int j=0;j<n;j++){
        //         cout<<pre[i][j]<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<2<<endl;
        int xr=0;
        while(q--){
            int k,idx;
            cin>>k>>idx;
            idx=idx-1;
            int i=idx,j=n;
            int ans=-1;
            while(i<=j){
                int mid=(i+j)/2;
                // cout<<mid<<endl;
                if(check(mid,idx,pre,k)){
                    // cout<<1<<endl;
                    j=mid-1;
                }else{
                    i=mid+1;
                    // cout<<i<<" "<<j<<endl;
                    ans=mid;
                }
            }
            if(ans==-1){
                xr=xr^(n+1);
            }else{
                xr=xr^ans;
            }
        }
        cout<<xr<<endl;
        // cout<<"xr:"<<xr<<endl;
        // cout<<"-----------------"<<endl;
    }
    return 0;
}