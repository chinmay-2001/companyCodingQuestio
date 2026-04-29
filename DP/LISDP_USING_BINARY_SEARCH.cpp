// https://www.youtube.com/watch?v=on2hvxBXJH4
#include <bits/stdc++.h>

int LIS(vector<int> v)
{
    vector<int> seq;
    for (int i = 0; i < v.size(); i++)
    {
        int num = v[i];

        int idx = lower_bound(seq.begin(), seq.end(), num) - seq.begin();
        if (idx == seq.size())
        {
            seq.push_back(num);
        }
        else
        {
            seq[idx] = num;
        }
    }
    return seq.size();
}