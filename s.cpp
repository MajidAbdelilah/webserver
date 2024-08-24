#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main() {

    cin.tie(0);
    ios_base::sync_with_stdio(false);
    int n; cin >> n;
    
    vector<int> A(n);
    for (int i = 0; i < n; ++i) cin >> A[i];
    int q;
    cin >> q;
    vector<int> queries(q);
    for (int i = 0; i < q; ++i)  cin >> queries[i];
    for (int &ki : queries) {
        bool found = binary_search(A.begin(), A.end(), ki);
        cout << (found ? 1 : 0) << endl;
    }
}
