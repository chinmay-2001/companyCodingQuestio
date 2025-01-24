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
int OR(int i,int j,vector<vector<int>>& prefix){
    int num=0;
    for(int k=0;k<32;k++){
        if(i==0){
            int c=prefix[k][j];
            if(c>0){
                num+=(pow(2,k));
            }
        }else{
            int c=prefix[k][j]-prefix[k][i-1];
            if(c>0){
                num+=(pow(2,k));
            }
        }
    }
    return num;
}

int check(vector<int>& a,int mid,vector<vector<int>>& prefix){
    int n=a.size();
    int i=0,j=0;
    int cnt=0;
    while(j<n){
        int o_r=OR(i,j,prefix);
        while(o_r>mid){
            o_r=OR(i+1,j,prefix);
            i++;
        }
        cnt+=(j-i+1);
        j++;
    }  
    return cnt;
}
/* Main()  function */
int main()
{
    int tc;
    cin>>tc;

    while(tc--){
        int n,k;
        cin>>n>>k;
        vector<int> a(n);
        for(int i=0;i<n;i++)cin>>a[i];
        int t=n*(n+1)/2;
        int ks=t-k+1;
        int i=0,j=1023;
        int ans=-1;
        
        vector<vector<int>> prefix(32,vector<int>(n+1,0));

        for(int i=0;i<n;i++){
            int num=a[i];
            int id=0;
            while(num>0){
                prefix[id][i]=num%2;
                num/=2;
                id++;
            }
        }

        for(int i=1;i<n;i++){
            for(int j=0;j<32;j++){
                prefix[j][i]+=prefix[j][i-1];
            }
        }

        while(i<=j){
            int mid=(i+j)/2;
            int c=check(a,mid,prefix);
            if(c>=ks){
                ans=mid;
                j=mid-1;
            }else{
                i=mid+1;
            }
        }

        cout<<ans<<endl;
    }
    return 0;
}