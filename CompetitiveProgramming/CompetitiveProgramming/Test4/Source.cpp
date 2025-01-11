#include <iostream>
#include <vector>


using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x


ul binarySearch(vector<ull>& arr, ull num)
{
    ul low = 0;
    ul high = static_cast<ul>(arr.size());
    ul mid = -1;
    ul out = -1;
    double search = num - 0.5;

    while (low < high)
    {
        mid = (low + high) / 2;
        cout << "low: " << low << ", mid: " << mid << ", high: " << high << endl;
        /*if (static_cast<float>(arr[mid]) == search)
            return mid;*/

        if (static_cast<double>(arr[mid]) < search)
        {
            low = mid + 1;
            out = low;
        }
        else
        {
            high = mid;
            out = high;
        }
    }

    return out;
}


void test()
{
    vector<ull> A{2, 3, 4, 4, 7, 10};
    cout << binarySearch(A, 4) << endl;
}


int main()
{
    test();

    /*ul N;
    cin >> N;

    vector<ull> A(N, 0);
    for (ul i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    ull count = 0;
    for (ul i = 0; i < N; i++)
    {
        count += (static_cast<ull>(N) - binarySearch(A, A[i] * 2));
    }

    cout << count;*/
}