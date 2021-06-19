// This code was written by Andy Winger and is available
// to anyone wishing to use it for whatever purpose. I wouldn't
// mind a little credit. I also wouldn't mind corrections and
// suggestions for improvement.
// andywin58@gmail.com

// This is a redo of a college project I did in an undergraduate
// Data Structures class at Florida International University. I did
// the original project in Ada (which I still consider to be a cool
// language) using linked lists of integers each with values of 
// between 0 and 999. Implement add, subtract and multiply;
// division was extra credit and I didn't do it. So this was, in
// affect, base 1000 arithmetic. I tested by computing 500!. 

// My redo here is in C++14 using deque<int>. Again, I didn't
// implement division. I will likely come back and do that later.

#include <deque>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <chrono>
#include <stdexcept>

using namespace std;

// The setw() in operator<<() is passed the value 3. If UPPER
// is changed then that needs to change as well.
constexpr int UPPER = 999;

class BigInt {
    public:
    BigInt() : isNegative_(false) {}
    explicit BigInt(long long n);

    BigInt operator+(const BigInt & rhs) const;
    BigInt& operator+=(const BigInt & rhs) {
        *this = *this + rhs;
        return *this;
    }
    BigInt operator-(const BigInt & rhs) const;
    BigInt& operator-=(const BigInt & rhs) {
        *this = *this - rhs;
        return *this;
    }
    BigInt operator*(const BigInt & rhs) const;
    BigInt operator*(long long rhs) const {
        return *this * BigInt(rhs);
    }
    BigInt& operator*=(const BigInt & rhs) {
        *this = *this * rhs;
        return *this;
    }
    BigInt& operator*=(long long rhs) {
        *this = *this * rhs;
        return *this;
    }

    bool operator==(const BigInt & rhs) const {
        return (isNegative_ == rhs.isNegative_ &&
                deq_ == rhs.deq_ );
    }
    bool operator!=(const BigInt & rhs) const {
        return ! (*this == rhs);
    }
    bool operator<(const BigInt & rhs) const;
    bool operator<=(const BigInt & rhs) const {
        return (*this == rhs || *this < rhs);
    }
    bool operator>(const BigInt & rhs) const {
        return ! (*this <= rhs);
    }
    bool operator>=(const BigInt & rhs) const {
        return ! (*this < rhs);
    }

    friend ostream & operator<<(ostream &os, const BigInt &big);

    private:
    void addTogther(deque<int> &rtn, const deque<int> &a,
                                     const deque<int> &b) const;
    void subtractSecond(deque<int> &a, const deque<int> &b) const;
    void subtractBorrow(deque<int>::reverse_iterator ri,
                        deque<int>::reverse_iterator theend) const;
    int getAddCarry(int &sum) const;
    int getMultCarry(int &product) const;
    bool isLess(const deque<int> &a, const deque<int> &b) const;

    bool isNegative_;
    deque<int> deq_;
};

BigInt::BigInt(long long n) {
    this->isNegative_ = false;
    if(n < 0) {
        this->isNegative_ = true;
        n *= -1;
    }

    while(n > UPPER) {
        deq_.push_front(n % (UPPER + 1));
         n /= (UPPER + 1);
    }
    deq_.push_front(n);
}

bool BigInt::operator<(const BigInt & rhs) const {
    bool rtn = false;
    if (this->isNegative_ == rhs.isNegative_) {
        if(this->isNegative_) {
            rtn = isLess(rhs.deq_, deq_);
        }
        else {
            rtn = isLess(deq_, rhs.deq_);
        }
    }
    else {
        rtn = this->isNegative_;
    }
    return rtn;
}

BigInt BigInt::operator+(const BigInt & rhs) const {
    BigInt rtn;
    if (this->isNegative_ == rhs.isNegative_) {
        rtn.isNegative_ = this->isNegative_;
        addTogther(rtn.deq_, this->deq_, rhs.deq_);
    }
    else {
        // Determine sign of result and which to subtract.
        if(isLess(rhs.deq_, this->deq_)) {
            rtn.isNegative_ = this->isNegative_;
            rtn.deq_ = this->deq_;
            subtractSecond(rtn.deq_, rhs.deq_);
        }
        else {
            rtn.isNegative_ = rhs.isNegative_;
            rtn.deq_ = rhs.deq_;
            subtractSecond(rtn.deq_, this->deq_);
        }
    }

    // Sign could be wrong for 0.
    if(rtn.deq_.size() == 1 && rtn.deq_.at(0) == 0) {
        rtn.isNegative_ = false;
    }
    return rtn;
}

