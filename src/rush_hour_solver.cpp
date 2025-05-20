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

int main() {
    string path;
    cout << "Masukkan path file input (.txt): ";
    getline(cin, path); // Baca seluruh line termasuk spasi

    ifstream fin(path);
    if (!fin) {
        cerr << "Cannot open " << path << "\n";
        perror("Error details");
        return 1;
    }

    fin >> A >> B >> N;
    B=B+1;
    Board startBoard(A);
    for (int i = 0; i < A; ++i) fin >> startBoard[i];
    fin.close();

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
// Perbesar ukuran papan tergantung posisi K
    if(exit_pos.first>B){
        B=B+1;
    }
    else if(exit_pos.second>A){
        A=A+1;
    }

/*
bool valid = true;


// 2) Validasi bahwa K berada di dinding (border)
int ei = exit_pos.first, ej = exit_pos.second;
bool onBorder = (ei == 0) || (ei == A-1) || (ej == 0) || (ej == B-1);
if (!onBorder) {
    cerr << "Error: Exit (K) harus berada di salah satu sisi papan\n";
    valid = false;
}

// 3) Temukan primary piece ('P') sekali, dapatkan orientasi & panjang
int pr=-1, pc=-1, plen=0;
bool horizP=false;
for (int i = 0; i < A && pr<0; ++i) {
    for (int j = 0; j < B; ++j) {
        if (startBoard[i][j] == 'P') {
            pr = i; pc = j;
            horizP = (j+1 < B && startBoard[i][j+1]=='P');
            plen = 1;
            if (horizP) {
                while (pc+plen < B && startBoard[pr][pc+plen]=='P') ++plen;
            } else {
                while (pr+plen < A && startBoard[pr+plen][pc]=='P') ++plen;
            }
            break;
        }
    }
}
if (pr < 0) {
    cerr << "Error: Tidak ada primary piece 'P' yang ditemukan\n";
    return 1;
}

// 4) Pastikan hanya ada tepat satu primary piece berurutan
int countPcells = 0;
for (int i = 0; i < A; ++i)
    for (int j = 0; j < B; ++j)
        if (startBoard[i][j] == 'P')
            ++countPcells;
if (countPcells != plen) {
    cerr << "Error: Harus tepat satu primary piece berurutan 'P' (panjang " 
         << plen << " sel), tapi ditemukan " << countPcells << "\n";
    valid = false;
}

// 5) Validasi bahwa exit sejajar dengan primary piece
if (horizP) {
    // primary horizontal → exit harus di baris pr
    if (ei != pr) {
        cerr << "Error: Exit harus sejajar horizontal pada baris " 
             << pr << "\n";
        valid = false;
    }
} else {
    // primary vertikal → exit harus di kolom pc
    if (ej != pc) {
        cerr << "Error: Exit harus sejajar vertikal pada kolom " 
             << pc << "\n";
        valid = false;
    }
}

if (!valid) {
    return 1;
}
*/

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
