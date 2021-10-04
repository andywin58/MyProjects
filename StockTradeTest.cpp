// This was yet another pre-interview coding test that I failed. At least they gave
// plenty of time to work on it, 3 days in fact. The object was to process stock trade
// orders: BUY, SELL, MODIFY, CANCEL, and PRINT. The tricky part was to execute trades
// or partial trades immediately if possible. (Note: no stock symbol needed here.)

// A sample input:
// op  type price quanty orderId
// BUY GFD 1000 10 order1
// BUY GFD 1010 10 order2
// MODIFY order2 SELL 1010 10
// SELL GFD 1000 15 order3
// PRINT

// My coding worked fine on the given examples in the instructions but only passed
// 2/30 of their given test cases. As anyone who's taken these pre-interview HackerRank
// tests know, the test cases are hidden. How to troubleshoot something you can't see?
// Companies tend to expect much from potential employees. Can I get an emoji? :)

#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <limits>

using namespace std;

struct Order {
    string oper;
    string orderNum;
    string type;
    int price;
    int quantity;

    Order(const string &op, const string &onum, const string &t, int p, int q) :
        oper (op),
        orderNum (onum),
        type (t),
        price (p),
        quantity (q)
        {}

    Order(const Order &rhs) :
        oper (rhs.oper),
        orderNum (rhs.orderNum),
        type (rhs.type),
        price (rhs.price),
        quantity (rhs.quantity)
        {}

    bool operator <(const Order &rhs) const {
        return this->price < rhs.price;
    }
/*
    bool operator ==(const Order &rhs) const {
        return (this->price == rhs.price);
    }

    bool operator >=(const Order &rhs) const {
        return !(*this < rhs);
    }

    bool operator >(const Order &rhs) const {
        return !(*this < rhs) && !(*this == rhs);
    }
*/
};

// Return indicates whether CheckForTrade should be called again
bool HandleTrade(list<Order> &oList,
                list<Order>::iterator it,
                Order &ord) {
    auto qDiff = ord.quantity - it->quantity;
    int qPrint = ord.quantity;
    if(0 == qDiff) {
        oList.erase(it);
        ord.quantity = 0;
    }
    else if(qDiff > 0) {
        // after adjustment call CheckForTrade again
        oList.erase(it);
        ord.quantity -= it->quantity;
        qPrint = it->quantity;
    }
    else {
        it->quantity -= ord.quantity;
        ord.quantity = 0;
    }
    cout << "TRADE " << it->orderNum << " " << it->price
                                  << " " << qPrint
                     << " " << ord.orderNum
                                  << " " << ord.price
                                  << " " << qPrint << endl;
    return (qDiff > 0);
}

void CheckForTrade(list<Order> &oList,
                   const string &op,
                   Order &ord) {

    if (oList.size() > 0) {
        if ("SELL" == op) {
            // Find the BUY order with the highest price
            auto it = max_element(oList.begin(), oList.end());
            if (it != oList.end() && it->price >= ord.price) {
                // Trade condition met.
                auto ordQ_gt_itQ = HandleTrade(oList, it, ord);
                if (ordQ_gt_itQ) {
                    CheckForTrade(oList, op, ord);
                }
            }
        }
        else { // BUY order
            // Find the SELL order with the lowest price
            auto it = min_element(oList.begin(), oList.end());
            if (it != oList.end() && it->price <= ord.price) {
                // Trade condition met.
                auto ordQ_gt_itQ = HandleTrade(oList, it, ord);
                if (ordQ_gt_itQ) {
                    CheckForTrade(oList, op, ord);
                }
            }
        }
    }
}

