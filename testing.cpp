#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

bool isPerfectSquare(long long num)
{
    long long sqrtNum = sqrt(num);
    return sqrtNum * sqrtNum == num;
}

int main()
{
    vector<pair<int, long long>> squareSums;

    for (int n = 1; n <= 1000000; ++n)
    {
        long long sum = (n * (n + 1LL)) / 2; // Sum of first n natural numbers
        if (isPerfectSquare(sum))
        {
            squareSums.push_back({n, sum});
        }
    }

    for (const auto &pair : squareSums)
    {
        cout << "n = " << pair.first << ", sum = " << pair.second << endl;
    }

    return 0;
}
