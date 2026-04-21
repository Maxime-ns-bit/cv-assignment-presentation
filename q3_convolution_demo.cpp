#include <iostream>
using namespace std;

int main()
{
    int H[3][3] = {
        {1,1,1},
        {1,4,1},
        {1,1,1}
    };

    int X[4][4] = {
        {1,0,0,0},
        {1,1,1,0},
        {1,1,1,0},
        {1,0,0,0}
    };

    // Position (2,3) using 1-based indexing
    int window[3][3] = {
        {X[0][1], X[0][2], X[0][3]},
        {X[1][1], X[1][2], X[1][3]},
        {X[2][1], X[2][2], X[2][3]}
    };

    int Y23 = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Y23 += H[i][j] * window[i][j];
        }
    }

    cout << "Y(2,3) = " << Y23 << endl; // Expected 7

    return 0;
}