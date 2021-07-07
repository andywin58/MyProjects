// The task is to parse a numeric string into multiple
// lottery number candidates. All digits in the string must
// be used and they must be used in the order in which they
// appear in the string.
// Lottery numbers must be between 1 and 59 inclusive.
// Lottery number candidates are groups of 7 unique lottery
// numbers.
// Print the input string followed by the lottery number
// candidate. Each string could result in multiple or no
// lottery number candidates.
// Examples:
// 102345678: 10 2 3 4 56 7 8
// 102345678: 10 2 3 45 6 7 8
// 102345678: 10 2 34 5 6 7 8
// 102345678: 10 23 4 5 6 7 8
// 312345067: 3 1 2 34 50 6 7
// 312345067: 3 1 23 4 50 6 7
// 312345067: 31 2 3 4 50 6 7


#include <iostream>
#include <vector>

using namespace std;

constexpr int LOTTERY_NUM_SIZE = 7;

ostream & PrintOutput(ostream & os,
                    const string & str,
                    const vector<int> & nums) {
    os << str << ": ";
    for( auto n : nums ) {
        os << n << " ";
    }
    os << endl;

    return os;
}

bool IsNumAMember( int a,
                    const vector<int> & nums) {
    bool rtn = false;
    for( auto n : nums ) {
        if( n == a ) {
            rtn = true;
            break;
        }
    }

    return rtn;
}


void ParseCandidate(const string & str,
                    string::size_type & pos,
                    vector<int> & lotteryNum) {
    if (str.size() == pos && lotteryNum.size() == LOTTERY_NUM_SIZE) {
       PrintOutput(cout, str, lotteryNum);
    }
    else if(pos < str.size() &&
          lotteryNum.size() < LOTTERY_NUM_SIZE) {
        int num = -1;
        num = stoi(str.substr(pos, 1)); // single character
        if(num > 0 && ! IsNumAMember(num, lotteryNum)) {
            lotteryNum.push_back(num);
            ++pos;
            ParseCandidate(str, pos, lotteryNum);
            --pos;
            lotteryNum.pop_back();
        }

        if(pos < str.size() - 1) {
            // What about a '0' lead digit.
            // Specification doesn't seem clear.
            num = stoi(str.substr(pos, 2));
            // Assume num must be between 1 and 59, INCLUSIVE.
            if(num > 9 && num <= 59 && ! IsNumAMember(num, lotteryNum)) {
                lotteryNum.push_back(num);
                pos += 2;
                ParseCandidate(str, pos, lotteryNum);
                pos -= 2;
                lotteryNum.pop_back();
            }
        }
    }
}



bool IsValidInput(const string & str) {
    bool rtn = true;
    if(str.size() >= LOTTERY_NUM_SIZE &&
       str.size() <= LOTTERY_NUM_SIZE * 2) {
        for( char c : str) {
            if( !isdigit(c) ) {
                rtn = false;
                break;
            }
        }
    }
    else {
        rtn = false;
    }

    return rtn;
}


int main()
{
    cout << "Parse a numeric string for lottery numbers." << endl;
    string input;
    do {
        string::size_type pos = 0;
        vector<int> lotteryNum;
        cout << "Enter a numeric string between 7 and 14 digits in length: ";
        cin >> input;
        cout << endl;
        if( IsValidInput(input) )
            ParseCandidate(input, pos, lotteryNum);
        else
            input = "";
    }
    while( input.size() > 0 );
    cout << "Done" << endl;

    return 0;
}
