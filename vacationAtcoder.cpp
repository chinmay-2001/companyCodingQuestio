#include <bits/stdc++.h>
using namespace std;

int main()
{
    int n;
    cin >> n;
    vector<vector<int>> cost;
    for (int i = 0; i < n; i++)
    {
        int a, b, c;
        cin >> a >> b >> c;
        cost.push_back({a, b, c});
    }

    vector<vector<int>> dp(n + 1, vector<int>(4, 0));

    for (int i = 1; i <= 3; i++)
    {
        dp[1][i] = cost[0][i - 1];
    }

    for (int i = 2; i <= n; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            int c = cost[i - 1][j - 1];
            for (int k = 1; k <= 3; k++)
            {
                if (k != j)
                {
                    dp[i][j] = max(dp[i][j], c + dp[i - 1][k]);
                }
            }
        }
    }

    int ans = 0;
    for (int i = 1; i <= 3; i++)
    {
        ans = max(ans, dp[n][i]);
    }
    cout << ans << endl;
}