#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <set>
#include <chrono>
#include <cmath>
#include <utility>
#include <algorithm>
#include <map>

using namespace std;
using Board = vector<string>;

struct State {
    Board board;
    vector<string> moves;
    size_t g; // Cost from start
    size_t h; // Heuristic estimate
};

int A, B, N;
pair<int, int> exit_pos;

string boardToString(const Board &b) {
    string s;
    for (const auto &row : b) s += row;
    return s;
}

void findPrimary(const Board &b, int &r, int &c, int &len, bool &horizontal) {
    for (int i = 0; i < A; ++i) {
        for (int j = 0; j < B; ++j) {
            if (b[i][j] == 'P') {
                horizontal = (j + 1 < B && b[i][j + 1] == 'P');
                len = 1;
                if (horizontal) {
                    while (j + len < B && b[i][j + len] == 'P') ++len;
                } else {
                    while (i + len < A && b[i + len][j] == 'P') ++len;
                }
                r = i;
                c = j;
                return;
            }
        }
    }
}

int heuristic(const Board &b) {
    int r, c, len;
    bool horizontal;
    findPrimary(b, r, c, len, horizontal);
    int front = horizontal ? (c + len - 1) : (r + len - 1);
    int exit_dist;
    if (horizontal) {
        exit_dist = exit_pos.second - front - 1;
    } else {
        exit_dist = exit_pos.first - front - 1;
    }
    return max(0, exit_dist);  // Pastikan tidak negatif
}

vector<State> getNeighbors(const State &cur) {
    vector<State> res;
    const Board &b = cur.board;
    set<char> seen;

    for (int i = 0; i < A; ++i) {
        for (int j = 0; j < B; ++j) {
            char ch = b[i][j];
            if (ch == '.' || ch == 'K' || seen.count(ch)) continue;
            seen.insert(ch);

            vector<pair<int, int>> cells;
            for (int x = 0; x < A; ++x)
                for (int y = 0; y < B; ++y)
                    if (b[x][y] == ch) cells.emplace_back(x, y);

            bool horizontal = (cells[0].first == cells[1].first);
            int minr = A, maxr = 0, minc = B, maxc = 0;
            for (auto &p : cells) {
                minr = min(minr, p.first);
                maxr = max(maxr, p.first);
                minc = min(minc, p.second);
                maxc = max(maxc, p.second);
            }

            if (horizontal) {
                // Geser kiri
                int steps_left = 0;
                for (int y = minc - 1; y >= 0; --y) {
                    if (b[minr][y] != '.') break;
                    steps_left++;
                }
                if (steps_left > 0) {
                    Board nb = b;
                    for (int k = 0; k < (maxc - minc + 1); ++k)
                        nb[minr][minc + k] = '.';
                    for (int k = 0; k < (maxc - minc + 1); ++k)
                        nb[minr][minc - steps_left + k] = ch;
                    State ns{nb, cur.moves, cur.g + 1, 0};
                    ns.moves.push_back(string(1, ch) + "-L");
                    res.push_back(ns);
                }

                // Geser kanan
                int steps_right = 0;
                for (int y = maxc + 1; y < B; ++y) {
                    if (b[minr][y] != '.') break;
                    steps_right++;
                }
                if (steps_right > 0) {
                    Board nb = b;
                    for (int k = 0; k < (maxc - minc + 1); ++k)
                        nb[minr][minc + k] = '.';
                    for (int k = 0; k < (maxc - minc + 1); ++k)
                        nb[minr][minc + steps_right + k] = ch;
                    State ns{nb, cur.moves, cur.g + 1, 0};
                    ns.moves.push_back(string(1, ch) + "-R");
                    res.push_back(ns);
                }
            } else {
                // Geser atas
                int steps_up = 0;
                for (int x = minr - 1; x >= 0; --x) {
                    if (b[x][minc] != '.') break;
                    steps_up++;
                }
                if (steps_up > 0) {
                    Board nb = b;
                    for (int k = 0; k < (maxr - minr + 1); ++k)
                        nb[minr + k][minc] = '.';
                    for (int k = 0; k < (maxr - minr + 1); ++k)
                        nb[minr - steps_up + k][minc] = ch;
                    State ns{nb, cur.moves, cur.g + 1, 0};
                    ns.moves.push_back(string(1, ch) + "-U");
                    res.push_back(ns);
                }

                // Geser bawah
                int steps_down = 0;
                for (int x = maxr + 1; x < A; ++x) {
                    if (b[x][minc] != '.') break;
                    steps_down++;
                }
                if (steps_down > 0) {
                    Board nb = b;
                    for (int k = 0; k < (maxr - minr + 1); ++k)
                        nb[minr + k][minc] = '.';
                    for (int k = 0; k < (maxr - minr + 1); ++k)
                        nb[minr + steps_down + k][minc] = ch;
                    State ns{nb, cur.moves, cur.g + 1, 0};
                    ns.moves.push_back(string(1, ch) + "-D");
                    res.push_back(ns);
                }
            }
        }
    }
    return res;
}

