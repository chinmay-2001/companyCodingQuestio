    #include <bits/stdc++.h>
    using namespace std;
    int main() {
        int t;
        cin>>t;
        while(t--){
            // cout<<"t"<<t<<endl;
            int n,k;
            cin>>n>>k;
            vector<int> ans;
            if(n==1 || k==1)cout<<-1<<endl;
            else if(k==2 and n%2==1){
                cout<<-1<<endl;
            }
            else{
                vector<int> temp;
                int r=(n/k)*k;
                for(int i=1;i<=r;i++){
                if(i%k!=0)temp.push_back(i);
                else{
                    ans.push_back(i);
                    for(auto p:temp)ans.push_back(p);
                    temp.clear();
                }
                }

                if(n%k==1){
                    ans.push_back(n);
                    swap(ans[n-2],ans[n-1]);
                }else if(n%k>1){
                    ans.push_back(n);
                    for(int i=r+1;i<n;i++){
                        ans.push_back(i);
                    }
                }
                
                for(auto p:ans){
                    cout<<p<<" ";
                }
                cout<<endl;
            }
        }
    }