#include "Sudoku.h"

// ============================================================================
// Constructor
// ============================================================================
Sudoku::Sudoku()
    : board(SIZE, std::vector<int>(SIZE, EMPTY))
    , originalBoard(SIZE, std::vector<int>(SIZE, EMPTY))
    , recursiveCalls(0)
    , backtracks(0)
    , solveTime(0.0)
    , emptyCells(SIZE * SIZE)
    , currentStepIndex(0)
    , rng(std::random_device{}())
{
}

// ============================================================================
// Board Management
// ============================================================================
void Sudoku::resetBoard() {
    board = originalBoard;
    emptyCells = 0;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) ++emptyCells;
        }
    }
    recursiveCalls = 0;
    backtracks = 0;
    solveTime = 0.0;
    solveSteps.clear();
    currentStepIndex = 0;
}

void Sudoku::clearBoard() {
    for (int r = 0; r < SIZE; ++r) {
        std::fill(board[r].begin(), board[r].end(), EMPTY);
    }
    originalBoard = board;
    emptyCells = SIZE * SIZE;
    recursiveCalls = 0;
    backtracks = 0;
    solveTime = 0.0;
}

void Sudoku::inputBoard() {
    std::cout << "\n=== Enter Sudoku Puzzle ===\n";
    std::cout << "Enter 9 rows. Use 0 for empty cells.\n";
    std::cout << "Separate numbers with spaces or newlines.\n\n";

    for (int r = 0; r < SIZE; ++r) {
        std::cout << "Row " << (r + 1) << ": ";
        for (int c = 0; c < SIZE; ++c) {
            int val;
            std::cin >> val;
            if (val < 0 || val > 9) {
                std::cerr << "Invalid input! Using 0.\n";
                val = 0;
            }
            board[r][c] = val;
        }
    }

    originalBoard = board;
    emptyCells = 0;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) ++emptyCells;
        }
    }
    std::cout << "Puzzle loaded successfully!\n";
}

// ============================================================================
// Display
// ============================================================================
void Sudoku::displayBoard(bool highlightInvalid) const {
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";

    std::cout << "\n" << BOLD << "     Sudoku Board" << RESET << "\n";
    std::cout << "  " << CYAN << "+-------+-------+-------+" << RESET << "\n";

    for (int r = 0; r < SIZE; ++r) {
        std::cout << " " << CYAN << "|" << RESET << " ";
        for (int c = 0; c < SIZE; ++c) {
            int val = board[r][c];
            bool isOriginal = (originalBoard[r][c] != EMPTY);
            bool invalid = highlightInvalid && !validateCell(r, c) && val != EMPTY;

            if (val == EMPTY) {
                std::cout << ". ";
            } else if (invalid) {
                std::cout << RED << val << " " << RESET;
            } else if (isOriginal) {
                std::cout << BOLD << YELLOW << val << " " << RESET;
            } else {
                std::cout << GREEN << val << " " << RESET;
            }

            if ((c + 1) % SUBGRID == 0) {
                std::cout << CYAN << "|" << RESET << " ";
            }
        }
        std::cout << "\n";
        if ((r + 1) % SUBGRID == 0) {
            std::cout << "  " << CYAN << "+-------+-------+-------+" << RESET << "\n";
        }
    }
    std::cout << "\n";
}

// ============================================================================
// Validation
// ============================================================================
bool Sudoku::validateCell(int row, int col) const {
    int val = board[row][col];
    if (val == EMPTY) return true;

    // Check row
    for (int c = 0; c < SIZE; ++c) {
        if (c != col && board[row][c] == val) return false;
    }
    // Check column
    for (int r = 0; r < SIZE; ++r) {
        if (r != row && board[r][col] == val) return false;
    }
    // Check 3x3 box
    int boxRow = (row / SUBGRID) * SUBGRID;
    int boxCol = (col / SUBGRID) * SUBGRID;
    for (int r = boxRow; r < boxRow + SUBGRID; ++r) {
        for (int c = boxCol; c < boxCol + SUBGRID; ++c) {
            if ((r != row || c != col) && board[r][c] == val) return false;
        }
    }
    return true;
}

bool Sudoku::validateBoard() const {
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] != EMPTY && !validateCell(r, c)) {
                return false;
            }
        }
    }
    return true;
}

// ============================================================================
// Backtracking Solver
// ============================================================================
bool Sudoku::isSafe(int row, int col, int num) const {
    // Check row
    for (int c = 0; c < SIZE; ++c) {
        if (board[row][c] == num) return false;
    }
    // Check column
    for (int r = 0; r < SIZE; ++r) {
        if (board[r][col] == num) return false;
    }
    // Check 3x3 box
    int boxRow = (row / SUBGRID) * SUBGRID;
    int boxCol = (col / SUBGRID) * SUBGRID;
    for (int r = boxRow; r < boxRow + SUBGRID; ++r) {
        for (int c = boxCol; c < boxCol + SUBGRID; ++c) {
            if (board[r][c] == num) return false;
        }
    }
    return true;
}

