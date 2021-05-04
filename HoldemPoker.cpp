// Texas Holdem poker game with N players. Each player can CHECK, BET,
// FOLD, CALL or RAISE.
// This routine will print all possible permutations of games with the
// above actions and give the final pot value.
// Assumptions:
//      At some point raises will no longer be allowed. 
//      Initial BETs are 10 chips.
//      RAISEs are 20 chips.
//      Players only put chips in the pot when required to do so. 

// This task was deceptively challenging. But the challenge is what
// made it fun.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>

using namespace std;

constexpr int NUM_PLAYERS = 3;
constexpr int BET_AMOUNT = 10;
constexpr int RAISE_AMOUNT = 20;
enum class ACTION : char { BET = 'b', CHECK, CALL, FOLD, RAISE };

class Action {
    public:
    Action (ACTION ac, int val=0) : m_a(ac), m_val(val) {}
    ACTION GetAction() const { return m_a; }
    int GetValue()     const { return ( m_val); }
    friend ostream & operator<<(ostream &out, const Action &ac);

    private:
        ACTION m_a;
        int m_val;
};

// Assume a value for BET and RAISE but not for CHECK, CALL, and FOLD.
// I doubt other players would mind a bit if extra money is thrown into
// the pot.
ostream & operator<<(ostream &out, const Action &ac) {
    if(ACTION::CHECK == ac.m_a) out << "CHECK";
    else if(ACTION::CALL == ac.m_a) out << "CALL";
    else if(ACTION::FOLD == ac.m_a) out << "FOLD";
    else if(ACTION::BET == ac.m_a) out << "BET-" << ac.m_val;
    else if(ACTION::RAISE == ac.m_a) out << "RAISE-" << ac.m_val;
    return out;
}

// I had so much trouble keeping track of pointers I just decided to
// narrow to 1. Once it started working I didn't feel like messing
// with it.
template <class T>
class CircularBuffer1P {
    public:
      explicit CircularBuffer1P(int n) : m_n(n), m_wpos(0) {
          m_vec.reserve(n); // It won't get any bigger
      }

      void Write(T const &t) {
          if(m_vec.size() < m_n) {
              m_vec.push_back(t);
          }
          else {
              m_vec[m_wpos] = t;
          }
          increment(m_wpos);
      }

      T Read() const {
          if(m_vec.empty()) throw runtime_error("CircularBuffer is empty");
          auto t = m_vec.at(m_wpos);
          increment(m_wpos);
          return t;
      }

    T Peek() const {
        if(m_vec.empty()) throw runtime_error("CircularBuffer is empty");
        return m_vec.at(m_wpos);
    }

    private:
      void increment(int &pos) const {
          ++pos;
          if(pos >= m_n) pos = 0;
      }
      vector<T> m_vec;
      int m_n;
      mutable int m_wpos;
};


void SetObligations(int val, CircularBuffer1P<int> &cb) {
    // Go around the loop 1 time.
    // Assume current player's obligation was Peeked so
    // set it to 0;
    cb.Write(0);
    for(int i=0; i < NUM_PLAYERS - 1; ++i) {
        if(cb.Peek() == -1) {
            // This player folded so ignore but advance cb.
            cb.Read();
        }
        else {
            cb.Write(val + cb.Peek());
        }
    }

    // Advance cb 1 more time to be ready for the
    // next player.
    cb.Read();
}

int GetAllValues(const vector<Action> & actv) {
    int rtn = 0;

    // The idea of cb is to set and get obligations that
    // players must meet in order to stay in the game.
    // Once a player FOLDs that player will have no more
    // obligations and thus be marked with -1.
    CircularBuffer1P<int> cb(NUM_PLAYERS);
    for (int inx=0; inx<NUM_PLAYERS; ++inx) {
        cb.Write(0);
    }
    int callVal = 0;
    for(auto ac : actv) {
        int thisVal = ac.GetValue();
        if(cb.Peek() == -1) {
            // This player folded so ignore but advance cb.
            cb.Read();
        }
        if(ac.GetAction() == ACTION::FOLD) {
            // Set and forget .. and increment cb.
            cb.Write(-1);
            // Presumably thisVal == 0.
        }
        else if(ac.GetAction() == ACTION::BET || ac.GetAction() == ACTION::RAISE) {
            // Get obligation.
            rtn += cb.Peek();
            // This call will increment cb.
            SetObligations(thisVal, cb);
            rtn += thisVal;
        }
        else if(ac.GetAction() == ACTION::CALL) {
            // Get obligation.
            rtn += cb.Peek();
            // Mark obligation as met and increment cb.
            cb.Write(0);
            // Presumably thisVal == 0.
        }
        else {
            // Only RAISE is left. No need for any action
            // with cb.
            // Presumably thisVal == 0.
        }
    }
    return rtn;
}

