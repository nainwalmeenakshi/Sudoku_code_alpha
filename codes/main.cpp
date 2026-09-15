/**
 * @file main.cpp
 * @brief Professional Sudoku Solver System - Main Entry Point
 *
 * Features:
 * - Console-based interactive menu
 * - Keyboard shortcuts for quick actions
 * - Light/Dark theme support
 * - User profile management
 * - Leaderboard tracking
 * - Step-by-step solving animation
 * - Comprehensive error handling
 *
 * Compilation:
 *   g++ -std=c++17 *.cpp -o SudokuSolver
 *   ./SudokuSolver
 *
 * @author Sudoku Solver Team
 * @version 2.0.0
 * @license MIT
 */

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cctype>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "Sudoku.h"
#include "FileManager.h"

// ============================================================================
// Global Constants
// ============================================================================
const std::string VERSION = "2.0.0";
const std::string APP_NAME = "Professional Sudoku Solver";

// ============================================================================
// Theme Support
// ============================================================================
struct Theme {
    std::string bg;
    std::string fg;
    std::string accent;
    std::string success;
    std::string error;
    std::string warning;
    std::string header;
    std::string reset;
};

Theme lightTheme = {
    "\033[47m",   // bg - white
    "\033[30m",   // fg - black
    "\033[34m",   // accent - blue
    "\033[32m",   // success - green
    "\033[31m",   // error - red
    "\033[33m",   // warning - yellow
    "\033[1;36m", // header - cyan bold
    "\033[0m"     // reset
};

Theme darkTheme = {
    "\033[40m",   // bg - black
    "\033[37m",   // fg - white
    "\033[36m",   // accent - cyan
    "\033[92m",   // success - bright green
    "\033[91m",   // error - bright red
    "\033[93m",   // warning - bright yellow
    "\033[1;35m", // header - magenta bold
    "\033[0m"     // reset
};

Theme currentTheme = darkTheme;

// ============================================================================
// Global Objects
// ============================================================================
Sudoku sudoku;
FileManager fileManager;
std::string username = "Player";
int totalSolved = 0;
double fastestSolve = 999999.0;
double totalTime = 0.0;
int hintsUsed = 0;

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Clear the terminal screen (cross-platform).
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Print a centered header with decorations.
 * @param text Text to display in header
 */
void printHeader(const std::string& text) {
    std::cout << currentTheme.header;
    std::cout << "\n  ╔══════════════════════════════════════════════════╗\n";
    std::cout << "  ║        " << text;
    // Padding to align
    int padding = 46 - text.length();
    if (padding > 0) std::cout << std::string(padding, ' ');
    std::cout << "║\n";
    std::cout << "  ╚══════════════════════════════════════════════════╝\n";
    std::cout << currentTheme.reset;
}

/**
 * @brief Print a success message.
 */
void printSuccess(const std::string& msg) {
    std::cout << currentTheme.success << "  ✓ " << msg << currentTheme.reset << "\n";
}

/**
 * @brief Print an error message.
 */
void printError(const std::string& msg) {
    std::cout << currentTheme.error << "  ✗ " << msg << currentTheme.reset << "\n";
}

/**
 * @brief Print an info message.
 */
void printInfo(const std::string& msg) {
    std::cout << currentTheme.accent << "  ℹ " << msg << currentTheme.reset << "\n";
}

/**
 * @brief Print a warning message.
 */
void printWarning(const std::string& msg) {
    std::cout << currentTheme.warning << "  ⚠ " << msg << currentTheme.reset << "\n";
}

/**
 * @brief Apply theme to the whole terminal.
 */
void applyTheme() {
    std::cout << currentTheme.bg << currentTheme.fg;
}

/**
 * @brief Reset terminal colors.
 */
void resetTheme() {
    std::cout << "\033[0m";
}

/**
 * @brief Check if a key was pressed (non-blocking).
 * @return Character pressed, or -1 if none
 */
int getKeyPress() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        return ch;
    }
    return -1;
}