bool IsValid(const Order &ord,
               const list<Order> &list1,
               const list<Order> &list2) {
    bool rtn = false;
    if (("GFD" == ord.type || "IOC" == ord.type) && ord.price > 0 && ord.quantity > 0) {
        auto it = find_if(list1.begin(), list1.end(),
           [&ord](const Order &item){
                return (item.orderNum == ord.orderNum);
        });
        if (list1.end() == it) {
            it = find_if(list2.begin(), list2.end(),
               [&ord](const Order &item){
                    return (item.orderNum == ord.orderNum);
            });
            if (list2.end() == it) {
                rtn = true;
            }
        }
    }

    return rtn;
}

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    string line;
    string type;
    string oNum;
    string operation;
    int p, q;
    list<Order> sellList;
    list<Order> buyList;
    getline (cin, line);
    while (!cin.eof()) {
        istringstream iss(line);
        iss >> operation;

        if ("BUY" == operation || "SELL" == operation) {
            iss >> type;
            iss >> p;
            iss >> q;
            iss >> oNum;
            Order ord(operation, oNum, type, p, q);
            if (IsValid(ord, sellList, buyList))
                continue;

            if ("BUY" == operation) {
                CheckForTrade(sellList, operation, ord);
                if (ord.quantity > 0 && ord.type != "IOC") {
                    // IOC means trade immediately or cancel
                    buyList.emplace_back(ord);
                }
            }
            else {
                // operation == "SELL"
                CheckForTrade(buyList, operation, ord);
                if (ord.quantity > 0 && ord.type != "IOC") {
                    // IOC means trade immediately or cancel
                    sellList.emplace_back(ord);
                }
            }
        }
        else if("CANCEL" == operation) {
            iss >> oNum;
            auto it = find_if(sellList.begin(), sellList.end(),
               [&oNum](const Order &item){
                    return (item.orderNum == oNum);
            });
            if (it != sellList.end()) {
                sellList.erase(it);
            }
            else {
                // If not in sellList then check buyList
                auto it = find_if(buyList.begin(), buyList.end(),
                   [&oNum](const Order &item){
                        return (item.orderNum == oNum);
                });
                if (it != buyList.end()) {
                    buyList.erase(it);
                }
                // else do nothing. Not found in either list
            }
        }
        else if("MODIFY" == operation) {
            iss >> oNum;
            string modOp;
            iss >> modOp;
            iss >> p;
            iss >> q;
            Order ord(modOp, oNum, "GFD", p, q);
            auto it = find_if(sellList.begin(), sellList.end(),
               [&oNum](const Order &item){
                    return (item.orderNum == oNum);
            });
            if (it != sellList.end()) {
                // First erase it. Append to appropriate list later
                sellList.erase(it);
                if ("BUY" == modOp) {
                    CheckForTrade(sellList, modOp, ord);
                    if (ord.quantity > 0 ) {
                        buyList.emplace_back(ord);
                    }
                }
                else {
                    CheckForTrade(buyList, modOp, ord);
                    if (ord.quantity > 0 ) {
                        sellList.emplace_back(ord);
                    }
                }
            }
            else {
                // If not in sellList then check buyList
                auto it = find_if(buyList.begin(), buyList.end(),
                   [&oNum](const Order &item){
                        return (item.orderNum == oNum);
                });
                if (it != buyList.end()) {
                    // First erase it. Append to appropriate list later
                    buyList.erase(it);
                    if ("SELL" == modOp) {
                        CheckForTrade(buyList, modOp, ord);
                        if (ord.quantity > 0 ) {
                            sellList.emplace_back(ord);
                        }
                    }
                    else {
                        CheckForTrade(sellList, modOp, ord);
                        if (ord.quantity > 0 ) {
                            buyList.emplace_back(ord);
                        }
                    }
                }
            }
        }
        else if("PRINT" == operation) {
            // Requirements not clear on PRINT.
            // Commented out code looks like they
            // should be appropriate but they only
            // reduced the pass rate.
         //   list<Order> sordidList;
        //    auto comp = []( Order a, Order b ) {
        //       return a.price > b.price;
        //    };
            cout << "SELL:" << endl;
         //   copy(sellList.begin(), sellList.end(), sordidList.begin());
        //    sordidList.sort(comp);
            for (auto s : sellList) {
                cout << s.price << " " << s.quantity << endl;
            }
            cout << "BUY:" << endl;
         //   list<Order> sordidList2;
         //   sordidList.clear();
        //    copy(buyList.begin(), buyList.end(), sordidList2.begin());
        //    sordidList.sort(comp);
            for (auto b : buyList) {
                cout << b.price << " " << b.quantity << endl;
            }
        }
        // else invalid op. do nothing

        getline (cin, line);
    }

    return 0;
}