string dirWord(char d) {
    switch (d) {
        case 'L': return "kiri";
        case 'R': return "kanan";
        case 'U': return "atas";
        case 'D': return "bawah";
        default: return "";
    }
}


void printB(Board b) {
        for (int i = 0; i < A; i++) {
            cout << b[i] << endl;
        }
    }
int main() {
    string path;
    cout << "Masukkan path file input (.txt): ";
    getline(cin, path); // Baca seluruh line termasuk spasi

    ifstream File(path);
    if (!File) {
        cerr << "Cannot open " << path << "\n";
        perror("Error details");
        return 1;
    }

    Board startBoard;
    string readline;
    int line = 1;
    while (getline(File, readline)) {
        if (line == 1) {
            try {
                A = stoi(readline.substr(0, readline.find(' ')));
                B = stoi(readline.substr(readline.find(' ')+1));
            } catch (...) {
                cerr << "Size of board can't be read";
                return 1;
            }
        } else if (line == 2) {
            try {
                N = stoi(readline);
            } catch (...) {
                cerr << "Amount of pieces can't be read";
                return 1;
            }

        } else {
            startBoard.push_back(readline);
        }
        line++;
    }
    File.close();

    // Cari posisi exit (K)
    exit_pos = {-1, -1};
    for (int i = 0; i < A+1; ++i) {
        for (int j = 0; j < B+1; ++j) {
            if (startBoard[i][j] == 'K') {
                exit_pos = {i, j};
                break;
            }
        }
        if (exit_pos.first != -1) break;
    }

    if (exit_pos.first == -1) {
        cerr << "Exit (K) tidak ditemukan di papan!\n";
        return 1;
    }

    // Validasi bahwa K berada di border
    int ei = exit_pos.first, ej = exit_pos.second;
    bool onBorder = (ei == 0) || (ei == A) || (ej == 0) || (ej == B);
    bool verticalP  = (ei == 0) || (ei == A);

    if (!onBorder) {
        cerr << "Error: Exit (K) harus berada di salah satu sisi papan\n";
        return 1;
    }

    // Cek primary piece ('P') ada pada row/kolom yang sama dengan exit piece
    bool foundP;
    if (verticalP) {
        for (int i = 0; i < A; i++) {
            if (foundP) {
                if (startBoard[i][ej] == 'P' && startBoard[i-1][ej] != 'P') {
                    cerr << "Disconnected P";
                    return 1;
                } 
            } else {
                if (startBoard[i][ej] == 'P') {
                    foundP = true;
                }
            }
        }
    }
    else {
        for (int j = 0; j < B; j++) {
            if (foundP) {
                if (startBoard[ei][j] == 'P' && startBoard[ei][j-1] != 'P') {
                    cerr << "Disconnected P";
                    return 1;
                } 
            } else {
                if (startBoard[ei][j] == 'P') {
                    foundP = true;
                }
            }
        }
    }
    if (!foundP) {
        cerr << "No P in row/column";
        return 1;
    }

    // Pastikan hanya ada tepat satu primary piece
    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if ((verticalP && j == ej) || (!verticalP && i == ei)) continue;
            if (startBoard[i][j] == 'P') {
                cerr << "Multiple P";
                return 1;
            }
        }
    }

    // Cek jumlah pieces
    int actualPieceCount = 0;
    Board boardCopy = startBoard;
    map<char, bool> pieces;

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            char c = boardCopy[i][j];
            if (c == ' ' || c == '.' || c == 'K') continue;

            if (pieces[c]) {
                cerr << "Found multiple of the same piece (" << c << ")";
                return 1;
            }

            pieces[c] = true;
            actualPieceCount++;
            boardCopy[i][j] = ' ';

            int l = 1;
            if (boardCopy[i+1][j] == c) { // Vertical
                while (boardCopy[i+l][j] == c) {
                    boardCopy[i+l][j] = ' ';
                    l++;
                }
            } else {
                while (boardCopy[i][j+l] == c) {
                    boardCopy[i][j+l] = ' ';
                    l++;
                }
            }
        }
    }
    
    if (actualPieceCount != N) {
        cerr << "Mismatching piece count";
        return 1;
    }

    int alg;
    cout << "Pilih algoritma (1: UCS, 2: Best First Search, 3: A*): ";
    cin >> alg;

    auto startTime = chrono::high_resolution_clock::now();
    vector<string> finalMoves;
    unordered_set<string> visited;

    if (alg == 1) { // UCS
        auto cmp = [](const State &a, const State &b) { return a.g > b.g; };
        priority_queue<State, vector<State>, decltype(cmp)> pq(cmp);
        pq.push({startBoard, {}, 0, 0});
        visited.insert(boardToString(startBoard));

        while (!pq.empty()) {
            State cur = pq.top(); pq.pop();
            int r, c, len; bool horizontal;
            findPrimary(cur.board, r, c, len, horizontal);
            // Perbaikan kondisi kemenangan
            int front = horizontal ? (c + len - 1) : (r + len - 1);
            if ((horizontal && r == exit_pos.first && front == exit_pos.second - 1) ||
                (!horizontal && c == exit_pos.second && front == exit_pos.first - 1)) {
                finalMoves = cur.moves;
                break;
            }
            for (auto &nxt : getNeighbors(cur)) {
                string key = boardToString(nxt.board);
                if (!visited.count(key)) {
                    visited.insert(key);
                    pq.push(nxt);
                }
            }
        }
    } else { // Best-First atau A*
        auto cmp = [alg](const State &a, const State &b) {
            if (alg == 2) return a.h > b.h;
            return (a.g + a.h) > (b.g + b.h);
        };
        priority_queue<State, vector<State>, decltype(cmp)> pq(cmp);
        State initial{startBoard, {}, 0, (size_t)heuristic(startBoard)};
        pq.push(initial);
        visited.insert(boardToString(startBoard));

        while (!pq.empty()) {
            State cur = pq.top(); pq.pop();
            int r, c, len; bool horizontal;
            findPrimary(cur.board, r, c, len, horizontal);
            int front = horizontal ? (c + len - 1) : (r + len - 1);
            if ((horizontal && r == exit_pos.first && front == exit_pos.second - 1) ||
                (!horizontal && c == exit_pos.second && front == exit_pos.first - 1)) {
                finalMoves = cur.moves;
                break;
            }
            for (auto &nxt : getNeighbors(cur)) {
                string key = boardToString(nxt.board);
                if (!visited.count(key)) {
                    visited.insert(key);
                    nxt.h = heuristic(nxt.board);
                    pq.push(nxt);
                }
            }
        }
    }

    // Output hasil
    cout << "Papan Awal\n";
    for (auto &row : startBoard) cout << row << "\n";
    cout << "\n";

    if (finalMoves.empty()) {
        cout << "Tidak ada solusi ditemukan.\n";
    } else {
        Board current = startBoard;

        string base_filename = path.substr(path.find_last_of("/\\") + 1);
        string::size_type const p(base_filename.find_last_of('.'));
        string file_without_extension = base_filename.substr(0, p);
        ofstream move_out(file_without_extension + "-moves.txt");
        move_out << A << ' ' << B << endl;
        for (auto s: startBoard) {
            move_out << s << endl;
        }

        for (size_t i = 0; i < finalMoves.size(); ++i) {
            string move = finalMoves[i];
            char car = move[0];
            char dir = move[2];
            cout << "Gerakan " << i + 1 << ": " << car << "-" << dirWord(dir) << "\n";
            bool found = false;
            vector<State> neighbors = getNeighbors({current, {}, 0, 0});
            for (auto &n : neighbors) {
                if (!n.moves.empty() && n.moves.back() == move) {
                    current = n.board;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cerr << "Error: Gerakan tidak valid: " << move << "\n";
                return 1;
            }
            for (auto &row : current) cout << row << "\n";
            cout << "\n";

            move_out << move << endl;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << "Waktu (ms): " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << "\n";
    return 0;
}
