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

int segnode[1000005];
int a[10000];
/* clang-format on */
void build(int node,int start,int end){
    if(start==end){
        segnode[node]=a[start];
    }else{
        int mid=(start+end)/2;
        build(2*node,start,mid); 
        build(2*node+1,mid+1,end);
        segnode[node]=segnode[2*node]+segnode[2*node+1]; 
    }
}

int query(int node,int start,int end,int l,int r){
    if(end<l || start>r){
        return 0;
    }

    if(l<=start and r>=end){
        return segnode[node];
    }

    int mid=(start+end)/2;
    
    int left=query(2*node,start,mid,l,r);
    int right=query(2*node+1,mid+1,end,l,r);
    return left+right;
}

int update(int node,int idx,int val,int start,int end){
    if(start==end){
        segnode[node]=val;
        a[idx]=val;
    }else{
        int mid=(start+end)/2;
        if(idx<=mid){
            update(2*node,idx,val,start,mid);
        }else{
            update(2*node+1,idx,val,mid+1,start);
        }

        segnode[node]=segnode[2*node]+segnode[2*node+1];
    }
}

/* Main()  function */
int main()
{
    int tc;
    cin>>tc;

    while(tc--){
        int n;
        cin>>n;
        
        f(i,0,n)cin>>a[i];

        build(1,0,n-1);
        int q;
        cin>>q;
        while(q--){
            int t,l,r;
            cin>>t>>l>>r;
            //query
            if(t==1){
                cout<<query(1,0,n-1,l-1,r-1)<<endl;
            }else if(t==2){  //update
                
                update(1,l-1,r,0,n-1);
            }
        }
    }
    return 0;
}