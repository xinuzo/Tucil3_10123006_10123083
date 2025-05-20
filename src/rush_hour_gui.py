import pygame
import copy

# Konfigurasi GUI
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

def read_input(path):
    exit_pos = None
    moves = []
    with open(path) as f:
        # Read dimensions
        Y, X = map(int, f.readline().split())
        # Peek next line to check for a count line and read board lines accordingly
        peek = f.readline().rstrip('\n')
        try:
            # If it's an integer (e.g., number of pieces), skip it
            _ = int(peek.strip())
            lines = [f.readline().rstrip('\n') for _ in range(Y)]
        except ValueError:
            # The peek line is part of the board
            lines = [peek] + [f.readline().rstrip('\n') for _ in range(Y-1)]

        # Build the start board, padding or truncating each line to width X
        start_board = []
        for line in lines:
            row = list(line.ljust(X, '.')[:X])
            start_board.append(row)

        # Find the exit position marked by 'K'
        for i in range(Y):
            for j in range(X):
                if start_board[i][j] == 'K':
                    exit_pos = (i, j)
                    break
            if exit_pos:
                break

        # Read remaining lines as moves
        for line in f:
            moves.append(line.strip())

    return start_board, Y, X, exit_pos, moves

def do_move(current, move, Y, X):
    new_board = copy.deepcopy(current)
    for i in range(Y):
        for j in range(X):
            if current[i][j] == move[0]:
                piece_len = 1
                new_board[i][j] = '.'
                if move[2] == 'U' or move[2] == "D": # Vertical piece
                    while current[i+piece_len][j] == move[0]:
                        new_board[i+piece_len][j] = '.'
                        piece_len += 1
                    
                    # Get how far the piece needs to move
                    piece_move_dist = -1 if move[2] == 'U' else piece_len
                    while current[i+piece_move_dist][j] == '.':
                        piece_move_dist += -1 if move[2] == 'U' else 1
                        if not (i+piece_move_dist > 0 and i+piece_move_dist < Y): break
                    piece_move_dist -= -1 if move[2] == 'U' else 1
                    
                    # Apply move
                    for p in range(piece_len):
                        new_board[i + piece_move_dist + p*(1 if move[2] == 'U' else -1)][j] = move[0]
                    return new_board
                elif move[2] == 'R' or move[2] == 'L': # Horizontal piece
                    while current[i][j+piece_len] == move[0]:
                        new_board[i][j+piece_len] = '.'
                        piece_len += 1

                    # Get how far the piece needs to move
                    piece_move_dist = -1 if move[2] == 'L' else piece_len
                    while current[i][j+piece_move_dist] == '.':
                        piece_move_dist += -1 if move[2] == 'L' else 1
                        if not (j+piece_move_dist > 0 and j+piece_move_dist < X): break
                    piece_move_dist -= -1 if move[2] == 'L' else 1
                    
                    # Apply move
                    for p in range(piece_len):
                        new_board[i][j + piece_move_dist + p*(1 if move[2] == 'L' else -1)] = move[0]
                    return new_board

class RushHourGUI:
    def __init__(self, board, Y, X, exit_pos, moves):
        pygame.init()
        self.screen = pygame.display.set_mode((X*CELL_SIZE, Y*CELL_SIZE))
        pygame.display.set_caption("Rush Hour Solver")
        self.font = pygame.font.SysFont('Arial', 24)
        self.clock = pygame.time.Clock()
        
        self.boards = [board]
        self.solution = moves
        self.Y = Y
        self.X = X
        self.exit_pos = exit_pos
        self.current_step = 0
        self.running = True
        
        # Precompute semua board state
        current = board

        for move in moves:
            new = do_move(current, move, self.Y, self.X)
            current = new
            self.boards.append(new)

    def draw_board(self, board):
        for i in range(self.Y):
            for j in range(self.X):
                color = COLORS.get(board[i][j], (255, 255, 255))
                pygame.draw.rect(self.screen, color,
                    (j*CELL_SIZE + 2, i*CELL_SIZE + 2, CELL_SIZE-4, CELL_SIZE-4))
                if board[i][j] != '.':
                    text = self.font.render(board[i][j], True, (0,0,0))
                    self.screen.blit(text, 
                        (j*CELL_SIZE + 20, i*CELL_SIZE + 20))
        
        # Gambar exit
        if self.exit_pos:
            pygame.draw.rect(self.screen, (0,255,0),
                (self.exit_pos[1]*CELL_SIZE, self.exit_pos[0]*CELL_SIZE, CELL_SIZE, 5))

    def run(self):
        idx = 0
        while self.running:
            self.screen.fill((255, 255, 255))
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RIGHT and idx < len(self.boards)-1:
                        idx = min(len(self.boards), idx+1)
                    elif event.key == pygame.K_LEFT and idx > 0:
                        idx = max(0, idx-1)
            
            self.draw_board(self.boards[idx])
            pygame.display.set_caption(f"Rush Hour Solver. (Move {idx+1})")
            pygame.display.flip()
            self.clock.tick(30)
        
        pygame.quit()

if __name__ == "__main__":
    path = input("Masukkan path file input: ")
    start_board, Y, X, exit_pos, moves = read_input(path)

    gui = RushHourGUI(start_board, Y, X, exit_pos, moves)
    gui.run()
