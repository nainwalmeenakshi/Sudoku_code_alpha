#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <stack>
#include <set>

/**
 * @class Sudoku
 * @brief Core class implementing Sudoku puzzle operations using OOP and backtracking.
 *
 * Features:
 * - 9x9 board representation using vector<vector<int>>
 * - Recursive backtracking solver
 * - Puzzle validation, generation, hints
 * - Statistics tracking and difficulty detection
 * - Undo/Redo support for step-by-step solving
 */
class Sudoku {
private:
    static constexpr int SIZE = 9;           ///< Board size (9x9)
    static constexpr int SUBGRID = 3;        ///< Subgrid size (3x3)
    static constexpr int EMPTY = 0;          ///< Empty cell marker

    std::vector<std::vector<int>> board;      ///< Current board state
    std::vector<std::vector<int>> originalBoard; ///< Original board for reset

    // Statistics
    int recursiveCalls;                       ///< Total recursive calls made
    int backtracks;                           ///< Total backtrack operations
    double solveTime;                         ///< Time taken to solve (seconds)
    int emptyCells;                           ///< Number of empty cells in puzzle

    // Step-by-step solver
    struct Step {
        int row, col, value;
        bool isBacktrack;
    };
    std::vector<Step> solveSteps;            ///< Stores each step for animation
    size_t currentStepIndex;                 ///< Current step index for replay

    // Puzzle generation
    std::mt19937 rng;                        ///< Random number generator

    // Undo/Redo
    std::stack<std::vector<std::vector<int>>> undoStack;
    std::stack<std::vector<std::vector<int>>> redoStack;

    /**
     * @brief Check if placing a number is valid according to Sudoku rules.
     * @param row Row index
     * @param col Column index
     * @param num Number to place (1-9)
     * @return true if placement is valid
     */
    bool isSafe(int row, int col, int num) const;

    /**
     * @brief Find the next empty cell on the board.
     * @param row Output parameter for row
     * @param col Output parameter for column
     * @return true if an empty cell was found
     */
    bool findEmptyCell(int& row, int& col) const;

    /**
     * @brief Recursive backtracking solver (internal).
     * @param recordSteps Whether to record steps for animation
     * @return true if solution was found
     */
    bool solveRecursive(bool recordSteps = false);

    /**
     * @brief Count solutions for uniqueness checking.
     * @param limit Maximum solutions to count
     * @return Number of solutions found
     */
    int countSolutions(int limit = 2);

    /**
     * @brief Generate a complete valid Sudoku board.
     */
    void generateCompleteBoard();

    /**
     * @brief Remove cells from a complete board to create a puzzle.
     * @param cellsToRemove Number of cells to remove
     */
    void removeCells(int cellsToRemove);

public:
    /**
     * @brief Constructor - initializes an empty board.
     */
    Sudoku();

    /**
     * @brief Reset board to its initial state.
     */
    void resetBoard();

    /**
     * @brief Clear the entire board.
     */
    void clearBoard();

    /**
     * @brief Input board manually from console.
     */
    void inputBoard();

    /**
     * @brief Display the board in a formatted grid with ANSI colors.
     * @param highlightInvalid Whether to highlight invalid cells
     */
    void displayBoard(bool highlightInvalid = false) const;

    /**
     * @brief Validate the entire board against Sudoku rules.
     * @return true if board is valid
     */
    bool validateBoard() const;

    /**
     * @brief Validate a single cell.
     * @param row Row index
     * @param col Column index
     * @return true if cell is valid
     */
    bool validateCell(int row, int col) const;

    /**
     * @brief Solve the puzzle using backtracking.
     * @param stepByStep Whether to record steps for animation
     * @return true if puzzle was solved
     */
    bool solve(bool stepByStep = false);

    /**
     * @brief Get the next step in the step-by-step solution.
     * @return Pair of (row, col) for the next step, or (-1, -1) if none
     */
    std::pair<int, int> getNextStep();

    /**
     * @brief Apply the next step in the solution.
     * @return true if a step was applied
     */
    bool applyNextStep();

    /**
     * @brief Check if step-by-step solver has more steps.
     * @return true if more steps available
     */
    bool hasMoreSteps() const;

    /**
     * @brief Reset step-by-step solver.
     */
    void resetStepSolver();

    /**
     * @brief Generate a puzzle with specified difficulty.
     * @param difficulty "easy", "medium", "hard", or "expert"
     */
    void generatePuzzle(const std::string& difficulty = "medium");

    /**
     * @brief Give the player a hint by filling one correct cell.
     * @return true if a hint was given
     */
    bool giveHint();

    /**
     * @brief Save the current board to a file.
     * @param filename Path to the file
     * @return true if save was successful
     */
    bool savePuzzle(const std::string& filename) const;

    /**
     * @brief Load a puzzle from a file.
     * @param filename Path to the file
     * @return true if load was successful
     */
    bool loadPuzzle(const std::string& filename);

    /**
     * @brief Get the current board data.
     * @return Reference to the board
     */
    const std::vector<std::vector<int>>& getBoard() const { return board; }

    /**
     * @brief Set a value at a specific cell.
     * @param row Row index
     * @param col Column index
     * @param value Value to set (0-9)
     */
    void setCell(int row, int col, int value);

    /**
     * @brief Get value at a specific cell.
     * @param row Row index
     * @param col Column index
     * @return Cell value
     */
    int getCell(int row, int col) const;

    /**
     * @brief Show solving statistics.
     */
    void showStatistics() const;

    /**
     * @brief Detect puzzle difficulty.
     * @return String representing difficulty
     */
    std::string detectDifficulty() const;

    /**
     * @brief Undo last move.
     * @return true if undo was performed
     */
    bool undo();

    /**
     * @brief Redo last undone move.
     * @return true if redo was performed
     */
    bool redo();

    /**
     * @brief Check if the board is solved.
     * @return true if board is completely and correctly filled
     */
    bool isSolved() const;

    /**
     * @brief Get the original board state.
     */
    const std::vector<std::vector<int>>& getOriginalBoard() const { return originalBoard; }

    /**
     * @brief Get statistics data.
     */
    int getRecursiveCalls() const { return recursiveCalls; }
    int getBacktracks() const { return backtracks; }
    double getSolveTime() const { return solveTime; }
    int getEmptyCells() const { return emptyCells; }

    // Board size constants
    static constexpr int getSize() { return SIZE; }
    static constexpr int getSubgrid() { return SUBGRID; }
};

#endif // SUDOKU_H

