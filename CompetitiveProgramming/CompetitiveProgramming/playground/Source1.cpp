#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <memory>
#include <queue>
#include <limits.h>
#include <cassert>

using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
typedef long long ll;
#define UL_MAX 0xffffffff
#define ULL_MAX 0xffffffffffffffff
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x
#define rep(i, n) for(ull i = 1; i <= n; i++)
#define rep0(i, n) for(ull i = 0; i < n; i++)
#define repi(i, n) for(ull i = n; i > 0; i--)
#define repi0(i, n) for(ll i = n - 1; i >= 0; i--)

namespace test5
{
    string exDconvertToNewString(const string& s) {
        string newString = "@";

        for (int i = 0; i < s.size(); i++) {
            newString += "#" + s.substr(i, 1);
        }

        newString += "#$";
        return newString;
    }

    // given a string s, find the longest string t such that t=a+b, where a is 
    // a prefix of s and b is a suffix of s, and t is a palindrome.
    string exDLongestSplitPalindrome(const string& s) {
        string Q = exDconvertToNewString(s);
        vector<int> P(Q.size(), 0);
        int c = 0, r = 0;   // current center, right limit

        for (int i = 1; i < Q.size() - 1; i++) {
            // find the corresponding letter in the palidrome subString
            int iMirror = c - (i - c);

            if (r > i) {
                P[i] = min(r - i, P[iMirror]);
            }

            // expanding around center i
            while (Q[i + 1 + P[i]] == Q[i - 1 - P[i]]) {
                P[i]++;
            }

            // Update c,r in case if the palindrome centered at i expands past r,
            if (i + P[i] > r) {
                c = i;              // next center = i
                r = i + P[i];
            }
        }

        ul equalEnds = 0;
        for (int i = 1; i < (Q.size() - 1) / 2; i++)
        {
            if (Q[i] != Q[Q.size() - 1 - i])
                break;

            equalEnds++;
        }

        int biggestCompletingPalindrome = 0;
        int palindromeCenter = 0;
        // find the biggest palindrome that ends at N-equalEnds-2
        for (int i = 1; i < Q.size() - 1; i++) {

            if ((i + P[i] >= Q.size() - 1 - equalEnds && i < Q.size() - equalEnds - 1)
                || (i - P[i] <= equalEnds && i > equalEnds))
            {
                int intersection;
                // if palindrome is straight after prefix
                if (i - P[i] <= equalEnds)
                {
                    intersection = equalEnds - i + P[i];
                }
                else
                {
                    intersection = equalEnds - (Q.size() - 1 - i - P[i]);
                }

                int currentPalindromeSize = P[i] - intersection;
                if (currentPalindromeSize > biggestCompletingPalindrome)
                {
                    palindromeCenter = i;
                    biggestCompletingPalindrome = currentPalindromeSize;
                }
            }
        }

        string res = s.substr(0, equalEnds / 2);

        res += s.substr((palindromeCenter - 1 - biggestCompletingPalindrome) / 2,
            biggestCompletingPalindrome);

        res += s.substr(s.size() - equalEnds / 2, equalEnds / 2);

        return res;
    }

    void exD()
    {
        ul t;
        cin >> t;

        rep(i, t)
        {
            string s;
            cin >> s;

            string res = exDLongestSplitPalindrome(s);
            cout << res << endl;
        }
    }

}

int main()
{
    test5::exD();
    return 0;
}
