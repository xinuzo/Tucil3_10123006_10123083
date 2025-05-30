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
        Y, X = map(int, f.readline().split())
        
        start_board = []
        for _ in range(Y):
            line = f.readline().strip('\n')
            start_board.append(list(line))

        # Cari K
        for i in range(len(start_board)):
            for j in range(len(start_board[i])):
                if start_board[i][j] == 'K':
                    exit_pos = (i, j)
                    break
            if exit_pos:
                break
        
        # No K = K next line
        if exit_pos is None:
            start_board.append(list(f.readline().strip('\n')))
            i = len(start_board)-1
            for j in range(len(start_board[i])):
                if start_board[i][j] == 'K':
                    exit_pos = (i, j)
                    break


        Y = max(exit_pos[0]+1, Y)
        X = max(exit_pos[1]+1, X)

        for i in range(len(start_board)):
            if len(start_board[i]) < X:
                for _ in range(len(start_board[i]), X):
                    start_board[i].append(' ')
    
        for line in f:
            moves.append(line)
    
    
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
                        if i+piece_len >= Y: break
                    
                    # Get how far the piece needs to move
                    piece_move_dist = -1 if move[2] == 'U' else piece_len
                    while current[i+piece_move_dist][j] == '.':
                        piece_move_dist += -1 if move[2] == 'U' else 1
                        if not (i+piece_move_dist >= 0 and i+piece_move_dist < Y): break
                    piece_move_dist -= -1 if move[2] == 'U' else 1
                    
                    # Apply move
                    for p in range(piece_len):
                        new_board[i + piece_move_dist + p*(1 if move[2] == 'U' else -1)][j] = move[0]
                    return new_board
                elif move[2] == 'R' or move[2] == 'L': # Horizontal piece
                    while current[i][j+piece_len] == move[0]:
                        new_board[i][j+piece_len] = '.'
                        piece_len += 1
                        if j+piece_len >= X: break

                    # Get how far the piece needs to move
                    piece_move_dist = -1 if move[2] == 'L' else piece_len
                    while current[i][j+piece_move_dist] == '.':
                        piece_move_dist += -1 if move[2] == 'L' else 1
                        if not (j+piece_move_dist >= 0 and j+piece_move_dist < X): break
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
    path = input("Enter input file path: ")
    start_board, Y, X, exit_pos, moves = read_input(path)

    gui = RushHourGUI(start_board, Y, X, exit_pos, moves)
    gui.run()