#include <bits/stdc++.h>
using namespace std;

const int Penalty = 10;

void modifiedDijkstra(int source, int destination, const vector<vector<pair<int,int>>>& adjList, 
                      vector<int>& distances, vector<int>& parents, vector<int>& visitingFrequency) {
    int n = adjList.size();
    distances.assign(n, INT_MAX);
    parents.assign(n, -1);
    distances[source] = 0;

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        int cost = pq.top().first;
        int node = pq.top().second;
        pq.pop();

        if (cost > distances[node]) continue;

        for (const auto& edge : adjList[node]) {
            int neighbor = edge.first;
            int weight = edge.second;

            int newCost = distances[node] + weight + (Penalty * visitingFrequency[neighbor]);

            if (newCost < distances[neighbor]) {
                distances[neighbor] = newCost;
                parents[neighbor] = node;
                pq.push({newCost, neighbor});
            }
        }
    }
}

vector<int> getPath(int source, int destination, const vector<int>& parents, vector<int>& visitingFrequency) {
    vector<int> path;
    for (int at = destination; at != -1; at = parents[at]) {
        path.push_back(at);
        if(at != source){
            visitingFrequency[at]++;
        }
    }
    reverse(path.begin(), path.end());
    return path;
}

int main() {
    int n = 7; 
    vector<vector<pair<int, int>>> adjList(n);
    vector<int> visitingFrequency(n, 0);

    adjList[0].emplace_back(1, 2);
    adjList[0].emplace_back(2, 4);
    adjList[0].emplace_back(3, 5);
    
    adjList[1].emplace_back(0, 2);
    adjList[1].emplace_back(2, 1);
    adjList[1].emplace_back(4, 3);
    
    adjList[2].emplace_back(0, 4);
    adjList[2].emplace_back(1, 1);
    adjList[2].emplace_back(3, 2);
    adjList[2].emplace_back(5, 6);
    
    adjList[3].emplace_back(0, 5);
    adjList[3].emplace_back(2, 2);
    adjList[3].emplace_back(4, 7);
    adjList[3].emplace_back(6, 8);
    
    adjList[4].emplace_back(1, 3);
    adjList[4].emplace_back(3, 7);
    adjList[4].emplace_back(5, 4);
    
    adjList[5].emplace_back(2, 6);
    adjList[5].emplace_back(4, 4);
    adjList[5].emplace_back(6, 1);
    
    adjList[6].emplace_back(3, 8);
    adjList[6].emplace_back(5, 1);

    vector<vector<int>> adj(n,vector<int>(n,INT_MAX));
    for(int i=0;i<n;i++){
        for(auto& p : adjList[i]){
            int u = i;
            int v = p.first;
            int w = p.second;
            adj[u][v] = w;
            adj[v][u] = w;
        }
    }

    while (true) {
        int choice;
        cout<<"\n0. Visiting status till now \n1. Continue to find Path \n2. Edit node's past visiting stats \n3. Exit \nEnter your choice : ";
        cin>>choice;
        if(choice > 3 || choice < 0){
            cout<<"Enter a valid option.\n";
            continue;
        }
        cout<<endl;
        if(choice == 0){
            for(int i=0;i<n;i++){
                cout<<"Node "<<i<<" : "<<visitingFrequency[i]<<" times\n";
            }
            continue;
        }
        if(choice == 2){
            for(int i=0;i<n;i++){
                cout<<"The visit of Node "<<i<<" (present visit = "<<visitingFrequency[i]<<") : ";
                int visit;
                cin>>visit;
                visitingFrequency[i] = visit;
            }
            cout<<'\n';
            continue;
        }
        if(choice == 2) break;

        int source, destination;
        cout << "Enter source node (-1 to exit): ";
        cin >> source;
        if (source == -1) break;

        cout << "Enter destination node (-1 to exit): ";
        cin >> destination;
        if (destination == -1) break;

        if (source < 0 || source >= n || destination < 0 || destination >= n) {
            cout << "Invalid source or destination node. Please try again.\n";
            continue;
        }

        vector<int> distances, parents;
        modifiedDijkstra(source, destination, adjList, distances, parents, visitingFrequency);

        if (distances[destination] == INT_MAX) {
            cout << "\nNo path exists from " << source << " to " << destination << ".\n";
        } else {
            
            vector<int> path = getPath(source,destination, parents, visitingFrequency);
            int dis = 0;
            for(int i=0;i<path.size()-1;i++){
                dis += adj[path[i]][path[i+1]];
            }

            cout << "\nPath cost from " << source << " to " << destination << " is " << dis << ".\n";

            cout << "Path: ";
            for (int node : path) {
                cout << node << " ";
            }
            cout << "\n";
        }

        cout << "\nVisiting frequency of nodes:\n";
        for (int i = 0; i < n; i++) {
            cout << "Node " << i << ": " << visitingFrequency[i] << " times\n";
        }
        cout << endl;

        // Returning to source
        swap(source,destination);

        distances.clear(), parents.clear();
        modifiedDijkstra(source, destination, adjList, distances, parents, visitingFrequency);

        vector<int> path = getPath(source,destination, parents, visitingFrequency);
        int dis = 0;
        for(int i=0;i<path.size()-1;i++){
            dis += adj[path[i]][path[i+1]];
        }

        cout << "\nPath cost from " << source << " to " << destination << " is " << dis << ".\n";

        cout << "Path: ";
        for (int node : path) {
            cout << node << " ";
        }
        cout << "\n";
        

        cout << "\nVisiting frequency of nodes:\n";
        for (int i = 0; i < n; i++) {
            cout << "Node " << i << ": " << visitingFrequency[i] << " times\n";
        }
        cout << endl;
        
        // end
    }

    cout << "Program exited.\n";
    return 0;
}
