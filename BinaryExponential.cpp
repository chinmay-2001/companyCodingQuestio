#include <bits/stdc++.h>
using namespace std;
#define MOD 1000000007
int pow(long long a,long long b){
    long long res=1;
    while(b>0){
        if(b&1)res=(res*a)%MOD;
        a=(a*a)%MOD;
        b/=2;
    }
    return res;
}
int multiply(int a,int b){
    int res=0;
    while(b>0){
        if(b&1)res+=a,res=res%MOD;
        a+=a;
        a=a%MOD;
        b/=2;
    }
    return res;
}
int main(){
    cout<<pow(2,1000000007)<<endl;
    // cout<<multiply(2,10)<<endl;
}