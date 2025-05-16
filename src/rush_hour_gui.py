import pygame
import heapq
import sys
from collections import deque

# Konfigurasi GUI
WIDTH, HEIGHT = 800, 600
CELL_SIZE = 60
COLORS = {
    '.': (200, 200, 200),
    'P': (255, 0, 0),
    'K': (0, 255, 0),
    'A': (0, 0, 255),
    'B': (255, 255, 0),
    'C': (255, 165, 0),
    'D': (0, 255, 255),
    'F': (139, 69, 19),
    'G': (128, 0, 128),
    'H': (255, 192, 203),
    'I': (0, 128, 0),
    'J': (165, 42, 42),
    'L': (210, 180, 140),
    'M': (47, 79, 79),
}

# Variabel global
A = B = 0
exit_pos = None
start_board = []

class State:
    def __init__(self, board, moves=None, g=0, h=0):
        self.board = [row[:] for row in board]
        self.moves = moves.copy() if moves else []
        self.g = g
        self.h = h
    
    def __lt__(self, other):
        return (self.g + self.h) < (other.g + other.h)

def board_to_string(board):
    return ''.join([''.join(row) for row in board])

def find_primary(board):
    for i in range(A):
        for j in range(B):
            if board[i][j] == 'P':
                horizontal = (j + 1 < B and board[i][j+1] == 'P')
                length = 1
                if horizontal:
                    while j + length < B and board[i][j+length] == 'P':
                        length += 1
                else:
                    while i + length < A and board[i+length][j] == 'P':
                        length += 1
                return i, j, length, horizontal
    return -1, -1, 0, False

def heuristic(board):
    r, c, length, horizontal = find_primary(board)
    front = c + length - 1 if horizontal else r + length - 1
    if horizontal:
        exit_dist = exit_pos[1] - front - 1
    else:
        exit_dist = exit_pos[0] - front - 1
    return max(0, exit_dist)

def get_neighbors(state):
    board = state.board
    neighbors = []
    seen = set()
    
    for i in range(A):
        for j in range(B):
            ch = board[i][j]
            if ch == '.' or ch in seen:
                continue
            seen.add(ch)
            
            cells = []
            for x in range(A):
                for y in range(B):
                    if board[x][y] == ch:
                        cells.append((x, y))
            
            if len(cells) < 2:  # <--- TAMBAHKAN INI
                continue        # <--- TAMBAHKAN INI
                
            horizontal = cells[0][0] == cells[1][0]
            min_r = min(x for x, y in cells)
            max_r = max(x for x, y in cells)
            min_c = min(y for x, y in cells)
            max_c = max(y for x, y in cells)
            
            if horizontal:
                # Geser kiri
                steps = 0
                while min_c - steps - 1 >= 0 and board[min_r][min_c - steps - 1] == '.':
                    steps += 1
                if steps > 0:
                    new_board = [row[:] for row in board]
                    for y in range(min_c, max_c + 1):
                        new_board[min_r][y] = '.'
                    for y in range(min_c - steps, max_c - steps + 1):
                        new_board[min_r][y] = ch
                    new_moves = state.moves + [f"{ch}-L"]
                    neighbors.append(State(new_board, new_moves, state.g + 1, 0))
                
                # Geser kanan
                steps = 0
                while max_c + steps + 1 < B and board[min_r][max_c + steps + 1] == '.':
                    steps += 1
                if steps > 0:
                    new_board = [row[:] for row in board]
                    for y in range(min_c, max_c + 1):
                        new_board[min_r][y] = '.'
                    for y in range(min_c + steps, max_c + steps + 1):
                        new_board[min_r][y] = ch
                    new_moves = state.moves + [f"{ch}-R"]
                    neighbors.append(State(new_board, new_moves, state.g + 1, 0))
            
            else:
                # Geser atas
                steps = 0
                while min_r - steps - 1 >= 0 and board[min_r - steps - 1][min_c] == '.':
                    steps += 1
                if steps > 0:
                    new_board = [row[:] for row in board]
                    for x in range(min_r, max_r + 1):
                        new_board[x][min_c] = '.'
                    for x in range(min_r - steps, max_r - steps + 1):
                        new_board[x][min_c] = ch
                    new_moves = state.moves + [f"{ch}-U"]
                    neighbors.append(State(new_board, new_moves, state.g + 1, 0))
                
                # Geser bawah
                steps = 0
                while max_r + steps + 1 < A and board[max_r + steps + 1][min_c] == '.':
                    steps += 1
                if steps > 0:
                    new_board = [row[:] for row in board]
                    for x in range(min_r, max_r + 1):
                        new_board[x][min_c] = '.'
                    for x in range(min_r + steps, max_r + steps + 1):
                        new_board[x][min_c] = ch
                    new_moves = state.moves + [f"{ch}-D"]
                    neighbors.append(State(new_board, new_moves, state.g + 1, 0))
    
    return neighbors