bool Sudoku::findEmptyCell(int& row, int& col) const {
    for (row = 0; row < SIZE; ++row) {
        for (col = 0; col < SIZE; ++col) {
            if (board[row][col] == EMPTY) return true;
        }
    }
    return false;
}

bool Sudoku::solveRecursive(bool recordSteps) {
    ++recursiveCalls;

    int row, col;
    if (!findEmptyCell(row, col)) {
        return true;
    }

    for (int num = 1; num <= 9; ++num) {
        if (isSafe(row, col, num)) {
            board[row][col] = num;

            if (recordSteps) {
                solveSteps.push_back({row, col, num, false});
            }

            if (solveRecursive(recordSteps)) {
                return true;
            }

            // Backtrack
            board[row][col] = EMPTY;
            ++backtracks;

            if (recordSteps) {
                solveSteps.push_back({row, col, EMPTY, true});
            }
        }
    }
    return false;
}

bool Sudoku::solve(bool stepByStep) {
    if (!validateBoard()) {
        std::cerr << "Error: Invalid board configuration!\n";
        return false;
    }

    recursiveCalls = 0;
    backtracks = 0;
    solveSteps.clear();
    currentStepIndex = 0;

    // Count empty cells
    emptyCells = 0;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) ++emptyCells;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    bool solved = solveRecursive(stepByStep);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    solveTime = duration.count() / 1000000.0;

    if (solved) {
        std::cout << "Puzzle solved successfully!\n";
        std::cout << "Time: " << std::fixed << std::setprecision(4) << solveTime << " seconds\n";
        std::cout << "Recursive calls: " << recursiveCalls << "\n";
        std::cout << "Backtracks: " << backtracks << "\n";
    } else {
        std::cout << "No solution exists for this puzzle.\n";
    }

    return solved;
}

// ============================================================================
// Step-by-Step Solver
// ============================================================================
std::pair<int, int> Sudoku::getNextStep() {
    if (currentStepIndex < solveSteps.size()) {
        const auto& step = solveSteps[currentStepIndex];
        return {step.row, step.col};
    }
    return {-1, -1};
}

bool Sudoku::applyNextStep() {
    if (currentStepIndex < solveSteps.size()) {
        const auto& step = solveSteps[currentStepIndex];
        if (!step.isBacktrack) {
            board[step.row][step.col] = step.value;
        } else {
            board[step.row][step.col] = EMPTY;
        }
        ++currentStepIndex;
        return true;
    }
    return false;
}

bool Sudoku::hasMoreSteps() const {
    return currentStepIndex < solveSteps.size();
}

void Sudoku::resetStepSolver() {
    currentStepIndex = 0;
    board = originalBoard;
}

// ============================================================================
// Puzzle Generation
// ============================================================================
void Sudoku::generateCompleteBoard() {
    clearBoard();
    // Fill diagonal 3x3 boxes first (they are independent)
    for (int box = 0; box < SIZE; box += SUBGRID) {
        std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::shuffle(nums.begin(), nums.end(), rng);
        int idx = 0;
        for (int r = box; r < box + SUBGRID; ++r) {
            for (int c = box; c < box + SUBGRID; ++c) {
                board[r][c] = nums[idx++];
            }
        }
    }
    // Solve the rest using backtracking
    recursiveCalls = 0;
    backtracks = 0;
    solveRecursive(false);
    originalBoard = board;
}

int Sudoku::countSolutions(int limit) {
    int row, col;
    if (!findEmptyCell(row, col)) {
        return 1; // Found a complete solution
    }

    int count = 0;
    for (int num = 1; num <= 9; ++num) {
        if (isSafe(row, col, num)) {
            board[row][col] = num;
            count += countSolutions(limit - count);
            board[row][col] = EMPTY;
            if (count >= limit) break;
        }
    }
    return count;
}

void Sudoku::removeCells(int cellsToRemove) {
    std::vector<std::pair<int, int>> positions;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            positions.push_back({r, c});
        }
    }
    std::shuffle(positions.begin(), positions.end(), rng);

    int removed = 0;
    for (const auto& pos : positions) {
        if (removed >= cellsToRemove) break;

        int backup = board[pos.first][pos.second];
        board[pos.first][pos.second] = EMPTY;

        // Check if puzzle still has a unique solution
        int solutions = countSolutions(2);
        if (solutions == 1) {
            ++removed;
        } else {
            board[pos.first][pos.second] = backup; // Restore
        }
    }
    emptyCells = removed;
}

void Sudoku::generatePuzzle(const std::string& difficulty) {
    int cellsToRemove;
    if (difficulty == "easy") {
        cellsToRemove = 30;
    } else if (difficulty == "medium") {
        cellsToRemove = 40;
    } else if (difficulty == "hard") {
        cellsToRemove = 50;
    } else if (difficulty == "expert") {
        cellsToRemove = 55;
    } else {
        cellsToRemove = 40;
    }

    generateCompleteBoard();
    removeCells(cellsToRemove);
    originalBoard = board;
}