/**
 * @brief Pause and wait for Enter key.
 */
void pressEnterToContinue() {
    std::cout << "\n  " << currentTheme.accent << "Press Enter to continue..." << currentTheme.reset;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

/**
 * @brief Wait for a specified duration with animation.
 * @param ms Milliseconds to wait
 */
void waitWithAnimation(int ms) {
    std::cout << currentTheme.accent << "  ";
    for (int i = 0; i < 10; ++i) {
        std::cout << "▮";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms / 10));
    }
    std::cout << currentTheme.reset << "\n";
}

// ============================================================================
// Menu Functions
// ============================================================================

/**
 * @brief Display the main menu.
 */
void displayMenu() {
    clearScreen();
    printHeader(APP_NAME + " v" + VERSION);

    std::cout << currentTheme.fg;
    std::cout << "\n  ┌──────────────────────────────────────────────────────┐\n";
    std::cout << "  │  " << currentTheme.accent << "1." << currentTheme.fg << "  Enter Sudoku Manually        " << currentTheme.warning << "[M]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "2." << currentTheme.fg << "  Load Sudoku From File         " << currentTheme.warning << "[L]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "3." << currentTheme.fg << "  Display Sudoku                " << currentTheme.warning << "[D]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "4." << currentTheme.fg << "  Validate Puzzle              " << currentTheme.warning << "[V]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "5." << currentTheme.fg << "  Solve Puzzle                  " << currentTheme.warning << "[S]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "6." << currentTheme.fg << "  Hint                          " << currentTheme.warning << "[H]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "7." << currentTheme.fg << "  Generate Puzzle               " << currentTheme.warning << "[G]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "8." << currentTheme.fg << "  Save Solution                 " << currentTheme.warning << "[O]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "9." << currentTheme.fg << "  View Statistics               " << currentTheme.warning << "[T]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "10." << currentTheme.fg << " Reset Puzzle                  " << currentTheme.warning << "[R]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "11." << currentTheme.fg << " Settings                      " << currentTheme.warning << "[C]" << currentTheme.fg << "    │\n";
    std::cout << "  │  " << currentTheme.accent << "12." << currentTheme.fg << " Exit                          " << currentTheme.warning << "[Q]" << currentTheme.fg << "    │\n";
    std::cout << "  └──────────────────────────────────────────────────────┘\n";
    std::cout << "\n  " << currentTheme.accent << "User: " << currentTheme.success << username 
              << currentTheme.accent << "  |  Solved: " << currentTheme.success << totalSolved 
              << currentTheme.accent << "  |  Fastest: " << currentTheme.success 
              << (fastestSolve < 999999.0 ? std::to_string(fastestSolve) + "s" : "N/A") 
              << currentTheme.reset << "\n";
    std::cout << "\n  " << currentTheme.accent << "Enter choice (1-12 or shortcut key): " << currentTheme.reset;
}

/**
 * @brief Settings menu.
 */