BigInt BigInt::operator-(const BigInt & rhs) const {
    BigInt rtn;
    if (this->isNegative_ != rhs.isNegative_) {
        rtn.isNegative_ = this->isNegative_;
        addTogther(rtn.deq_, this->deq_, rhs.deq_);
    }
    else {
        // Determine sign of result and which to subtract.
        if(isLess(rhs.deq_, this->deq_)) {
            rtn.isNegative_ = this->isNegative_;
            rtn.deq_ = this->deq_;
            subtractSecond(rtn.deq_, rhs.deq_);
        }
        else {
            rtn.isNegative_ = ! this->isNegative_;
            rtn.deq_ = rhs.deq_;
            subtractSecond(rtn.deq_, this->deq_);
        }
    }

    // Sign could be wrong for 0.
    if(rtn.deq_.size() == 1 && rtn.deq_.at(0) == 0) {
        rtn.isNegative_ = false;
    }
    return rtn;
}

BigInt BigInt::operator*(const BigInt & rhs) const {
    BigInt rtn;
    int carry = 0;
    int multiplier = 0;
    for (auto it = deq_.rbegin(); it != deq_.rend(); ++it) {
        BigInt intermediate;
        for (auto rit = rhs.deq_.rbegin(); rit != rhs.deq_.rend(); ++rit) {
            int product = *it * *rit + carry;
            carry = getMultCarry(product);
            intermediate.deq_.push_front(product);
        }
        if(carry > 0) {
            intermediate.deq_.push_front(carry);
            carry = 0;
        }
        for(int i=0; i<multiplier; ++i) {
            intermediate.deq_.push_back(0);
        }
        ++multiplier;

        rtn += intermediate;
    }

    // Get the sign right
    if(this->isNegative_ == rhs.isNegative_) {
        rtn.isNegative_ = false;
    }
    else {
        rtn.isNegative_ = true;
    }

    // Remove leading 0's.
    while(rtn.deq_.front() == 0 && rtn.deq_.size() > 1) {
        rtn.deq_.pop_front();
    }

    // Sign could be wrong for 0.
    if(rtn.deq_.size() == 1 && rtn.deq_.at(0) == 0) {
        rtn.isNegative_ = false;
    }

    return rtn;
}

ostream & operator<<(ostream &os, const BigInt &big) {
    auto it = big.deq_.begin();
    int count = 0;
    if(big.isNegative_) {
        os << "-";
    }

    // No leading 0s.
    if(it != big.deq_.end()) {
        os << big.deq_.at(0);
        ++it;
    }

    for( ; it != big.deq_.end(); ++it ) {
        os << setw(3) << setfill('0') << *it;
        if(count > 25) {
            os << '\n';
            count = 0;
        }
        else
        {
            ++count;
        }
    }
    return os;
}

bool BigInt::isLess(const deque<int> &a, const deque<int> &b) const {
    bool rtn = false;
    if(a.size() == b.size()) {
        auto ait = a.begin();
        auto bit = b.begin();
        while (ait != a.end() && bit != b.end()) {
           if(*ait != *bit) {
               rtn = (*ait < *bit);
               break;
           }
           else {
               ++ait;
               ++bit;
           }
        }
        // If all elements of both lists are all equal then
        // rtn will remain false which is correct.
    }
    else {
        rtn = (a.size() < b.size());
    }

    return rtn;
}

int BigInt::getAddCarry(int &sum) const {
    int carry = 0;
    if(sum > UPPER) {
        carry = 1;
        sum -= (UPPER + 1);
    }
    return carry;
}

// Add the contents of this->deq_ to b and put result in a. a should
// be empty when passed in.
void BigInt::addTogther(deque<int> &rtn, const deque<int> &a,
                                         const deque<int> &b) const {
    int carry = 0;
    auto ait = a.rbegin();
    auto bit = b.rbegin();
    while (ait != a.rend() && bit != b.rend()) {
        int sum = *ait + *bit + carry;
        carry = getAddCarry(sum);
        rtn.push_front(sum);
        ++ait;
        ++bit;
    }

    // Possibly one of these BigInts has more.
    for (; ait != a.rend(); ++ait) {
        int sum = *ait + carry;
        carry = getAddCarry(sum);
        rtn.push_front(sum);
    }

    for (; bit != b.rend(); ++bit) {
        int sum = *bit + carry;
        carry = getAddCarry(sum);
        rtn.push_front(sum);
    }

    if(carry > 0) {
        rtn.push_front(carry);
    }
}

