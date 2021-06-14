// This is a simple example that demostrates one of the many problems
// with multithreaded programing. Here's a good article that describes
// several other multithreading problems.
// https://www.acodersjourney.com/top-20-cplusplus-multithreading-mistakes/

// Note here that several threads are executing PalindromeCheck::isValid
// and trying to write to log at virtually the same time. Sometimes when I
// run this routine as is, the "Final result" looks fine. That's a deception
// of multithreaded programming. Even though a problem exists it may not
// show up all the time.

// The solution to the problem is simple. Just uncomment the line with
// lock_guard<mutex> lk(m). What this will do is allow 1 thread at a time
// to execute the following log statement. When the function ends the
// lock_guard goes out of scope thus unlocking the mutex and allowing the
// next thread in line to do it's logging.

#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <exception>
#include <future>
#include <mutex>

using namespace std;

class PalindromeCheck {
    mutex m;
    ostringstream log;

    public:

    bool isValid(const string & s) {
        bool rtn = true;
        string::size_type enx = s.size() - 1;
        for(int bnx = 0; bnx <= s.size() / 2; ++bnx) {
            if(s.at(bnx) != s.at(enx)) {
               rtn = false;
               break;
            }
            --enx;
        }

        //lock_guard<mutex> lk(m);
        log << "Thread " << this_thread::get_id() << ": " << s << (rtn ? " IS" : " is NOT")
            << " a palindrome\n";
    }

    string getLog() {
        return log.str();
    }
};


main() {
    PalindromeCheck pc;
 //   async(launch::async, &PalindromeCheck::isValid, &pc, "111111");
    thread t1(&PalindromeCheck::isValid, &pc, "11111111111111111111111111111111111111111111111111111");
    thread t2(&PalindromeCheck::isValid, &pc, "1234554321");
    thread t3(&PalindromeCheck::isValid, &pc, "123454321");
    thread t4(&PalindromeCheck::isValid, &pc, "1234584321");
    thread t5(&PalindromeCheck::isValid, &pc, "a111111111111111111111111111111111111111111111111111a");
    thread t6(&PalindromeCheck::isValid, &pc, "abcdefghijhgfedcba");
    thread t7(&PalindromeCheck::isValid, &pc, "able was I.I saw elba");
    thread t8(&PalindromeCheck::isValid, &pc, "The quick red fox ...");
    thread t9(&PalindromeCheck::isValid, &pc, "able was I.I saw elba");
    thread t10(&PalindromeCheck::isValid, &pc, "The quick red fox ...");
    thread t11(&PalindromeCheck::isValid, &pc, "The quick red fox ...");
    t11.join();
    t10.join();
    t9.join();
    t8.join();
    t7.join();
    t6.join();
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();

    cout << "Final result:\n" << pc.getLog() << endl;
}