void settingsMenu() {
    clearScreen();
    printHeader("Settings");

    std::cout << currentTheme.fg;
    std::cout << "\n  1. Toggle Theme (Current: " 
              << (currentTheme.bg == darkTheme.bg ? "Dark" : "Light") << ")\n";
    std::cout << "  2. Change Username (Current: " << username << ")\n";
    std::cout << "  3. View Leaderboard\n";
    std::cout << "  4. View History\n";
    std::cout << "  5. View User Profile\n";
    std::cout << "  6. View Logs\n";
    std::cout << "  7. Back to Main Menu\n";
    std::cout << "\n  Enter choice: " << currentTheme.reset;

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1: {
            if (currentTheme.bg == darkTheme.bg) {
                currentTheme = lightTheme;
                fileManager.setConfig("theme", "light");
                printSuccess("Switched to Light Theme");
            } else {
                currentTheme = darkTheme;
                fileManager.setConfig("theme", "dark");
                printSuccess("Switched to Dark Theme");
            }
            waitWithAnimation(500);
            break;
        }
        case 2: {
            std::cout << "\n  Enter new username: ";
            std::cin >> username;
            fileManager.setConfig("username", username);
            printSuccess("Username updated to: " + username);
            waitWithAnimation(500);
            break;
        }
        case 3:
            fileManager.displayLeaderboard();
            pressEnterToContinue();
            break;
        case 4:
            fileManager.displayHistory();
            pressEnterToContinue();
            break;
        case 5: {
            auto profile = fileManager.loadProfile(username);
            std::cout << "\n  User Profile: " << username << "\n";
            if (profile.size() >= 5) {
                std::cout << "  Total Solved: " << profile[1] << "\n";
                std::cout << "  Average Time: " << profile[2] << "s\n";
                std::cout << "  Fastest Solve: " << profile[3] << "s\n";
                std::cout << "  Hints Used: " << profile[4] << "\n";
            }
            pressEnterToContinue();
            break;
        }
        case 6: {
            std::ifstream logFile("logs.txt");
            if (logFile.is_open()) {
                std::cout << "\n  Recent Logs:\n";
                std::string line;
                int count = 0;
                while (std::getline(logFile, line) && count < 20) {
                    std::cout << "  " << line << "\n";
                    ++count;
                }
                logFile.close();
            } else {
                printInfo("No logs available.");
            }
            pressEnterToContinue();
            break;
        }
        case 7:
            return;
        default:
            printError("Invalid choice!");
            waitWithAnimation(500);
    }
}

// ============================================================================
// Main Application
// ============================================================================

/**
 * @brief Run the step-by-step solver with animation.
 */
void stepByStepSolve() {
    clearScreen();
    printHeader("Step-by-Step Solver");

    std::cout << currentTheme.fg;
    std::cout << "\n  Solving with animation...\n\n" << currentTheme.reset;

    // First, solve and record steps
    sudoku.resetBoard();
    sudoku.solve(true);

    // Reset to original for playback
    sudoku.resetStepSolver();

    while (sudoku.hasMoreSteps()) {
        clearScreen();
        printHeader("Solving... (Press Q to skip)");

        sudoku.applyNextStep();
        sudoku.displayBoard(true);

        std::cout << currentTheme.accent;
        std::cout << "  Step " << (sudoku.getNextStep().first != -1 ? 
                  "at position (" + std::to_string(sudoku.getNextStep().first + 1) + "," 
                  + std::to_string(sudoku.getNextStep().second + 1) + ")" : "Final") << "\n";
        std::cout << currentTheme.reset;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check for skip key
        int key = getKeyPress();
        if (key == 'q' || key == 'Q') break;
    }

    // Show final result
    clearScreen();
    printHeader("Solution Complete!");
    sudoku.displayBoard();
    sudoku.showStatistics();
    pressEnterToContinue();
}

/**
 * @brief Main application entry point.
 */
