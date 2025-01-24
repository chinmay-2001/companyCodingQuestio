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
    cin>>tc ;

    while(tc--){
        int n,e,k;
        int s,cb;
        int p;
        cin>>n;
        cin>>s>>cb;
        cin>>k;
        cin>>e>>p;

        vector<pair<int,int>> adj[n+1];
        vector<int> w(n);
        for(int i=0;i<e;i++){
            int u,v,weight;
            cin>>u>>v>>weight;
            adj[u].push_back(make_pair(v,weight));
            w[u]=weight;
        }
        // int s;
        // cin>>s;
        vector<vector<int>> d(n+1,vector<int>(k+1,INT_MAX));
        priority_queue< pair<int,pair<int,int>>, vector<pair<int,pair<int,int>>> , greater<pair<int,pair<int,int>>> > que;
        for(int i=0;i<=k;i++){
            d[s][i]=0;
            que.push({d[s][i],{s,i}});
        }

        while(!que.empty()){
            pair<int,pair<int,int>> top=que.top();
            que.pop();
            int wu=top.first;
            int node=top.second.first;
            int nk=top.second.second;        
            if(wu==d[node][nk]){
                for(auto p:adj[node]){
                    int n=p.first;
                    int wv=p.second;
                    if(d[node][nk]+wv<d[n][nk]){
                        d[n][nk]=d[node][nk]+wv;    
                        que.push({d[n][nk],{n,nk}});
                    }
                    
                    if(nk+1<=k and d[node][nk]<d[n][nk+1]){
                        d[n][nk+1]=d[node][nk];
                        que.push({d[n][nk+1],{n,nk+1}});
                    }
                }
            }
        }

        // for(int i=1;i<=n;i++){
        //     cout<<d[i]<<" ";
        // }
        if(d[cb][k]==INT_MAX){
            cout<<-1<<endl;
        }else{
            cout<<d[cb][k]<<endl;
        }
        cout<<endl;
    }
    return 0;
}