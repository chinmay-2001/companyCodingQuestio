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

bool customsorting(vector <int>&a,vector<int> &b){
    if(a[1]<b[1]){
        return true;
    }else if(a[1]>b[1]){
        return false;
    }else{
        if(a[0]<b[0]){
            return true;
        }else{
            return false;
        }
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
        vector<vector<int>> ranges;
        vector<int> rightRange;
        vector<int> leftRange;
        for(int i=0;i<n;i++){
            int l,r;
            cin>>l>>r;
            ranges.push_back({l,r});
            rightRange.push_back(r);
            leftRange.push_back(l);
        }
        sort(rightRange.begin(),rightRange.end());
        sort(ranges.begin(),ranges.end(),customsorting);

        //for pair of ranges
        // int cnt=0;
        // for(int i=1;i<n;i++){
        //     int left=ranges[i][0];
        //     auto it=lower_bound(rightRange.begin(),rightRange.end(),left);

        //     cnt+=(it- rightRange.begin());
        // }
        // cout<<cnt<<endl;

        //for triplet of range
        // int cnt=0;
        // for(int i=1;i<n;i++){
        //     int l=ranges[i][0];
        //     int r=ranges[i][1];
        //     auto rit=lower_bound(rightRange.begin(),rightRange.end(),l);
        //     int lcnt=(rit- rightRange.begin());
        //     auto lit=upper_bound(leftRange.begin(),leftRange.end(),r);
        //     int rcnt=n-(lit-leftRange.begin());
        //     cnt+=(lcnt*rcnt);
        // }
        // cout<<cnt<<endl;

        //for quardraplate of range

        // int paircnt=0;
        // vector<int>  prefixPair(n,0);
        // for(int i=n-1;i>=0;i--){
        //     int left=ranges[i][0];
        //     auto it=upper_bound(leftRange.begin(),leftRange.end(),left);
        //     paircnt=n- (it- leftRange.begin());
        //     if(i==n-1){
        //         prefixPair[i]=paircnt;
        //     }
        //     else{
        //         prefixPair[i]=prefixPair[i+1]+paircnt;
        //     }
        // }

        // int cnt=0;
        // for(int i=1;i<n-1;i++){
        //     int l=ranges[i][0];
        //     int r=ranges[i][1];
        //     auto rit=lower_bound(rightRange.begin(),rightRange.end(),l);
        //     int lcnt=(rit- rightRange.begin());
        //     int rcnt=prefixPair[i+1];
        //     cnt+=(lcnt*rcnt);
        // }
        // cout<<cnt<<endl;


        //for k ranges

        vector<int> dp

        for(int i=0;i<n;i++){
            int
        }

        

    }
    return 0;
}