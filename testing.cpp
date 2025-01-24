#include<bits/stdc++.h>
using namespace std;
const int mod=1e9+7;
int main() {
    int t;
    cin>>t;
    while(t--){
        int n;
        cin>>n;
        vector<int> a(n);
        vector<int> b(n);
        for(int i=0;i<n;i++)cin>>a[i];
        for(int i=0;i<n;i++)cin>>b[i];

        vector<vector<int>> dp(n+1,vector<int>(3,-1));
        dp[1][1]=a[0];
        dp[1][2]=b[0];
        dp[2][1]=a[1]+max(dp[1][1],dp[1][2]);
        dp[2][1]=b[1]+max(dp[1][1],dp[1][2]);
        for(int i=3;i<=n;i++){
            dp[i][1]=max(a[i-1]+ a[i-2]+dp[i-2][2],a[i-1]+b[i-2]+dp[i-2][2]);

        }
        
    }

    return 0;
}