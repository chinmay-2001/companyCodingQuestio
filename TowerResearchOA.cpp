#include <bits/stdc++.h>
using namespace std;
void travel(vector<int>&nums,vector<int>& cycle,int i,int curr){
    if(cycle[i]!=0)return;
    cycle[i]=curr;
    travel(nums,cycle,nums[i]-1,curr);
}
long long calculateLCM(long long n1,long long n2){
    long long lcm=(n1/__gcd(n1,n2))*n2;
    return lcm;
}
int main(){
    int t;
    cin>>t;
    while(t--){
        int n;
        cin>>n;
        vector<int> nums(n);
        for(int i=0;i<n;i++)cin>>nums[i];
        vector<int> cycle(n+1,0);
        long long curr=0;
        int i=0;
        while(i<n){
            if(cycle[i]==0){
                curr++;
                cycle[i]=curr;
                travel(nums,cycle,nums[i]-1,curr);
            }
            i++;
        }
        map<int,long long> ma;
        for(int i=0;i<n;i++){
            ma[cycle[i]]++;
        }
        if(ma.size()==1){
            cout<<n<<endl;
        }else{
            vector<int> numbers;
            for(auto p:ma)numbers.push_back(p.second);
            long long n1=numbers[0];
            long long n2=numbers[1];
            long long lcm=calculateLCM(n1,n2);
            for(int i=2;i<numbers.size();i++){
                lcm=calculateLCM(lcm,numbers[i]);
            }
            cout<<lcm<<endl;
        }
    }
}

