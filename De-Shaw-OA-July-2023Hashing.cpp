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
int lef=-1,righ=-1;
int countmex(vector<int> nums,int i){
    int cnt=0;
    //how to get no of mex
    int n=nums.size();
    if(i==1){
        int idx=find(nums.begin(),nums.end(),i)-nums.begin();
        int l=idx;
        int r=n-idx-1;
        cout<<l<<" "<<r<<endl;
        
        return (l*(l+1)/2) + (r*(r+1)/2);
    }else{
        int idx=find(nums.begin(),nums.end(),i-1)-nums.begin();
        int cidx=find(nums.begin(),nums.end(),i)-nums.begin();
        if(lef==-1  and righ==-1){
            lef=idx;
            righ=idx;
        }else{
            if(idx<lef)lef=idx;
            if(idx>righ)righ=idx;
        }
        cout<<"cidx:"<<cidx<<" idx:"<<idx<<endl;
        cout<<lef<<" "<<righ<<endl;
        

        if(cidx>lef  and cidx<righ)return 0;

        int x,y;
        if(cidx<lef){
            x=lef-cidx-1;
            y=n-righ-1;
        }
        if(cidx>righ){
            x=lef;
            y=cidx-righ-1;
        }

        cout<<"x:"<<x<<" y:"<<y<<endl;
        int cnt=x*y+x+y+1;
        cout<<"int:"<<cnt<<endl;
        return cnt;
    }
}

/* Main()  function */
int main()
{
    int tc;
    cin>>tc ;

    while(tc--){
        int n,k;
        cin>>n>>k;
        vector<int> nums(n);
        f(i,0,n)cin>>nums[i];
        // cout<<n<<k<<endl;
        
        int cnt=0;
        int tsubarray=(n*(n+1)/2);
        if(tsubarray>=k){
            cout<<n+1<<endl;
            continue;
        }
        int ans=-1;
        for(int i=1;i<=n;i++){
            // cout<<i<<endl;
            cnt+=countmex(nums,i);
            cout<<"cnt:"<<cnt<<endl;
            if(cnt>=k){
                ans=i;
                break;
            }
        }
        cout<<ans<<endl;
    }
    return 0;
}