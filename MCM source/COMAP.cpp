#include<bits/stdc++.h>
#include<Windows.h>
using namespace std;

#define V 86
#define MAX_LENGTH 100
#define Ultimate_Destination 0

int P[V],capacity[V];
int edge[V][V],extra[V][V];

void get_capacity(){
    ifstream cin("rooms.csv");

    cin.ignore(MAX_LENGTH,'\n');
    capacity[0]=INT_MAX;
    for (int i=1; i<V; i++){
        cin>>capacity[i];
    }

    cin.close();
}

void get_position(int &total){
    ifstream cin("visitors' position.csv");

    cin.ignore(MAX_LENGTH,':');
    cin>>total;
    for (int i=1; i<=total; i++){
        int pos; cin>>pos;
        P[pos]++;
    }

    cin.close();
}

void get_edge(){
    ifstream cin("passages' condition.csv");

    cin.ignore(MAX_LENGTH,':');
    int total; cin>>total;
    for (int i=1; i<=total; i++){
        int u,v,MPPS;
        cin>>u,cin.ignore(MAX_LENGTH,','),cin>>v,cin.ignore(MAX_LENGTH,','),cin>>MPPS;
        edge[u][v]=edge[v][u]=MPPS;
    }

    cin.close();
}

void get_extra(){
    ifstream cin("extra passages.csv");

    cin.ignore(MAX_LENGTH,':');
    int total; cin>>total;
    for (int i=1; i<=total; i++){
        int u,v,MPPS;
        cin>>u,cin.ignore(MAX_LENGTH,','),cin>>v,cin.ignore(MAX_LENGTH,','),cin>>MPPS;
        extra[u][v]=extra[v][u]=MPPS;
    }

}

int dist[V],pre[V];

void BellmanFord(){
    bool in_queue[V];
    queue<int> q;

    memset(dist,0x3f,sizeof(dist));
    q.push(0),in_queue[0]=true; dist[0]=0;
    while (!q.empty()){
        int u=q.front(),v;
        for (int i=0; i<V; i++){
            if (edge[u][i]==0) continue;
            v=i;
            if (dist[u]+P[v]<dist[v]){
                dist[v]=dist[u]+P[v];
                pre[v]=u;
                if (!in_queue[v]){
                    q.push(v);
                    in_queue[v]=true;
                }
            }
        }
        q.pop(),in_queue[u]=false;
    }
}

int last[V];
vector<int> next[V];

bool CA_move(int u,int TIME){
    vector<int> in_rooms;
    for (int i=0; i<next[u].size(); i++){
        int v=next[u][i];
        if (P[v]) in_rooms.push_back(v);
    }

    int i=0;
    int flow[V]={0};
    while (P[u]<capacity[u]&&in_rooms.size()>0){
        int v=in_rooms[i];

        P[u]++,P[v]--;
        flow[v]++;
        if (P[v]==0 || flow[v]==edge[u][v]) {
            in_rooms.erase(in_rooms.begin()+i);
            if (in_rooms.size()==0) break;
            i%=in_rooms.size();
        }
        else i=(i+1)%in_rooms.size();
    }

    bool ret=P[u]>0 && u!=0;
    for (int i=0; i<next[u].size(); i++){
        int v=next[u][i];
        ret=CA_move(v,TIME) || ret;
    }
    if (!ret && !last[u]) last[u]=TIME;
    return ret;
}
void calc(){
    for (int i=1; i<V; i++){
        next[pre[i]].push_back(i);
    }

    memset(last,0,sizeof(last));
    for (int i=1; ; i++){
//        for (int j=0; j<V; j++) cout<<P[j]<<endl;
//        Sleep(200);
        if (CA_move(Ultimate_Destination,i)==false) break;
    }
}

int p_route[V];
int earlist_t[V];

void personnel(){//using Bellman-Ford Algorithm
    bool in_queue[V];
    queue<int> q;

    memset(earlist_t,0x3f,sizeof(earlist_t));
    q.push(0),in_queue[0]=true; earlist_t[0]=0;
    while (!q.empty()){
        int u=q.front(),v;
        for (int i=0; i<V; i++){
            if (edge[u][i]==0&&extra[u][i]==0) continue;
            v=i;
            if (max(earlist_t[u],last[v])<earlist_t[v]){
                earlist_t[v]=max(earlist_t[u],last[v]);
                p_route[v]=u;
                if (!in_queue[v]){
                    q.push(v);
                    in_queue[v]=true;
                }
            }
        }
        q.pop(),in_queue[u]=false;
    }
}
void print(){
    ofstream cout("evacuation route.txt");
    for (int i=1; i<V; i++) cout<<i<<" -> "<<pre[i]<<endl;
    cout.close();

    ofstream tout("time consuming.txt");
    for (int i=1; i<V; i++) tout<<last[i]<<endl;
    tout.close();

    ifstream cin("accident locations.csv");
    int x;
    vector<int> pos;
    while (cin>>x) pos.push_back(x);
    cin.close();

    ofstream pout("personnel route.txt");
    for (int i=0; i<pos.size(); i++){
        stack<int> st;
        for (int j=pos[i]; j!=0; j=p_route[j]) st.push(j);
        pout<<"To room "<<pos[i]<<": ";
        while (!st.empty()) pout<<st.top()<<" -> " , st.pop();
        pout<<"accident location\n";
        pout<<"It takes "<<earlist_t[pos[i]]<<" minutes.\n";
    }
    pout.close();
}
int main()
{
    int total;
    get_capacity();
    get_position(total);
    get_edge();
    get_extra();


    BellmanFord();
    calc();
    personnel();
    print();

    return 0;
}
