\
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <tuple>
#include <limits>
#include <cctype>
#include <algorithm>

using namespace std;

// ---------------------------
// Constants & Type Definitions
// ---------------------------
const int INF = 1e9;
const int CHILD_MAX_AGE = 12;
const int ELDER_MIN_AGE = 60;

struct Person {
    string name;
    int age;
    char gender; // 'M' or 'F'
    string city;
};

// ---------------------------
// Beneficiary Prioritization
// ---------------------------
int ageGroup(const Person& p) {
    if (p.age <= CHILD_MAX_AGE) return 0; // Children first
    if (p.age >= ELDER_MIN_AGE) return 1; // Then elders
    return 2;                              // Then adults
}

// Higher priority returns true if a should come before b
bool higherPriority(const Person& a, const Person& b) {
    int groupA = ageGroup(a);
    int groupB = ageGroup(b);
    if (groupA != groupB) return groupA < groupB;

    char genderA = toupper(a.gender);
    char genderB = toupper(b.gender);
    if (genderA != genderB) return genderA == 'F'; // Females first

    // Ties within same group
    if (groupA == 0) { // children: younger first
        if (a.age != b.age) return a.age < b.age;
    } else { // elders/adults: older first
        if (a.age != b.age) return a.age > b.age;
    }

    // Finally, alphabetical by name
    return a.name < b.name;
}

// Stable merge sort by priority
void mergeRange(vector<Person>& arr, int l, int m, int r, vector<Person>& tmp) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) {
        if (higherPriority(arr[i], arr[j])) tmp[k++] = arr[i++];
        else tmp[k++] = arr[j++];
    }
    while (i <= m) tmp[k++] = arr[i++];
    while (j <= r) tmp[k++] = arr[j++];
    for (int t = l; t <= r; ++t) arr[t] = tmp[t];
}

void mergeSortRec(vector<Person>& arr, int l, int r, vector<Person>& tmp) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortRec(arr, l, m, tmp);
    mergeSortRec(arr, m + 1, r, tmp);
    mergeRange(arr, l, m, r, tmp);
}

void mergeSort(vector<Person>& arr) {
    if (arr.empty()) return;
    vector<Person> tmp(arr.size());
    mergeSortRec(arr, 0, (int)arr.size() - 1, tmp);
}

// ---------------------------
– Graph Representation
// ---------------------------
class Graph {
public:
    int vertices;
    unordered_map<string, int> cityToIndex;
    unordered_map<int, string> indexToCity;
    vector<vector<pair<int, int>>> adjacent_list;
    vector<tuple<int, int, int>> edges;

    explicit Graph(int V) : vertices(V) {
        adjacent_list.resize(V);
    }

    void setCity(const string& name, int index) {
        cityToIndex[name] = index;
        indexToCity[index] = name;
    }

    void setEdge(const string& from, const string& to, int weight) {
        int start = cityToIndex[from];
        int finish = cityToIndex[to];
        adjacent_list[start].push_back({finish, weight});
        edges.emplace_back(start, finish, weight);
    }
};

