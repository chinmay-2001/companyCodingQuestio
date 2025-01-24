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

int segnode[100000000];
vector<int> temp(200005,0);

void build(int node,int start,int end){
    if(start==end){
        segnode[node]=0;
        
    }else{
        int mid=(start+end)/2;
        build(2*node,start,mid);
        build(2*node+1,mid+1,end);
        segnode[node]=segnode[2*node] + segnode[2*node+1];
    }
}

int query(int node,int start,int end,int l, int r){
    // cout<<node<<endl;
    if(start>r || end<l)return 0;

    if(start>=l and end<=r)return segnode[node];

    int mid=(start+end)/2;

    int left=query(2*node,start,mid,l,r);
    int right=query(2*node+1,mid+1,end,l,r);
    return left+right;
}

int update(int node,int start,int end,int idx,int val){
    if(start==end){
        segnode[node]=val;
        temp[start]=val;
    }else{
        int mid=(start+end)/2;
        
        if(mid<idx){
            update(2*node+1,mid+1,end,idx,val);
        }else{
            update(2*node,start,mid,idx,val);
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

        int c,d;
        cin>>c>>d;
        vi a(n);       
        vi b(n);       

        f(i,0,n)cin>>a[i];
        f(i,0,n)cin>>b[i];

        build(1,0,200004);
        int count=0;
        for(int i=0;i<n;i++){
            int diff=a[i]-b[i]+(d-c);
            count+=query(1,0,200004,0,diff);
            int y=temp[a[i]-b[i]];
            update(1,0,200004,a[i]-b[i],y+1);
        }
        cout<<count<<endl;
    }
    return 0;
}