void BigInt::subtractBorrow(deque<int>::reverse_iterator ri,
                    deque<int>::reverse_iterator theend) const {
    if(theend == ri) {
        // This isn't exceptional; its a programming error. It shouldn't
        // happen.
        throw runtime_error("End reached in subtractBorrow.");
    }

    // We just need 1.
    if(*ri > 0) {
       *ri -= 1; // Done. Assume the ability to borrow exists.
    }
    else {
        // *ri must be 0. So set it to UPPER and then borrow from the next.
        *ri = UPPER;
        ++ri;
        subtractBorrow(ri, theend);
    }
}

void BigInt::subtractSecond(deque<int> &a, const deque<int> &b) const {
    auto itMore = a.rbegin();
    auto itLess = b.rbegin();
    while (itMore != a.rend() ) {
        if(*itLess <= *itMore) {
            // subtract and move on.
            *itMore -= *itLess;
        }
        else {
            // Need to borrow.
            // This could possibly leave a 0 front member
            // which will be dealt with later.
            subtractBorrow(itMore + 1, a.rend());
            *itMore += UPPER + 1;
            *itMore -= *itLess;
        }
        ++itMore;
        ++itLess;
    }

    // Remove leading 0's.
    while(a.front() == 0 && a.size() > 1) {
        a.pop_front();
    }
}

int BigInt::getMultCarry(int &product) const {
    int carry = 0;
    if(product > UPPER) {
        carry = product / (UPPER + 1);
        product = product % (UPPER + 1);
    }
    return carry;
}

// I copied ThreadPool from
// https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
// and did some multithreaded testing.
// 500! calculated with 8 threads took around 0.1 second on average.
// Interestingly if I just partitioned the job into 8 parts and ran them
// sequentially the time to run all was about the same, i.e. I saw no
// improvement from multithreading. I wish I could explain why.
// If I run this function with no partitioning and no multithreading
// it usually took between 2 and 3 seconds.
// All testing was done on https://www.onlinegdb.com/online_c++_compiler
BigInt factorial(int beg, int end) {
    BigInt rtn(end);

    for(int i=end-1; i>= beg; --i) {
        rtn *= i;
    }
    cout << "Thread " << this_thread::get_id() << ": Partion " << beg << "->" << end << ":" << endl;
    cout << rtn << endl;
    return rtn;
}

// I tested with a sequence of 333.
void PrintFibonacciSequence(int n) {
    BigInt prev(1);
    BigInt prevprev(0);
    BigInt theswap(0);
    cout << 0 << endl;
    cout << 1 << endl;
    for(int inx = 1; inx<n; ++inx) {
        theswap = prev + prevprev;
        cout << theswap << endl;
        prevprev = prev;
        prev = theswap;
    }
}

int main()
{
    int num_pools = 8;
 //   ThreadPool pool(num_pools);
    int factorializeThis = 500;
//    vector< future<BigInt> > results;
    int partition = factorializeThis / num_pools;
    int beg = 1;
    int end = partition;
    auto start = chrono::high_resolution_clock::now();
    BigInt x(1);
    do {
        BigInt p = factorial( beg, end);
        beg = end + 1;
        end += partition;
        if(end > factorializeThis)
            end = factorializeThis;
        x = x * p;
    } while(beg<factorializeThis);
/*
    do {

        results.emplace_back(
           pool.enqueue(factorial, beg, end));
        beg = end + 1;
        end += partition;
        if(end > factorializeThis)
            end = factorializeThis;
    } while(beg<factorializeThis);

    for(auto && result: results) {
        BigInt p = result.get();
        x = x * p;
    }
*/

    cout << factorializeThis << "! =" << endl;
    cout << x;
    std::cout << std::endl;
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

//    cout << "Multithreaded time to execute was " << duration.count() << " microseconds" << endl;
    cout << "Single threaded partitioned time to execute was "
         << duration.count() << " microseconds" << endl;
    return 0;
}
