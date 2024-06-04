// Leetcode problem 149. This implementation passed 34/41 and failed with a time
// limit exceeded.

// Given an array of points where points[i] = [xi, yi] represents a point on the
// X-Y plane, return the maximum number of points that lie on the same straight
// line.

// Example 1:
// Input: points = [[1,1],[2,2],[3,3]]
// Output: 3
// Example 2:
// Input: points = [[1,1],[3,2],[5,3],[4,1],[2,3],[1,4]]
// Output: 4

// Constraints:
// 1 <= points.length <= 300
// points[i].length == 2
// -104 <= xi, yi <= 104
// All the points are unique.

using IntPair = std::pair<int, int>;

struct IntPairHash {
    static_assert(sizeof(int) * 2 == sizeof(size_t));

    size_t operator()(IntPair p) const noexcept {
        return size_t(p.first) << 32 | p.second;
    }
};

class Solution {
public:
    static bool eql(double a, double b)
    {
        return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a))
                * std::numeric_limits<float>::epsilon());
     //   return abs(a - b) <= ( (abs(a) < abs(b) ? abs(b) : abs(a))
     //           * std::numeric_limits<float>::epsilon());
//cout << a << " and " << b << " are " << (bbb ? "equal" : "NOT equal") << endl;
//return bbb;
    }

    struct LineStat {
        double m; // slope
        double b; // y-intercept
      //  std::unordered_ Can't use without defining a hash for pairs
        unordered_set<pair<int, int>, IntPairHash > pts;
        bool infinite_slope = false;
        int xi = 0;
        LineStat (float slope, float y, bool infinite, int x) :
        m(slope), b(y), infinite_slope(infinite), xi(x) {}
        bool operator == (const LineStat &rhs) const {
            return (eql(m, rhs.m) && eql(b, rhs.b) ||
                    (infinite_slope == rhs.infinite_slope &&
                    xi == rhs.xi));
        }
    };

    void get_m_and_b(const pair<int, int> &p1, 
                     const pair<int, int> &p2, 
                     vector<LineStat> &vls) {
 //   cout.setf(ios::fixed);
//cout << setprecision(13) ;
//cout << "p1= (" << p1.first << ", " << p1.second << ", p2= (" << p2.first << ", " << p2.second << ")" ;//<< endl;
        double slope = 99999.0; // The value chosen here has impact
        bool isinfinite = false;
        int xintercept = 0;
        if (p2.first != p1.first) {
            slope = static_cast<double>((p2.second - p1.second)) / (p2.first - p1.first);
  //          cout << ". slope= " << slope ;
        }
        else {
            isinfinite = true;
            xintercept = p1.first;
        }

        double yintercept = static_cast<double>(p1.second) - slope * p1.first;
//cout << ", yintercept= " << yintercept << endl;
        auto it = std::find_if(vls.begin(), vls.end(), [=](LineStat ls)
        {
            return (eql(ls.m, slope) && eql(ls.b, yintercept) ||
                   (ls.infinite_slope && isinfinite && ls.xi == xintercept));
        });
        if (it != vls.end()) {
            it->pts.insert(p1);
            it->pts.insert(p2);
//cout << "Found: m= " << it->m << ", b= " << it->b << endl; //", count= " << it->count << endl;
        }
        else {
            vls.emplace_back(slope, yintercept,
                             isinfinite, xintercept);
        }
    }

    // This should be a vector of pairs.
    int maxPoints(vector<vector<int>>& points) {
        if (points.size() < 3) {
            return points.size();
        }
        vector<LineStat> vec_ls;
        // If points.size() > 1 then return will be at least 2.
        for (int inx=0; inx< points.size() - 1; ++inx) {
            for (int jnx=inx+1; jnx< points.size(); ++jnx) {
                get_m_and_b (make_pair(points.at(inx).at(0),
                                       points.at(inx).at(1)),
                             make_pair(points.at(jnx).at(0),
                                       points.at(jnx).at(1)),
                    vec_ls);
            // If p is on any current lines then increment that line point count
            // else 
            }
        }

        int rtn = 2;
        std::for_each(vec_ls.begin(), vec_ls.end(), [&](LineStat ls)
        {
            if (rtn < ls.pts.size()) rtn = ls.pts.size();
        });

        return rtn;
    }
};
