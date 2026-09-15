#include "FileManager.h"

// ============================================================================
// Constructor
// ============================================================================
FileManager::FileManager()
    : logFile("logs.txt")
    , historyFile("history.txt")
    , configFile("config.txt")
{
}

// ============================================================================
// Utility
// ============================================================================
std::string FileManager::getTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool FileManager::isFileReadable(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

// ============================================================================
// File Operations
// ============================================================================
bool FileManager::readPuzzle(const std::string& filename, std::vector<std::vector<int>>& board) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return false;
    }

    const int SIZE = 9;
    board.assign(SIZE, std::vector<int>(SIZE, 0));
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < SIZE) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int val;
        int col = 0;
        while (iss >> val && col < SIZE) {
            if (val < 0 || val > 9) {
                std::cerr << "Warning: Invalid value '" << val << "' at row " 
                          << (row + 1) << ", col " << (col + 1) << ". Using 0.\n";
                val = 0;
            }
            board[row][col] = val;
            ++col;
        }
        ++row;
    }

    file.close();
    log("Puzzle loaded from: " + filename);
    return true;
}

bool FileManager::savePuzzle(const std::string& filename, 
                              const std::vector<std::vector<int>>& board, 
                              bool solved) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << "\n";
        return false;
    }

    const int SIZE = 9;
    file << "# Sudoku Puzzle - " << (solved ? "Solved" : "Unsolved") << "\n";
    file << "# Generated: " << getTimestamp() << "\n";
    file << "# Format: 9 lines of 9 numbers separated by spaces\n";
    file << "# 0 represents empty cells\n\n";

    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            file << board[r][c];
            if (c < SIZE - 1) file << " ";
        }
        file << "\n";
    }

    file.close();
    log("Puzzle saved to: " + std::string(solved ? "(solved) " : "") + filename);
    return true;
}

std::string FileManager::validateFile(const std::string& filename) {
    if (!isFileReadable(filename)) {
        return "File does not exist or is not readable: " + filename;
    }

    std::ifstream file(filename);
    const int SIZE = 9;
    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < SIZE) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        int val;
        int col = 0;
        while (iss >> val) {
            if (val < 0 || val > 9) {
                return "Invalid value '" + std::to_string(val) + "' at row " 
                       + std::to_string(row + 1) + ", col " + std::to_string(col + 1);
            }
            ++col;
        }
        if (col != SIZE) {
            return "Row " + std::to_string(row + 1) + " has " + std::to_string(col) 
                   + " values (expected " + std::to_string(SIZE) + ")";
        }
        ++row;
    }

    if (row < SIZE) {
        return "File has only " + std::to_string(row) + " rows (expected " 
               + std::to_string(SIZE) + ")";
    }

    return ""; // Valid
}

// ============================================================================
// Logging
// ============================================================================
void FileManager::log(const std::string& message) {
    std::ofstream file(logFile, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open log file: " << logFile << "\n";
        return;
    }
    file << "[" << getTimestamp() << "] " << message << "\n";
    file.close();
}

// ============================================================================
// History
// ============================================================================
void FileManager::recordHistory(const std::string& difficulty, double solveTime,
                                 int recursiveCalls, int backtracks) {
    std::ofstream file(historyFile, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open history file: " << historyFile << "\n";
        return;
    }

    file << difficulty << " | "
         << std::fixed << std::setprecision(4) << solveTime << "s | "
         << "Recursive: " << recursiveCalls << " | "
         << "Backtracks: " << backtracks << " | "
         << "Date: " << getTimestamp() << "\n";
    file.close();

    log("History recorded: " + difficulty + " - " + std::to_string(solveTime) + "s");
}

void FileManager::displayHistory() const {
    std::ifstream file(historyFile);
    if (!file.is_open()) {
        std::cout << "No history available yet.\n";
        return;
    }

    std::cout << "\n========== Solve History ==========\n";
    std::cout << "Format: Difficulty | Time | Details | Date\n";
    std::cout << "--------------------------------------\n";

    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::cout << ++count << ". " << line << "\n";
        }
    }

    if (count == 0) {
        std::cout << "No history entries found.\n";
    }
    std::cout << "====================================\n\n";
    file.close();
}

