#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <utility>
#include <tuple>
#include <unordered_set>

using namespace std;

const char WALL = '#';
const char PLAYER = '-';
const char BOX = '*';
const char GOAL = 'O';

class Directions {
    public:
        constexpr const static int dy [4] = { -1, 0, 0, 1};
        constexpr const static int dx [4] = { 0, -1, 1, 0};
        const static int count = 4;
};

class Board {
    public: 
        Board(const vector<vector<char>>& board) : brd(board) {}

        const vector<vector<bool>>& reachablePositions(
            const pair<int, int>& player, const vector<pair<int, int>>& boxes) {

            int m, n = rows(), cols();

            queue<pair<int, int>> queue ( { player } );
            vector<vector<bool>> reachable(m, vector<bool>(n));

            while ( !queue.empty() ) {
                auto position = queue.front();
                queue.pop();

                int py = position.first;
                int px = position.second;
                for ( int i = 0; i < Directions::count; i++ ) {
                    int nx = px + Directions::dx[i];
                    int ny = py + Directions::dy[i];

                    if (within(ny, nx) && !reachable[ny][nx]) {
                        reachable[ny][nx] = true;
                        queue.push({ny, nx});
                    }
                }
            }

            return reachable;
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
            playerPosition(playerPos), 
            boxes(bxes),
            move(' '),
            parentState(nullptr) { }
            
        State(const pair<int, int>& playerPos, const vector<pair<int, int>>& bxes, State* parSt, char mv): 
            playerPosition(playerPos), 
            boxes(bxes),
            move(mv),
            parentState(parSt) { }

        State movePlayer(int dy, int dx) {
            auto newBoxes = boxes;
            char mv;
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

            auto newPlayerPos = make_pair(playerPosition.first + dy, playerPosition.second + dx);
            return State(newPlayerPos, newBoxes, this, mv);
        }

        State dragBox(int dy, int dx) {
            // assumes that the movement is valid
            auto movedBox = make_pair(-dy, -dx);
            int i;
            for( i = 0; i < boxes.size(); i++) {
                if (movedBox == boxes[i]) {
                    break;
                }
            }

            auto newBoxes = boxes;
            newBoxes[i] = playerPosition;
            char mv;
            if ( dy == 0 ) {
                if ( dx == -1 ) {
                    mv = 'L';
                } else if ( dx == 1) {
                    mv = 'R';
                }
            } else if (dx == 0) {
                if (dy == -1) {
                    mv = 'U';
                } else if (dy == 1) {
                    mv = 'D';
                }
            }

            auto newPlayerPos = make_pair(playerPosition.first + dy, playerPosition.second + dx);
            return State(newPlayerPos, newBoxes, this, mv);
        }

        bool operator==(const State& st) const { 
            return playerPosition == st.playerPosition && boxes == st.boxes;
        } 

        static string output(State* state) {
            string s = "";
            while (state -> parentState != NULL ) {
                s.push_back(state -> move);
                state = state -> parentState;
            }

            reverse(s.begin(), s.end());
            return s;
        }
};

template<>
  struct hash<State> {
    size_t operator()(const State& st) {
        size_t hsh = 0;
        for ( auto pos : st.boxes) {
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

        int playerPos = st.playerPosition.first * 1013 + st.playerPosition.second;
        return hsh ^ (hash<int>()(playerPos));
    }
  };

// class Player {
//     public:
//         pair<int, int> static normalized(const vector<vector<bool>>& reachable) {
//             int m = reachable.size();
//             int n = reachable.size();

//             for (int j = 0; j < m; j++) {
//                 for (int i = 0; i < n; i++) {
//                     if (reachable[j][i]) {
//                         return { j, i };
//                     }
//                 }
//             }

//             return {-1, -1};
//         }
// };


class Solution {
    public:
        Solution(const Board brd): board(brd) {}

        void go(State initialState) {
            bool foundSolution = false;
            while (!foundSolution || !queue.empty()) {
                State parent = queue.front();
                queue.pop();

                for ( auto state : successors(parent) ) {
                    if ( visited.find(state) == visited.end()) {
                        visited.insert(state);
                        queue.push(state);

                        if ( winningState(state) ) {
                            foundSolution = true;
                            cout << "Found solution!" << endl;
                            cout << State::output(&state) << endl;
                            break;
                        }
                    }
                }
            }
        }

        vector<State>& successors(State& parent) {
            vector<State> succs;

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

        bool winningState(State st) {
            for (auto box : st.boxes) {
                if (!board.atGoal(box)) {
                    return false;
                }
            }

            return true;
        }

    private:
        Board board;
        unordered_set<State> visited;
        queue<State> queue;
};


int main() {
    cout << "Welcome back to this hell of a language!" << endl;

    int rows, cols;
    cin >> rows;
    cin >> cols;

    vector<vector<char>> brd(rows, vector<char>(cols));
    vector<pair<int, int>> boxes;
    pair<int, int> player;

    for( int j = 0; j < rows; j++ ) {
        for (int i = 0; i < cols; i++ ) {
            char chr; 
            cin >> chr;

            if (chr == BOX) {
                boxes.push_back(make_pair(j, i));
            } if ( chr == PLAYER ) {
                player = make_pair(j, i);
            } else {
                brd[j][i] = chr;
            }
        }
    }

    // calculate top-left reachable position of the player
    auto board = Board(brd);
    auto initial = State(player, boxes);

    Solution(board).go(initial);

    return 0;
}