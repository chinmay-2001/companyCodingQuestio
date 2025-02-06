const int MAXN = 100000;
#define MOD 1000000007
#define ll long long
ll fact[MAXN + 1], ifact[MAXN + 1];

// Function to compute (base^exp) % MOD using Binary Exponentiation
ll power(ll base, ll exp, ll mod)
{
    ll result = 1;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

void precompute()
{
    fact[0] = ifact[0] = 1;
    for (int i = 1; i <= MAXN; i++)
    {
        fact[i] = (fact[i - 1] * i) % MOD;
    }
    ifact[MAXN] = power(fact[MAXN], MOD - 2, MOD); // Fermat's theorem
    for (int i = MAXN - 1; i >= 1; i--)
    {
        ifact[i] = (ifact[i + 1] * (i + 1)) % MOD;
    }
}

ll nCr(int n, int r)
{
    if (r > n || r < 0)
        return 0;
    return (fact[n] * ifact[r] % MOD) * ifact[n - r] % MOD;
}

/* clang-format on */