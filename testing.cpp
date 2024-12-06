#include <iostream>
#include<bits/stdc++.h>
using namespace std;
const int mod=1e9+7;
int main() {
    int n,k,m;
    cin>>n>>k>>m;
    long long dp[n+1][m][k+1];
    memset(dp,0,sizeof(dp));
    dp[0][0][0]=1;
    for(int i=1;i<=n;i++)
    {
        for(int j=0;j<m;j++)
        {
            for(int l=0;l<=k;l++)
            {
                if(l>0)
                {
                    dp[i][j][l]=(dp[i][j][l]+(j>=(i))?dp[i-1][j-i][l-1]:dp[i-1][((j-i)%m+m)%m][l-1])%mod;
                }
                dp[i][j][l]=(dp[i][j][l]+dp[i-1][j][l])%mod;
            }
        }
    }
    
   cout<<(dp[n][0][k])<<endl;

    return 0;
}