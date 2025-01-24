#include <bits/stdc++.h>
using namespace std;
int visited[100000];
int bfs(map<int,int> adj,int node){
    queue<int> que;
    que.push(node);
    visited[node]=1;
    int cnt=1;
    while(!que.empty()){
        int v=que.front();
        que.pop();
        if(visited[adj[v]]==0){
            que.push(adj[v]);
            visited[adj[v]]=1;
            cnt++;
        }
    }
    return cnt;
}
long long calculateLCM(long long n1,long long n2){
    long long lcm=(n1/__gcd(n1,n2))*n2;
    return lcm;
}

int main(){
    int t;
    cin>>t;
    while(t--){
        fill(visited,visited+100000,0);
        int n;
        cin>>n;
        // cout<<"n:"<<n<<endl;
        vector<int> nums(n);
        for(int i=0;i<n;i++)cin>>nums[i];
        
        map<int,int> adj;
        for(int i=0;i<n;i++){
            adj[i+1]=nums[i];
        }
        vector<int> numbers;
        for(auto p:adj){
            if(visited[p.first]==0){
                int num=bfs(adj,p.first);
                numbers.push_back(num);
            }
        }

        if(numbers.size()==1){
            cout<<n<<endl;
        }else{
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