// ============================================================================
// Hint System
// ============================================================================
bool Sudoku::giveHint() {
    // Find all empty cells
    std::vector<std::pair<int, int>> emptyCellsList;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) {
                emptyCellsList.push_back({r, c});
            }
        }
    }

    if (emptyCellsList.empty()) return false;

    // Solve a copy of the board
    auto boardCopy = board;
    auto savedRecursive = recursiveCalls;
    auto savedBacktracks = backtracks;

    // Temporarily solve to get hint
    board = boardCopy;
    recursiveCalls = 0;
    backtracks = 0;
    solveRecursive(false);

    // Pick a random empty cell and fill it with the solution value
    std::shuffle(emptyCellsList.begin(), emptyCellsList.end(), rng);
    for (const auto& cell : emptyCellsList) {
        int solution = board[cell.first][cell.second];
        boardCopy[cell.first][cell.second] = solution;
        board = boardCopy;
        if (validateCell(cell.first, cell.second)) {
            board = boardCopy;
            recursiveCalls = savedRecursive;
            backtracks = savedBacktracks;
            --emptyCells;
            std::cout << "Hint: Place " << solution << " at position ("
                      << (cell.first + 1) << ", " << (cell.second + 1) << ")\n";
            return true;
        }
        boardCopy[cell.first][cell.second] = EMPTY;
    }

    board = boardCopy;
    recursiveCalls = savedRecursive;
    backtracks = savedBacktracks;
    return false;
}

// ============================================================================
// Cell Access
// ============================================================================
void Sudoku::setCell(int row, int col, int value) {
    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && value >= 0 && value <= 9) {
        // Save state for undo
        undoStack.push(board);
        while (!redoStack.empty()) redoStack.pop();

        board[row][col] = value;
        if (value == EMPTY) {
            ++emptyCells;
        } else {
            if (originalBoard[row][col] == EMPTY) --emptyCells;
        }
    }
}

int Sudoku::getCell(int row, int col) const {
    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        return board[row][col];
    }
    return -1;
}

// ============================================================================
// Undo / Redo
// ============================================================================
bool Sudoku::undo() {
    if (undoStack.empty()) return false;
    redoStack.push(board);
    board = undoStack.top();
    undoStack.pop();
    return true;
}

bool Sudoku::redo() {
    if (redoStack.empty()) return false;
    undoStack.push(board);
    board = redoStack.top();
    redoStack.pop();
    return true;
}

// ============================================================================
// Utility
// ============================================================================
bool Sudoku::isSolved() const {
    if (!validateBoard()) return false;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) return false;
        }
    }
    return true;
}

void Sudoku::showStatistics() const {
    std::cout << "\n========== Statistics ==========\n";
    std::cout << "Solving Time:       " << std::fixed << std::setprecision(4) << solveTime << " seconds\n";
    std::cout << "Recursive Calls:    " << recursiveCalls << "\n";
    std::cout << "Backtracking Count: " << backtracks << "\n";
    std::cout << "Empty Cells:        " << emptyCells << "\n";
    std::cout << "Filled Cells:       " << (SIZE * SIZE - emptyCells) << "\n";
    std::cout << "Difficulty:         " << detectDifficulty() << "\n";
    std::cout << "Solved:             " << (isSolved() ? "Yes" : "No") << "\n";
    std::cout << "================================\n\n";
}

std::string Sudoku::detectDifficulty() const {
    int empty = 0;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) ++empty;
        }
    }

    if (empty <= 30) return "Easy";
    if (empty <= 40) return "Medium";
    if (empty <= 50) return "Hard";
    return "Expert";
}

bool Sudoku::savePuzzle(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << "\n";
        return false;
    }

    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            file << board[r][c];
            if (c < SIZE - 1) file << " ";
        }
        file << "\n";
    }

    file.close();
    return true;
}

bool Sudoku::loadPuzzle(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return false;
    }

    std::vector<std::vector<int>> newBoard(SIZE, std::vector<int>(SIZE, EMPTY));
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < SIZE) {
        // Skip comments
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int val;
        int col = 0;
        while (iss >> val && col < SIZE) {
            if (val >= 0 && val <= 9) {
                newBoard[row][col] = val;
            }
            ++col;
        }
        ++row;
    }

    file.close();

    if (row < SIZE) {
        std::cerr << "Error: Invalid puzzle format. Expected 9 rows.\n";
        return false;
    }

    board = newBoard;
    originalBoard = newBoard;
    emptyCells = 0;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (board[r][c] == EMPTY) ++emptyCells;
        }
    }

    std::cout << "Puzzle loaded successfully from " << filename << "\n";
    return true;
}
