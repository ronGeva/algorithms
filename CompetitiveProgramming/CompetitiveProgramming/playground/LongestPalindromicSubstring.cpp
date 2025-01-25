#include "LongestPalindromicSubstring.h"
#include <vector>

string convertToNewString(const string& s) {
    string newString = "@";

    for (int i = 0; i < s.size(); i++) {
        newString += "#" + s.substr(i, 1);
    }

    newString += "#$";
    return newString;
}

string longestPalindromeSubstring(const string& s) {
    string Q = convertToNewString(s);
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

    // Find the longest palindrome length in p.

    int maxPalindrome = 0;
    int centerIndex = 0;

    for (int i = 1; i < Q.size() - 1; i++) {

        if (P[i] > maxPalindrome) {
            {
                maxPalindrome = P[i];
                centerIndex = i;
            }
        }
    }
    //indexs: 012345678910
    //Q:      @#a#b#a#a#$
    //P:      00103012100
    return s.substr((centerIndex - 1 - maxPalindrome) / 2, maxPalindrome);
}