// GetAllValues and PrintVector are only called when
// printing output line.
void PrintVector(const vector<Action> & actv) {
    // cd represents players. As actv is iterated thru cb
    // is looped. When FOLD is encountered then that player
    // is marked as -1 and it will be skipped in the future.
    CircularBuffer1P<int> cb(NUM_PLAYERS);
    for (int inx=1; inx <= NUM_PLAYERS; ++inx) {
        cb.Write(inx);
    }
    cout << "{ ";
    if(actv.size() > 0) {
        cout << "P" << cb.Read() << "#" << actv.at(0);
        for( int inx=1; inx < actv.size(); ++inx) {
            int player = -1;
            bool currentIsFold =
                (actv.at(inx).GetAction() == ACTION::FOLD);
            do {
                if(currentIsFold) {
                    player = cb.Peek();
                    cb.Write(-1);
                    // Following exits loop for multiple
                    // consecutive FOLDs.
                    currentIsFold = false;
                }
                else {
                    player = cb.Read();
                }
            } while(player < 0 );
            cout << ",P" << player << "#" << actv.at(inx);
        }
    }
    cout << " }";
}


bool IsExitCondition(const vector<Action> &actv) {
    bool rtn = false;

    // Need number of FOLDS and
    // (CALLs since last BET or RAISE).
    int callsSince = 0;
    bool countCalls = true;
    int foldCount = 0;
    for(auto it = actv.rbegin(); it != actv.rend(); ++it) {
        if(it->GetAction() == ACTION::FOLD) ++foldCount;
        if(countCalls && it->GetAction() == ACTION::CALL) {
            ++callsSince;
        }

        if(NUM_PLAYERS - 1 == foldCount + callsSince) {
            rtn = true;
            break;
        }

        if(it->GetAction() == ACTION::BET
                   || it->GetAction() == ACTION::RAISE) {
            countCalls = false;
        }
    }

    if(false == rtn) {
// This was in my first implementation. std::count_if() and lambda
// functions are great but we must know when those should NOT be used.
// This version was inefficient because it will iterate over entire
// vector each time. My improvement operates in constant time rather
// than O(N) time.
    // int checkCount = std::count_if(actv.begin(), actv.end(),
    // [](const Action &ac){
    //     return ACTION::CHECK == ac.GetAction();
    // });

        // Check for the special case of all CHECKS meaning that
        // no one was willing to open.
        rtn = (actv.size() == NUM_PLAYERS &&
               actv.at(actv.size() - 1).GetAction() == ACTION::CHECK);
    }

    return rtn;
}

void NextAction(Action ac, vector<Action> &actv,
                int pot, int &inx ) {
    actv.push_back(ac);
    if (IsExitCondition( actv)) {
        cout << inx++ << ". ";
        PrintVector(actv);
        cout << " and pot = " << pot + GetAllValues(actv) << endl;
    }
    else {
        if(ACTION::CHECK == ac.GetAction()) {
            NextAction(Action(ACTION::CHECK), actv, pot, inx);
            NextAction(Action(ACTION::BET, BET_AMOUNT), actv,
                       pot, inx);
        }
        else {
            NextAction(Action(ACTION::CALL), actv, pot, inx);
            NextAction(Action(ACTION::FOLD), actv, pot, inx);
            // Limiting factor
            if(GetAllValues(actv) < pot ) {
                NextAction(Action(ACTION::RAISE, RAISE_AMOUNT),
                                  actv, pot, inx);
            }
        }
    }
    actv.pop_back();
}

int main()
{
    int pot = 60;
    int inx = 1;  
    vector<Action> actv;
    cout << "Running Texas Holdem poker game with " << NUM_PLAYERS
         << " players" << endl;
    if(NUM_PLAYERS >= 3) {
        // Inital actions are CHECK or BET.
        NextAction(Action(ACTION::CHECK), actv, pot, inx);
        NextAction(Action(ACTION::BET, BET_AMOUNT), actv, pot, inx);
    }
    else {
        cout << "Not enough players. Invite some friends." << endl;
    }
    return 0;
}