def solve(algorithm):
    visited = set()
    initial = State(start_board, g=0, h=heuristic(start_board))
    heap = []
    heapq.heappush(heap, (initial.g + initial.h, initial))
    visited.add(board_to_string(initial.board))
    
    while heap:
        current_cost, current = heapq.heappop(heap)
        
        r, c, length, horizontal = find_primary(current.board)
        front = c + length - 1 if horizontal else r + length - 1
        if (horizontal and r == exit_pos[0] and front == exit_pos[1] - 1) or \
           (not horizontal and c == exit_pos[1] and front == exit_pos[0] - 1):
            return current.moves
        
        for neighbor in get_neighbors(current):
            neighbor_str = board_to_string(neighbor.board)
            if neighbor_str not in visited:
                visited.add(neighbor_str)
                neighbor.h = heuristic(neighbor.board)
                if algorithm == 1:
                    priority = neighbor.g
                elif algorithm == 2:
                    priority = neighbor.h
                else:
                    priority = neighbor.g + neighbor.h
                heapq.heappush(heap, (priority, neighbor))
    
    return []

def read_input(path):
    global A, B, exit_pos, start_board
    exit_pos = None
    with open(path) as f:
        A, B = map(int, f.readline().split())
        _ = f.readline()  # Skip N
        
        start_board = []
        for _ in range(A):
            line = f.readline().strip('\n').ljust(B, '.')[:B]
            start_board.append(list(line))
        
        # Cari K
        for i in range(A):
            for j in range(B):
                if start_board[i][j] == 'K':
                    exit_pos = (i, j)
                    break
            if exit_pos:
                break
    return start_board

class RushHourGUI:
    def __init__(self, board, solution):
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption("Rush Hour Solver")
        self.font = pygame.font.SysFont('Arial', 24)
        self.clock = pygame.time.Clock()
        
        self.boards = [board]
        self.solution = solution
        self.current_step = 0
        self.running = True
        
        # Precompute semua board state
        current = board
        for move in solution:
            for neighbor in get_neighbors(State(current)):
                if neighbor.moves and neighbor.moves[-1] == move:
                    self.boards.append(neighbor.board)
                    current = neighbor.board
                    break
    
    def draw_board(self, board):
        for i in range(A):
            for j in range(B):
                color = COLORS.get(board[i][j], (255, 255, 255))
                pygame.draw.rect(self.screen, color,
                    (j*CELL_SIZE + 2, i*CELL_SIZE + 2, CELL_SIZE-4, CELL_SIZE-4))
                if board[i][j] != '.':
                    text = self.font.render(board[i][j], True, (0,0,0))
                    self.screen.blit(text, 
                        (j*CELL_SIZE + 20, i*CELL_SIZE + 20))
        
        # Gambar exit
        pygame.draw.rect(self.screen, (0,255,0),
            (exit_pos[1]*CELL_SIZE, exit_pos[0]*CELL_SIZE, CELL_SIZE, 5))
    
    def run(self):
        idx = 0
        while self.running:
            self.screen.fill((255, 255, 255))
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RIGHT and idx < len(self.boards)-1:
                        idx += 1
                    elif event.key == pygame.K_LEFT and idx > 0:
                        idx -= 1
            
            self.draw_board(self.boards[idx])
            pygame.display.flip()
            self.clock.tick(30)
        
        pygame.quit()

if __name__ == "__main__":
    path = input("Enter input file path: ")
    read_input(path)
    
    if not exit_pos:
        print("Error: Exit (K) tidak ditemukan!")
        sys.exit(1)
    
    solution = solve(3)  # Gunakan A*
    print("Solusi ditemukan:", solution)
    
    if solution:
        gui = RushHourGUI(start_board, solution)
        gui.run()
    else:
        print("Tidak ada solusi yang ditemukan")
