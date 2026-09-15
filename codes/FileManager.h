#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

/**
 * @class FileManager
 * @brief Handles all file I/O operations for the Sudoku Solver.
 *
 * Features:
 * - Read/write puzzle files
 * - Logging system with timestamps
 * - History tracking
 * - Configuration management
 * - Input validation and error handling
 */
class FileManager {
private:
    std::string logFile;      ///< Path to log file
    std::string historyFile;  ///< Path to history file
    std::string configFile;   ///< Path to config file

    /**
     * @brief Get current timestamp as string.
     * @return Formatted timestamp string
     */
    static std::string getTimestamp();

    /**
     * @brief Check if a file exists and is readable.
     * @param filename Path to the file
     * @return true if file is valid
     */
    static bool isFileReadable(const std::string& filename);

public:
    /**
     * @brief Constructor with default file paths.
     */
    FileManager();

    /**
     * @brief Read a Sudoku puzzle from a file.
     * @param filename Path to the puzzle file
     * @param board Output parameter for the 9x9 board
     * @return true if read was successful
     */
    bool readPuzzle(const std::string& filename, std::vector<std::vector<int>>& board);

    /**
     * @brief Save a Sudoku puzzle to a file.
     * @param filename Path to the output file
     * @param board The 9x9 board to save
     * @param solved Whether this is a solved puzzle
     * @return true if save was successful
     */
    bool savePuzzle(const std::string& filename, const std::vector<std::vector<int>>& board, bool solved = false);

    /**
     * @brief Validate a puzzle file format and content.
     * @param filename Path to the file
     * @return Error message (empty string if valid)
     */
    std::string validateFile(const std::string& filename);

    /**
     * @brief Write a log entry.
     * @param message Log message
     */
    void log(const std::string& message);

    /**
     * @brief Record puzzle solving history.
     * @param difficulty Puzzle difficulty
     * @param solveTime Time taken to solve
     * @param recursiveCalls Number of recursive calls
     * @param backtracks Number of backtracks
     */
    void recordHistory(const std::string& difficulty, double solveTime,
                       int recursiveCalls, int backtracks);

    /**
     * @brief Display the solving history.
     */
    void displayHistory() const;

    /**
     * @brief Load configuration settings.
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    std::string getConfig(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Set a configuration value.
     * @param key Configuration key
     * @param value Configuration value
     */
    void setConfig(const std::string& key, const std::string& value);

    /**
     * @brief Display the leaderboard.
     */
    void displayLeaderboard() const;

    /**
     * @brief Update the leaderboard with a new entry.
     * @param username Player username
     * @param solveTime Solve time in seconds
     * @param difficulty Puzzle difficulty
     */
    void updateLeaderboard(const std::string& username, double solveTime,
                           const std::string& difficulty);

    /**
     * @brief Load user profile.
     * @param username Username to load
     * @return Profile data as a vector of strings
     */
    std::vector<std::string> loadProfile(const std::string& username);

    /**
     * @brief Save user profile.
     * @param username Username
     * @param data Profile data to save
     */
    void saveProfile(const std::string& username, const std::vector<std::string>& data);
};

#endif // FILEMANAGER_H

