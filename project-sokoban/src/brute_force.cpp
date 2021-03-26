#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <utility>
#include <tuple>
#include <unordered_set>

using namespace std;

// #define DEBUG 

#ifdef DEBUG 
#define deb(x) (x)
#else 
#define deb(x) do{}while(0)
#endif

const char WALL = '#';
const char PLAYER = '-';
const char BOX = 'O';
const char GOAL = '*';
const char EMPTY = ' ';

class Directions {
    public:
        constexpr const static int dy [4] = { -1, 0, 0, 1};
        constexpr const static int dx [4] = { 0, -1, 1, 0};
        const static int count = 4;
};

class Board {
    public: 
        Board(const vector<vector<char>>& board) : brd(board) {}

        // not used yet, todo fixed &boxes param not used
        // const vector<vector<bool>> reachablePositions(
        //     const pair<int, int>& player, const vector<pair<int, int>>& boxes) {

        //     int m = rows();
        //     int n = cols();

        //     queue<pair<int, int>> queue ( { player } );
        //     vector<vector<bool>> reachable(m, vector<bool>(n));

        //     while ( !queue.empty() ) {
        //         auto position = queue.front();
        //         queue.pop();

        //         int py = position.first;
        //         int px = position.second;
        //         for ( int i = 0; i < Directions::count; i++ ) {
        //             int nx = px + Directions::dx[i];
        //             int ny = py + Directions::dy[i];

        //             if (within(ny, nx) && !reachable[ny][nx]) {
        //                 reachable[ny][nx] = true;
        //                 queue.push({ny, nx});
        //             }
        //         }
        //     }

        //     return reachable;
        // }

        char** boardCopy() {
            int m = rows();
            int n = cols();

            char** tmp = new char*[m];
            for (int i = 0; i < m; i++) {
                tmp[i] = new char[n];
            }

            for(int j = 0; j < m; j++) {
                for (int i = 0; i < n; i++) {
                    tmp[j][i] = brd[j][i];
                }
            }

            return tmp;
        }

        int rows() {
            return brd.size();
        }

        int cols() {
            return brd[0].size();
        }

        bool atGoal(pair<int, int> box) {
            return atGoal(box.first, box.second);
        }

        bool atGoal(int y, int x) {
            return brd[y][x] == GOAL;
        }

        bool accessible(pair<int, int> p) {
            return accessible(p.first, p.second);
        }

        bool accessible(int y, int x) {
            return within(y, x) && brd[y][x] != WALL;
        }

        bool within(int y, int x) {
            return (y >= 0 and y < rows() && x >= 0 and x < cols());
        }

        bool within(pair<int, int> p) {
            return within(p.first, p.second);
        }

    private:
        vector<vector<char>> brd;
};

class State {
    public:
        vector<pair<int, int>> boxes;
        pair<int, int> playerPosition; 
        State* parentState;
        char move;

        
        State(const pair<int, int>& playerPos, const vector<pair<int, int>>& bxes): 
            boxes(bxes),
            playerPosition(playerPos), 
            parentState(nullptr),
            move(' ') { }
            
        State(const pair<int, int>& playerPos, const vector<pair<int, int>>& bxes, State* parSt, char mv): 
            boxes(bxes),
            playerPosition(playerPos), 
            parentState(parSt),
            move(mv) { }

        State* movePlayer(int dy, int dx) {
            auto newBoxes = boxes;
            char mv = '\0';
            if ( dy == 0 ) {
                if ( dx == -1 ) {
                    mv = 'l';
                } else if ( dx == 1) {
                    mv = 'r';
                }
            } else if (dx == 0) {
                if (dy == -1) {
                    mv = 'u';
                } else if (dy == 1) {
                    mv = 'd';
                }
            }

            int py = playerPosition.first;
            int px = playerPosition.second;
            auto newPlayerPos = make_pair(py + dy, px + dx);
            return new State(newPlayerPos, newBoxes, this, mv);
        }

        State* dragBox(int dy, int dx) {
            // assumes that the movement is valid
            int py = playerPosition.first;
            int px = playerPosition.second;

            auto movedBox = make_pair(py - dy, px - dx);
            size_t i;
            for( i = 0; i < boxes.size(); i++) {
                if (movedBox == boxes[i]) {
                    break;
                }
            }

            auto newBoxes = boxes;
            newBoxes[i] = playerPosition;
            char mv = '\0';
            if ( dy == 0 ) {
                if ( dx == -1 ) {
                    mv = 'L';
                } else {
                    mv = 'R';
                }
            } else if (dx == 0) {
                if (dy == -1) {
                    mv = 'U';
                } else {
                    mv = 'D';
                }
            }

            auto newPlayerPos = make_pair(py + dy, px + dx);
            return new State(newPlayerPos, newBoxes, this, mv);
        }

        bool operator==(const State& st) const { 
            return playerPosition == st.playerPosition && boxes == st.boxes;
        } 

        static string output(State* state) {
            string s = "";
            State* tmp = state;
            while (tmp -> parentState != nullptr ) {
                s.push_back(tmp -> move);
                tmp = tmp -> parentState;
            }

            reverse(s.begin(), s.end());
            return s;
        }
};