int main() {
    // Load configuration
    std::string themeSetting = fileManager.getConfig("theme", "dark");
    if (themeSetting == "light") {
        currentTheme = lightTheme;
    }

    username = fileManager.getConfig("username", "Player");
    if (username.empty()) username = "Player";

    // Load user profile
    auto profile = fileManager.loadProfile(username);
    if (profile.size() >= 5) {
        totalSolved = std::stoi(profile[1]);
        totalTime = std::stod(profile[2]) * totalSolved;
        fastestSolve = std::stod(profile[3]);
        hintsUsed = std::stoi(profile[4]);
    }

    fileManager.log("Application started by: " + username);

    // Main loop
    while (true) {
        displayMenu();

        std::string input;
        std::cin >> input;

        int choice = 0;
        

        // Check for keyboard shortcuts
        if (input.length() == 1 && std::isalpha(input[0])) {
            char shortcut = std::toupper(input[0]);
            switch (shortcut) {
                case 'M': choice = 1;break;
                case 'L': choice = 2;break;
                case 'D': choice = 3;break;
                case 'V': choice = 4;break;
                case 'S': choice = 5;break;
                case 'H': choice = 6;break;
                case 'G': choice = 7;break;
                case 'O': choice = 8;break;
                case 'T': choice = 9;break;
                case 'R': choice = 10;break;
                case 'C': choice = 11;break;
                case 'Q': choice = 12;break;
                default:
                    printError("Unknown shortcut: " + input);
                    waitWithAnimation(500);
                    continue;
            }
        } else {
            try {
                choice = std::stoi(input);
            } catch (...) {
                printError("Invalid input! Please enter a number or shortcut key.");
                waitWithAnimation(500);
                continue;
            }
        }

        switch (choice) {
            case 1: { // Enter Manually
                clearScreen();
                printHeader("Manual Input");
                sudoku.inputBoard();
                fileManager.log("Manual puzzle entered");
                pressEnterToContinue();
                break;
            }
            case 2: { // Load from File
                clearScreen();
                printHeader("Load Puzzle");
                std::string filename;
                std::cout << "\n  Enter filename (default: sample_input.txt): ";
                std::cin >> filename;
                if (filename.empty()) filename = "sample_input.txt";

                std::string validationError = fileManager.validateFile(filename);
                if (!validationError.empty()) {
                    printError(validationError);
                } else {
                    std::vector<std::vector<int>> loadedBoard;
                    if (fileManager.readPuzzle(filename, loadedBoard)) {
                        // Transfer loaded board to sudoku
                        for (int r = 0; r < 9; ++r) {
                            for (int c = 0; c < 9; ++c) {
                                sudoku.setCell(r, c, loadedBoard[r][c]);
                            }
                        }
                        printSuccess("Puzzle loaded successfully!");
                        sudoku.displayBoard();
                    }
                }
                fileManager.log("Puzzle loaded from: " + filename);
                pressEnterToContinue();
                break;
            }
            case 3: { // Display
                clearScreen();
                printHeader("Current Puzzle");
                sudoku.displayBoard(true);
                pressEnterToContinue();
                break;
            }
            case 4: { // Validate
                clearScreen();
                printHeader("Validation");
                if (sudoku.validateBoard()) {
                    printSuccess("Puzzle is valid!");
                } else {
                    printError("Puzzle contains conflicts!");
                    sudoku.displayBoard(true);
                }
                fileManager.log("Puzzle validated: " + 
                    std::string(sudoku.validateBoard() ? "Valid" : "Invalid"));
                pressEnterToContinue();
                break;
            }
            case 5: { // Solve
                clearScreen();
                printHeader("Solving Puzzle");

                std::cout << "\n  " << currentTheme.fg << "Solve mode:\n";
                std::cout << "  1. Instant Solve\n";
                std::cout << "  2. Step-by-Step (Animated)\n";
                std::cout << "  Enter choice: " << currentTheme.reset;

                int solveMode;
                std::cin >> solveMode;

                if (solveMode == 2) {
                    stepByStepSolve();
                } else {
                    if (sudoku.solve(false)) {
                        sudoku.displayBoard();
                        fileManager.recordHistory(
                            sudoku.detectDifficulty(),
                            sudoku.getSolveTime(),
                            sudoku.getRecursiveCalls(),
                            sudoku.getBacktracks()
                        );

                        // Update profile
                        ++totalSolved;
                        totalTime += sudoku.getSolveTime();
                        if (sudoku.getSolveTime() < fastestSolve) {
                            fastestSolve = sudoku.getSolveTime();
                        }

                        // Save profile
                        std::vector<std::string> profileData = {
                            username,
                            std::to_string(totalSolved),
                            std::to_string(totalTime / std::max(1, totalSolved)),
                            std::to_string(fastestSolve),
                            std::to_string(hintsUsed)
                        };
                        fileManager.saveProfile(username, profileData);

                        // Update leaderboard
                        fileManager.updateLeaderboard(username, sudoku.getSolveTime(),
                                                      sudoku.detectDifficulty());
                    }
                }
                pressEnterToContinue();
                break;
            }
            case 6: { // Hint
                clearScreen();
                printHeader("Hint System");
                if (sudoku.giveHint()) {
                    ++hintsUsed;
                    printSuccess("Hint applied!");
                    sudoku.displayBoard();
                } else {
                    printError("No more hints available!");
                }
                fileManager.log("Hint given");
                pressEnterToContinue();
                break;
            }
            case 7: { // Generate Puzzle
                clearScreen();
                printHeader("Generate Puzzle");

                std::cout << "\n  " << currentTheme.fg << "Select difficulty:\n";
                std::cout << "  1. Easy\n";
                std::cout << "  2. Medium\n";
                std::cout << "  3. Hard\n";
                std::cout << "  4. Expert\n";
                std::cout << "  Enter choice: " << currentTheme.reset;

                int diffChoice;
                std::cin >> diffChoice;

                std::string difficulty;
                switch (diffChoice) {
                    case 1: difficulty = "easy"; break;
                    case 2: difficulty = "medium"; break;
                    case 3: difficulty = "hard"; break;
                    case 4: difficulty = "expert"; break;
                    default: difficulty = "medium";
                }

                sudoku.generatePuzzle(difficulty);
                printSuccess("Generated " + difficulty + " puzzle!");
                sudoku.displayBoard();
                fileManager.log("Generated puzzle: " + difficulty);
                pressEnterToContinue();
                break;
            }
            case 8: { // Save Solution
                clearScreen();
                printHeader("Save Solution");

                if (!sudoku.isSolved()) {
                    printWarning("Puzzle is not solved yet. Save as unsolved?");
                    std::cout << "  Save anyway? (y/n): ";
                    char confirm;
                    std::cin >> confirm;
                    if (std::tolower(confirm) != 'y') break;
                }

                std::string filename;
                std::cout << "\n  Enter filename (default: solved_output.txt): ";
                std::cin >> filename;
                if (filename.empty()) filename = "solved_output.txt";

                if (sudoku.savePuzzle(filename)) {
                    printSuccess("Solution saved to: " + filename);
                }
                fileManager.log("Solution saved to: " + filename);
                pressEnterToContinue();
                break;
            }
            case 9: { // Statistics
                clearScreen();
                printHeader("Statistics");
                sudoku.showStatistics();
                pressEnterToContinue();
                break;
            }
            case 10: { // Reset
                clearScreen();
                printHeader("Reset Puzzle");
                std::cout << "\n  " << currentTheme.warning << "Are you sure? (y/n): " << currentTheme.reset;
                char confirm;
                std::cin >> confirm;
                if (std::tolower(confirm) == 'y') {
                    sudoku.resetBoard();
                    printSuccess("Puzzle reset to original state!");
                    sudoku.displayBoard();
                    fileManager.log("Puzzle reset");
                }
                pressEnterToContinue();
                break;
            }
            case 11: { // Settings
                settingsMenu();
                break;
            }
            case 12: { // Exit
                clearScreen();
                printHeader("Exit");

                std::cout << "\n  " << currentTheme.warning << "Are you sure you want to exit? (y/n): " << currentTheme.reset;
                char confirm;
                std::cin >> confirm;

                if (std::tolower(confirm) == 'y') {
                    fileManager.log("Application closed by: " + username);
                    clearScreen();
                    printHeader("Thank you for using " + APP_NAME + "!");
                    std::cout << "\n  " << currentTheme.success << "  Puzzles Solved: " << totalSolved 
                              << "  |  Fastest: " << (fastestSolve < 999999.0 ? 
                                 std::to_string(fastestSolve) + "s" : "N/A") 
                              << currentTheme.reset << "\n\n";
                    resetTheme();
                    return 0;
                }
                break;
            }
            default:
                printError("Invalid choice! Please enter a number between 1-12.");
                waitWithAnimation(500);
        }
    }

    resetTheme();
    return 0;
}
