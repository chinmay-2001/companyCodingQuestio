#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
#define ina(a) for(auto &i: a) cin>>i
#define opa(a) for(auto &i: a) cout << i << ' '; cout << endl;
#define swap(a, i, j) { auto temp = a[i]; a[i] = a[j]; a[j] = temp; }
#define v vector
#define p(res) cout << res << '\n'

int f(v<v<int>> &dp, int mid, int l,int k){
    int res=0;
    for(int b=0; b<32; b++){
        int left=(l>0)?dp[b][l-1]:0;
        if (dp[b][mid]-left == l-mid+1) res|=(1<<b);
    }
    cout<<"l:"<<l<<" r:"<<mid<<" k:"<<k<<endl;
    cout<<res<<endl;
    return res;
}

// Code Here!
void solve(){
    int n, q; cin>>n>>q;
    v<int> a(n); 
    ina(a);
    int res=0;
    v<v<int>> dp(32, v<int> (n, 0));
    for(int i=0; i<n; i++){
        for(int b=0; b<32; b++){
            if (a[i]&(1<<b)) dp[b][i]++;
        }
    }
    for(int b=0; b<32; b++){
        for(int i=1; i<n; i++){
            dp[b][i]+=dp[b][i-1];
        }
    }
    for(int qq=0; qq<q; qq++){
        int k, l; cin>>k>>l; l--;
        int i=l, j=n, cur=n+1; 
        while(i<=j){
            int mid=i+(j-i)/2;
            if (f(dp, mid, l,k) >= k) {
                cur=mid; i=mid+1;
            }
            else j=mid-1;
        }
        if (cur!=n+1) cur++;
        res^=cur;
    }
    p(res);
}

int main(){
    int t=1;
    cin>>t;
    while(t--){
        solve();
        cout<<"---------"<<endl;
    }
}