struct StatePtrHash {
    size_t operator()(State* const& st) const {
        size_t hsh = 0;
        for ( auto pos : st -> boxes) {
            size_t x = pos.first * 1013 + pos.second;
            x ^= x >> 17;
            x *= 830770091;   
            x ^= x >> 11;
            x *= -1404298415; 
            x ^= x >> 15;
            x *= 830770091;   
            x ^= x >> 14;

            hsh += x;
        }

        int playerPos = st -> playerPosition.first * 1013 + st -> playerPosition.second;
        return hsh ^ (hash<int>()(playerPos));
    }
};

struct StatePtrEq {
    bool operator () ( State const* lhs, State const* rhs ) const {
        return lhs->operator==(*rhs);
    }
};

class Solution {
    public:
        Solution(const Board brd): board(brd) {}

        void go(State* initialState) {
            bool foundSolution = false;
            unordered_set<State*, StatePtrHash, StatePtrEq> visited;
            queue<State*> queue ( { initialState } );
            while (!(foundSolution || queue.empty())) {
                State* parent = queue.front();
                queue.pop();
                auto succs = successors(*parent);

                deb(cout << "Visited states " << visited.size() << endl );
                deb(cout << "Queue size " << queue.size() << endl );
                deb(cout << "New states " << succs.size() << endl );
                deb(showState(*parent));
                
                for ( auto state : succs ) {
                    auto foo = visited.find(state);
                    if ( foo == visited.end()) {
                        visited.insert(state);
                        queue.push(state);

                        if ( winningState(state) ) {
                            foundSolution = true;
                            deb(cout << "Found solution!" << endl);
                            cout << State::output(state) << endl;
                            break;
                        }
                    }
                }                 
            }
        }

        vector<State*> successors(State& parent) {
            vector<State*> succs;

            int py = parent.playerPosition.first;
            int px = parent.playerPosition.second;

            for (int i = 0; i < Directions::count; i++) {
                int dx = Directions::dx[i];
                int dy = Directions::dy[i];

                int nx = px + dx;
                int ny = py + dy;
                // check if it's possible to move 
                if ( validMove(parent.boxes, ny, nx) ) {
                    succs.push_back(parent.movePlayer(dy, dx));

                    int by = py - dy;
                    int bx = px - dx;

                    // check if it's possible to drag a box 
                    if ( validDrag(parent.boxes, by, bx) ) {
                        deb(cout << "------- Drag ------" << endl);
                        deb(cout << dy << ", " << dx << " - Delta " << endl);
                        deb(cout << py << ", " << px << " - old Player" << endl);
                        deb(cout << by << ", " << bx << " - old Box" << endl);
                        deb(cout << ny << ", " << nx << " - new Player" << endl);
                        deb(showState(parent));
                        deb(cout << "---------------------" << endl << endl);
                        succs.push_back(parent.dragBox(dy, dx));
                    } 
                }
            }

            return succs;
        }

        bool validMove(vector<pair<int, int>>& boxes, int ny, int nx) {
            auto playerPos = make_pair(ny, nx);
            for ( auto box : boxes ) {
                if (box == playerPos) {
                    return false;
                }
            }

            return board.accessible(ny, nx);
        }

        // assumes that it's already a valid move
        bool validDrag(vector<pair<int, int>>& boxes, int by, int bx) {
            auto boxPos = make_pair(by, bx);
            for ( auto box : boxes ) {
                if (box == boxPos) {
                    return true;
                }
            }

            return false;
        }

        bool winningState(State* st) {
            for (auto box : st -> boxes) {
                if (!board.atGoal(box)) {
                    return false;
                }
            }

            return true;
        }

        void showState(State& state) {
            int m = board.rows();
            int n = board.cols();
            char** tmp = board.boardCopy();

            for (auto box : state.boxes) {
                tmp[box.first][box.second] = BOX;
            }

            auto player = state.playerPosition;
            tmp[player.first][player.second] = PLAYER;

            for (int j = 0; j < m; j++ ) {
                for (int i = 0; i < n; i++ ) {
                    cout << tmp[j][i];
                }
                cout << endl;
            }

            cout << endl << endl;

            for (int j = 0; j < m; j++ ) {
                free(tmp[j]);
            }

            free(tmp);
        }

    private:
        Board board;
};


int main() {
    deb(cout << "Welcome back to this hell of a problem/language!" << endl);

    int rows, cols;
    cin >> rows;
    cin >> cols;

    vector<vector<char>> brd(rows, vector<char>(cols));
    vector<pair<int, int>> boxes;
    pair<int, int> player;
    string s;
    std::getline(std::cin, s);

    for( int j = 0; j < rows; j++ ) {
        std::getline(std::cin, s);
        for (int i = 0; i < cols; i++ ) {
            char chr = s[i];
            if ( chr == BOX || chr == PLAYER ) {
                if (chr == BOX ) {
                    boxes.push_back(make_pair(j, i));
                } else if ( chr == PLAYER ) {
                    player = make_pair(j, i);
                }

                brd[j][i] = EMPTY;
            } else {
                brd[j][i] = chr;
            }
        }
    }

    auto board = Board(brd);
    auto initial = new State(player, boxes);

    Solution(board).go(initial);

    return 0;
}