#include <iostream>
#include <cmath>
using namespace std;

int main()
{
    // Histogram from the exam
    int hist[12] = { 2, 4, 3, 1, 3, 6, 4, 3, 1, 0, 3, 2 };

    int cdf[12] = { 0 };
    cdf[0] = hist[0];

    // Step 1: compute cumulative distribution
    for (int i = 1; i < 12; i++)
    {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    int N = cdf[11];      // total pixels = 32
    int L = 12;           // number of gray levels
    int cdf_min = 0;

    // Step 2: find first non-zero CDF
    for (int i = 0; i < 12; i++)
    {
        if (cdf[i] != 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    cout << "Histogram values:\n";
    for (int i = 0; i < 12; i++)
        cout << hist[i] << " ";
    cout << "\n\n";

    cout << "CDF values:\n";
    for (int i = 0; i < 12; i++)
        cout << cdf[i] << " ";
    cout << "\n\n";

    cout << "Using normalized formula:\n";
    cout << "s_k = ((CDF(k) - CDF_min) / (N - CDF_min)) * (L - 1)\n";
    cout << "N = " << N << ", L = " << L << ", CDF_min = " << cdf_min << "\n\n";

    cout << "w\tCDF(w)\tw_baru\n";

    for (int w = 0; w < 12; w++)
    {
        float s = ((float)(cdf[w] - cdf_min) / (N - cdf_min)) * (L - 1);

        if (s < 0) s = 0;   // safety
        int w_baru = (int)round(s);

        cout << (w + 1) << "\t" << cdf[w] << "\t" << w_baru << "\n";
    }

    return 0;
}