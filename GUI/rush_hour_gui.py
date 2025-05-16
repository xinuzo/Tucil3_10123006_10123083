import pygame
import heapq
from datetime import datetime
import sys

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
    exit_dist = exit_pos[1] - front - 1 if horizontal else exit_pos[0] - front - 1
    return max(0, exit_dist)

def get_neighbors(state):
    # ... (Implementasi sama seperti versi C++ diubah ke Python)
    # Lihat kode lengkap di github repo berikut

def solve(algorithm):
    # ... (Implementasi algoritma solver sama seperti C++)

class RushHourGUI:
    def __init__(self, board, solution):
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.font = pygame.font.SysFont('Arial', 24)
        self.clock = pygame.time.Clock()
        
        self.boards = [board]
        self.solution = solution
        self.current_step = 0
        self.running = True

    def draw_board(self, board):
        for i in range(A):
            for j in range(B):
                color = COLORS.get(board[i][j], (255, 255, 255))
                pygame.draw.rect(self.screen, color, 
                    (j*CELL_SIZE, i*CELL_SIZE, CELL_SIZE, CELL_SIZE))
                if board[i][j] != '.':
                    text = self.font.render(board[i][j], True, (0,0,0))
                    self.screen.blit(text, 
                        (j*CELL_SIZE + 20, i*CELL_SIZE + 20))

    def run(self):
        while self.running:
            self.screen.fill((255, 255, 255))
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RIGHT and self.current_step < len(self.solution):
                        # Update board sesuai gerakan
                        pass
            
            self.draw_board(self.boards[self.current_step])
            pygame.display.flip()
            self.clock.tick(30)

if __name__ == "__main__":
    # Baca input dan selesaikan puzzle
    with open('tcase1.txt') as f:
        A, B = map(int, f.readline().split())
        N = int(f.readline())
        start_board = [list(f.readline().strip()) for _ in range(A)]
    
    # Temukan exit position
    exit_pos = (-1, -1)
    for i in range(A):
        for j in range(B):
            if start_board[i][j] == 'K':
                exit_pos = (i, j)
    
    # Pilih algoritma dan selesaikan
    algorithm = 3  # A*
    solution = solve(algorithm)
    
    # Jalankan GUI
    gui = RushHourGUI(start_board, solution)
    gui.run()