// ============================================================================
// Configuration
// ============================================================================
std::string FileManager::getConfig(const std::string& key, const std::string& defaultValue) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return defaultValue;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string fileKey = line.substr(0, eqPos);
            // Trim whitespace
            fileKey.erase(0, fileKey.find_first_not_of(" \t"));
            fileKey.erase(fileKey.find_last_not_of(" \t") + 1);

            if (fileKey == key) {
                std::string value = line.substr(eqPos + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                return value;
            }
        }
    }

    file.close();
    return defaultValue;
}

void FileManager::setConfig(const std::string& key, const std::string& value) {
    std::vector<std::string> lines;
    std::ifstream inFile(configFile);
    bool found = false;

    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') {
                lines.push_back(line);
                continue;
            }

            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string fileKey = line.substr(0, eqPos);
                fileKey.erase(0, fileKey.find_first_not_of(" \t"));
                fileKey.erase(fileKey.find_last_not_of(" \t") + 1);

                if (fileKey == key) {
                    lines.push_back(key + "=" + value);
                    found = true;
                    continue;
                }
            }
            lines.push_back(line);
        }
        inFile.close();
    }

    if (!found) {
        lines.push_back(key + "=" + value);
    }

    std::ofstream outFile(configFile);
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();

    log("Config updated: " + key + " = " + value);
}

// ============================================================================
// Leaderboard
// ============================================================================
void FileManager::displayLeaderboard() const {
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        std::cout << "No leaderboard data available yet.\n";
        return;
    }

    std::cout << "\n========== Leaderboard (Top 10) ==========\n";
    std::cout << "Rank | Player       | Time    | Difficulty | Date\n";
    std::cout << "---------------------------------------------\n";

    std::string line;
    int rank = 0;
    while (std::getline(file, line) && rank < 10) {
        if (!line.empty()) {
            std::cout << std::setw(4) << std::left << std::to_string(++rank) << " | " 
                      << line << "\n";
        }
    }

    if (rank == 0) {
        std::cout << "No entries yet.\n";
    }
    std::cout << "==========================================\n\n";
    file.close();
}

void FileManager::updateLeaderboard(const std::string& username, double solveTime,
                                     const std::string& difficulty) {
    std::vector<std::tuple<double, std::string, std::string, std::string>> entries;

    // Read existing entries
    std::ifstream inFile("leaderboard.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty()) {
                // Parse: username | time | difficulty | date
                std::istringstream iss(line);
                std::string name, timeStr, diff, date;
                std::getline(iss, name, '|');
                std::getline(iss, timeStr, '|');
                std::getline(iss, diff, '|');
                std::getline(iss, date, '|');
                double time = std::stod(timeStr);
                entries.emplace_back(time, name, diff, date);
            }
        }
        inFile.close();
    }

    // Add new entry
    entries.emplace_back(solveTime, username, difficulty, getTimestamp());

    // Sort by time (ascending)
    std::sort(entries.begin(), entries.end());

    // Write top 10 back
    std::ofstream outFile("leaderboard.txt");
    int count = 0;
    for (const auto& entry : entries) {
        if (count >= 10) break;
        outFile << std::get<1>(entry) << "|" 
                << std::fixed << std::setprecision(4) << std::get<0>(entry) << "|"
                << std::get<2>(entry) << "|"
                << std::get<3>(entry) << "\n";
        ++count;
    }
    outFile.close();

    log("Leaderboard updated: " + username + " - " + std::to_string(solveTime) + "s");
}

// ============================================================================
// Profile
// ============================================================================
std::vector<std::string> FileManager::loadProfile(const std::string& username) {
    std::vector<std::string> data;
    std::string profileFile = "profile_" + username + ".txt";
    std::ifstream file(profileFile);
    
    if (!file.is_open()) {
        // Create default profile
        data = {username, "0", "0", "0", "0"};
        saveProfile(username, data);
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            data.push_back(line);
        }
    }
    file.close();
    return data;
}

void FileManager::saveProfile(const std::string& username, 
                               const std::vector<std::string>& data) {
    std::string profileFile = "profile_" + username + ".txt";
    std::ofstream file(profileFile);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not save profile for " << username << "\n";
        return;
    }

    for (const auto& line : data) {
        file << line << "\n";
    }
    file.close();
    log("Profile saved: " + username);
}