// ---------------------------
// Shortest Path Algorithms
// ---------------------------
pair<vector<int>, vector<int>> dijkstras_algorithm(Graph& graph, int source) {
    vector<int> distance(graph.vertices, INF);
    vector<int> parent(graph.vertices, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap;

    distance[source] = 0;
    minHeap.push({0, source});

    while (!minHeap.empty()) {
        int u_dist = minHeap.top().first;
        int u = minHeap.top().second;
        minHeap.pop();
        if (u_dist > distance[u]) continue;

        for (auto& edge : graph.adjacent_list[u]) {
            int v = edge.first;
            int weight = edge.second;
            if (distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                parent[v] = u;
                minHeap.push({distance[v], v});
            }
        }
    }
    return {distance, parent};
}

pair<vector<int>, vector<int>> bellmanFord_algorithm(Graph& graph, int source) {
    vector<int> distance(graph.vertices, INF);
    vector<int> parent(graph.vertices, -1);
    distance[source] = 0;

    for (int i = 0; i < graph.vertices - 1; i++) {
        for (const auto& edge : graph.edges) {
            int u = std::get<0>(edge);
            int v = std::get<1>(edge);
            int w = std::get<2>(edge);
            if (distance[u] != INF && distance[u] + w < distance[v]) {
                distance[v] = distance[u] + w;
                parent[v] = u;
            }
        }
    }
    return {distance, parent};
}

pair<vector<int>, vector<int>> floydWarshall_singleSource(Graph& graph, int source) {
    int V = graph.vertices;
    vector<vector<int>> dist(V, vector<int>(V, INF));
    vector<vector<int>> next(V, vector<int>(V, -1));

    for (int i = 0; i < V; ++i) dist[i][i] = 0;
    for (const auto& edge : graph.edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int w = std::get<2>(edge);
        dist[u][v] = w;
        next[u][v] = v;
    }

    for (int k = 0; k < V; ++k) {
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    vector<int> parent(V, -1);
    for (int j = 0; j < V; ++j) {
        if (j == source || dist[source][j] == INF) continue;
        int curr = source;
        while (next[curr][j] != j && next[curr][j] != -1) {
            curr = next[curr][j];
        }
        parent[j] = curr;
    }
    return {dist[source], parent};
}

// ---------------------------
// Printing Helpers
// ---------------------------
void print_path(const vector<int>& parent, int j, const Graph& graph) {
    if (parent[j] == -1) {
        cout << graph.indexToCity.at(j);
        return;
    }
    print_path(parent, parent[j], graph);
    cout << " -> " << graph.indexToCity.at(j);
}

void print_path_distance(const pair<vector<int>, vector<int>>& result, int source, int destination, const Graph& graph) {
    const vector<int>& distance = result.first;
    const vector<int>& parent = result.second;

    if (distance.empty()) {
        cout << "Algorithm did not return a result (e.g., negative cycle detected).\n";
        return;
    }
    if (distance[destination] == INF) {
        cout << "No path exists from " << graph.indexToCity.at(source) << " to " << graph.indexToCity.at(destination) << ".\n";
    } else {
        cout << "Shortest distance: " << distance[destination] << "\n";
        cout << "Path: ";
        print_path(parent, destination, graph);
        cout << "\n";
    }
}

// ---------------------------
// Input / Output
// ---------------------------
vector<Person> inputBeneficiaries() {
    int n;
    cout << "Enter number of flood-affected people to register: ";
    cin >> n;

    vector<Person> people;
    people.reserve(n);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < n; ++i) {
        Person p;
        cout << "\n--- Person " << (i + 1) << " ---\n";

        cout << "Name: ";
        getline(cin, p.name);

        cout << "Age: ";
        while (!(cin >> p.age) || p.age < 0) {
            cout << "Invalid input. Please enter a valid non-negative age: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cout << "Gender (M/F): ";
        cin >> p.gender;
        p.gender = toupper(p.gender);
        while (p.gender != 'M' && p.gender != 'F') {
            cout << "Invalid input. Please enter M or F: ";
            cin >> p.gender;
            p.gender = toupper(p.gender);
        }

        cout << "Living place (city): ";
        cin >> ws;
        getline(cin, p.city);

        people.push_back(p);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return people;
}

void printBeneficiaries(const vector<Person>& ppl) {
    cout << "\n--- Prioritized List of Beneficiaries (Sorted) ---\n";
    cout << "Rank | Name                 | Age | Gender | City\n";
    cout << "-----------------------------------------------------\n";
    for (int i = 0; i < (int)ppl.size(); ++i) {
        cout.width(4);
        cout << left << i + 1 << " | ";
        cout.width(20);
        cout << left << ppl[i].name << " | ";
        cout.width(3);
        cout << left << ppl[i].age << " | ";
        cout.width(6);
        cout << left << ppl[i].gender << " | ";
        cout << ppl[i].city << "\n";
    }
    cout << "-----------------------------------------------------\n";
    cout << "Time Complexity for Merge Sort: O(n log n)\n\n";
}

// ---------------------------
// Main
// ---------------------------
int main() {
    // 1) Input & sort beneficiaries
    vector<Person> people = inputBeneficiaries();
    mergeSort(people);
    printBeneficiaries(people);

    // 2) Build city graph
    int vertices, numEdges;
    cout << "--- City and Road Network Setup ---\n";
    cout << "Enter the number of cities: ";
    cin >> vertices;

    Graph graph(vertices);

    cout << "Enter the name of the " << vertices << " cities (one per line):\n";
    for (int i = 0; i < vertices; i++) {
        string name;
        cin >> name;
        graph.setCity(name, i);
    }

    cout << "Enter the number of roads: ";
    cin >> numEdges;
    cout << "Enter the connections (format: city1 city2 distance) on each line:\n";
    for (int i = 0; i < numEdges; i++) {
        string city1, city2;
        int distance;
        cin >> city1 >> city2 >> distance;
        if (distance < 0) {
            cout << "Negative distance is not allowed. Please try again.\n";
            --i;
            continue;
        }
        if (!graph.cityToIndex.count(city1) || !graph.cityToIndex.count(city2)) {
            cout << "Unknown city name. Please enter valid cities.\n";
            --i;
            continue;
        }
        graph.setEdge(city1, city2, distance);
    }

    // 3) Run shortest paths for a source→destination
    string source, destination;
    cout << "\nEnter a source city for pathfinding: ";
    cin >> source;
    cout << "Enter a destination city for pathfinding: ";
    cin >> destination;

    if (graph.cityToIndex.find(source) == graph.cityToIndex.end() ||
        graph.cityToIndex.find(destination) == graph.cityToIndex.end()) {
        cout << "Error: Source or destination city not found in the graph.\n";
        return 1;
    }

    int from = graph.cityToIndex[source];
    int to = graph.cityToIndex[destination];

    cout << "\n--- Shortest Path Results (" << source << " to " << destination << ") ---\n";

    cout << "\nDijkstra's Algorithm:\n";
    print_path_distance(dijkstras_algorithm(graph, from), from, to, graph);
    cout << "Time Complexity: O(E log V)\n";

    cout << "\nBellman-Ford Algorithm:\n";
    print_path_distance(bellmanFord_algorithm(graph, from), from, to, graph);
    cout << "Time Complexity: O(V * E)\n";

    cout << "\nFloyd-Warshall Algorithm:\n";
    print_path_distance(floydWarshall_singleSource(graph, from), from, to, graph);
    cout << "Time Complexity: O(V^3)\n";

    // 4) Route to each beneficiary city from the chosen source (via Dijkstra)
    auto shortestPathsResult = dijkstras_algorithm(graph, from);
    const vector<int>& distanceVec = shortestPathsResult.first;
    const vector<int>& parentVec = shortestPathsResult.second;

    cout << "\n--- Routes from " << source << " to each Beneficiary ---\n";
    cout << "Beneficiary Name      | Route Information\n";
    cout << "--------------------------------------------------------\n";
    for (const auto& p : people) {
        cout.width(21);
        cout << left << p.name << " | ";
        if (graph.cityToIndex.find(p.city) == graph.cityToIndex.end()) {
            cout << "City '" << p.city << "' not found in the graph.\n";
            continue;
        }
        int cityIdx = graph.cityToIndex[p.city];
        if (distanceVec[cityIdx] == INF) {
            cout << "No path exists to " << p.city << ".\n";
        } else {
            cout << "Distance: " << distanceVec[cityIdx] << ", Path: ";
            print_path(parentVec, cityIdx, graph);
            cout << "\n";
        }
    }
    cout << "--------------------------------------------------------\n";
    return 0;
}
