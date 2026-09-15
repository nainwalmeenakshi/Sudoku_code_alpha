/**
 * @file gui.cpp
 * @brief SFML Graphical User Interface for Sudoku Solver
 *
 * This file provides a graphical interface using SFML library.
 * To compile with GUI support:
 *   g++ -std=c++17 *.cpp -o SudokuSolverGUI -lsfml-graphics -lsfml-window -lsfml-system
 *
 * The GUI is separated from the solving logic (Sudoku class).
 * All solving operations delegate to the Sudoku class.
 *
 * Note: SFML must be installed on the system.
 *   macOS: brew install sfml
 *   Ubuntu: sudo apt install libsfml-dev
 */

#ifdef SFML_FOUND

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "Sudoku.h"

// ============================================================================
// Constants
// ============================================================================
constexpr int WINDOW_WIDTH = 700;
constexpr int WINDOW_HEIGHT = 800;
constexpr int GRID_SIZE = 9;
constexpr int CELL_SIZE = 60;
constexpr int GRID_OFFSET_X = 65;
constexpr int GRID_OFFSET_Y = 120;
constexpr int BUTTON_WIDTH = 120;
constexpr int BUTTON_HEIGHT = 35;
constexpr int FONT_SIZE = 24;
constexpr int SMALL_FONT_SIZE = 14;

// Colors
const sf::Color COLOR_BG(30, 30, 30);
const sf::Color COLOR_GRID(100, 100, 100);
const sf::Color COLOR_CELL(50, 50, 50);
const sf::Color COLOR_TEXT(220, 220, 220);
const sf::Color COLOR_ORIGINAL(255, 200, 50);
const sf::Color COLOR_HIGHLIGHT(255, 100, 100);
const sf::Color COLOR_BUTTON(70, 70, 70);
const sf::Color COLOR_BUTTON_HOVER(100, 100, 100);
const sf::Color COLOR_STATUS(100, 200, 100);

// ============================================================================
// GUI Application Class
// ============================================================================
class SudokuGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Font smallFont;
    Sudoku sudoku;
    std::string statusMessage;
    sf::Clock timer;
    int selectedRow;
    int selectedCol;
    bool isSolved;
    bool isAnimating;
    std::vector<std::string> difficultyLevels;

    // UI Elements
    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        std::string label;
        bool isHovered;
    };

    std::vector<Button> buttons;
    std::vector<Button> difficultyButtons;

    // Timer display
    sf::Text timerText;
    sf::Text statusText;

public:
    SudokuGUI() 
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sudoku Solver", sf::Style::Close | sf::Style::Titlebar)
        , selectedRow(-1)
        , selectedCol(-1)
        , isSolved(false)
        , isAnimating(false)
    {
        difficultyLevels = {"Easy", "Medium", "Hard", "Expert"};
        setup();
    }

    /**
     * @brief Initialize GUI elements.
     */
    void setup() {
        window.setFramerateLimit(60);

        // Load fonts
        if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            // Fallback to basic font
            if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                // Last resort - try to create from memory
                std::cerr << "Warning: Could not load font. Text may not display.\n";
            }
        }

        if (!smallFont.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            if (!smallFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Warning: Could not load small font.\n";
            }
        }

        // Setup timer text
        timerText.setFont(smallFont);
        timerText.setCharacterSize(SMALL_FONT_SIZE);
        timerText.setFillColor(COLOR_TEXT);
        timerText.setPosition(10, 10);

        // Setup status text
        statusText.setFont(smallFont);
        statusText.setCharacterSize(SMALL_FONT_SIZE);
        statusText.setFillColor(COLOR_STATUS);
        statusText.setPosition(10, WINDOW_HEIGHT - 40);

        setStatus("Welcome to Sudoku Solver!");

        // Create buttons
        createButton("Solve", WINDOW_WIDTH - 160, 10);
        createButton("Reset", WINDOW_WIDTH - 160, 55);
        createButton("Hint", WINDOW_WIDTH - 160, 100);
        createButton("Generate", WINDOW_WIDTH - 160, 145);
        createButton("Validate", WINDOW_WIDTH - 160, 190);
        createButton("Save", WINDOW_WIDTH - 160, 235);
        createButton("Load", WINDOW_WIDTH - 160, 280);

        // Create difficulty buttons
        float diffY = 330;
        for (const auto& diff : difficultyLevels) {
            createDifficultyButton(diff, WINDOW_WIDTH - 160, diffY);
            diffY += 40;
        }
    }

    /**
     * @brief Create a control button.
     */
    void createButton(const std::string& label, float x, float y) {
        Button btn;
        btn.shape.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        btn.shape.setPosition(x, y);
        btn.shape.setFillColor(COLOR_BUTTON);
        btn.shape.setOutlineThickness(1);
        btn.shape.setOutlineColor(sf::Color::White);

        btn.text.setFont(smallFont);
        btn.text.setString(label);
        btn.text.setCharacterSize(SMALL_FONT_SIZE);
        btn.text.setFillColor(COLOR_TEXT);

        // Center text in button
        sf::FloatRect textBounds = btn.text.getLocalBounds();
        btn.text.setPosition(
            x + (BUTTON_WIDTH - textBounds.width) / 2,
            y + (BUTTON_HEIGHT - textBounds.height) / 2 - 2
        );

        btn.label = label;
        btn.isHovered = false;
        buttons.push_back(btn);
    }

    /**
     * @brief Create a difficulty selector button.
     */
    void createDifficultyButton(const std::string& label, float x, float y) {
        Button btn;
        btn.shape.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        btn.shape.setPosition(x, y);
        btn.shape.setFillColor(sf::Color(50, 50, 80));
        btn.shape.setOutlineThickness(1);
        btn.shape.setOutlineColor(sf::Color::White);

        btn.text.setFont(smallFont);
        btn.text.setString(label);
        btn.text.setCharacterSize(SMALL_FONT_SIZE);
        btn.text.setFillColor(COLOR_TEXT);

        sf::FloatRect textBounds = btn.text.getLocalBounds();
        btn.text.setPosition(
            x + (BUTTON_WIDTH - textBounds.width) / 2,
            y + (BUTTON_HEIGHT - textBounds.height) / 2 - 2
        );

        btn.label = label;
        btn.isHovered = false;
        difficultyButtons.push_back(btn);
    }

    /**
     * @brief Set the status bar message.
     */
    void setStatus(const std::string& msg) {
        statusMessage = msg;
        statusText.setString(msg);
        std::cout << "[GUI] " << msg << std::endl;
    }

    /**
     * @brief Draw the Sudoku grid.
     */
    void drawGrid() {
        const auto& board = sudoku.getBoard();

        // Draw background
        sf::RectangleShape gridBg(sf::Vector2f(GRID_SIZE * CELL_SIZE + 2, GRID_SIZE * CELL_SIZE + 2));
        gridBg.setPosition(GRID_OFFSET_X, GRID_OFFSET_Y);
        gridBg.setFillColor(sf::Color(20, 20, 20));
        window.draw(gridBg);

        // Draw cells
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                cell.setPosition(GRID_OFFSET_X + c * CELL_SIZE + 1, GRID_OFFSET_Y + r * CELL_SIZE + 1);

                // Highlight selected cell
                if (r == selectedRow && c == selectedCol) {
                    cell.setFillColor(sf::Color(80, 80, 120));
                } else {
                    cell.setFillColor(COLOR_CELL);
                }

                window.draw(cell);

                // Draw cell value
                int val = board[r][c];
                if (val != 0) {
                    sf::Text cellText;
                    cellText.setFont(font);
                    cellText.setString(std::to_string(val));
                    cellText.setCharacterSize(FONT_SIZE);
                    
                    // Color: yellow for original, white for user-entered
                    bool isOriginal = (sudoku.getOriginalBoard()[r][c] != 0);
                    cellText.setFillColor(isOriginal ? COLOR_ORIGINAL : COLOR_TEXT);

                    // Center text in cell
                    sf::FloatRect textBounds = cellText.getLocalBounds();
                    cellText.setPosition(
                        GRID_OFFSET_X + c * CELL_SIZE + (CELL_SIZE - textBounds.width) / 2,
                        GRID_OFFSET_Y + r * CELL_SIZE + (CELL_SIZE - textBounds.height) / 2 - 4
                    );

                    // Highlight invalid cells
                    if (!sudoku.validateCell(r, c)) {
                        cellText.setFillColor(COLOR_HIGHLIGHT);
                    }

                    window.draw(cellText);
                }
            }
        }

        // Draw grid lines (thicker for 3x3 boxes)
        for (int i = 0; i <= GRID_SIZE; ++i) {
            float thickness = (i % 3 == 0) ? 3.0f : 1.0f;
            
            // Vertical lines
            sf::RectangleShape vLine(sf::Vector2f(thickness, GRID_SIZE * CELL_SIZE));
            vLine.setPosition(GRID_OFFSET_X + i * CELL_SIZE, GRID_OFFSET_Y);
            vLine.setFillColor(COLOR_GRID);
            window.draw(vLine);

            // Horizontal lines
            sf::RectangleShape hLine(sf::Vector2f(GRID_SIZE * CELL_SIZE, thickness));
            hLine.setPosition(GRID_OFFSET_X, GRID_OFFSET_Y + i * CELL_SIZE);
            hLine.setFillColor(COLOR_GRID);
            window.draw(hLine);
        }
    }

    /**
     * @brief Draw all UI buttons.
     */
    void drawButtons() {
        for (auto& btn : buttons) {
            btn.shape.setFillColor(btn.isHovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON);
            window.draw(btn.shape);
            window.draw(btn.text);
        }

        for (auto& btn : difficultyButtons) {
            btn.shape.setFillColor(btn.isHovered ? COLOR_BUTTON_HOVER : sf::Color(50, 50, 80));
            window.draw(btn.shape);
            window.draw(btn.text);
        }
    }

    /**
     * @brief Handle mouse click events.
     */
    void handleMouseClick(int mouseX, int mouseY) {
        // Check button clicks
        for (const auto& btn : buttons) {
            if (btn.shape.getGlobalBounds().contains(mouseX, mouseY)) {
                handleButtonClick(btn.label);
                return;
            }
        }

        // Check difficulty button clicks
        for (const auto& btn : difficultyButtons) {
            if (btn.shape.getGlobalBounds().contains(mouseX, mouseY)) {
                setStatus("Generating " + btn.label + " puzzle...");
                sudoku.generatePuzzle(btn.label);
                isSolved = false;
                timer.restart();
                setStatus(btn.label + " puzzle generated!");
                return;
            }
        }

        // Check grid cell clicks
        if (mouseX >= GRID_OFFSET_X && mouseX < GRID_OFFSET_X + GRID_SIZE * CELL_SIZE &&
            mouseY >= GRID_OFFSET_Y && mouseY < GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE) {
            selectedRow = (mouseY - GRID_OFFSET_Y) / CELL_SIZE;
            selectedCol = (mouseX - GRID_OFFSET_X) / CELL_SIZE;
        }
    }

    /**
     * @brief Handle button click actions.
     */
    void handleButtonClick(const std::string& label) {
        if (label == "Solve") {
            if (sudoku.solve(false)) {
                isSolved = true;
                setStatus("Puzzle solved! Time: " + formatTime(sudoku.getSolveTime()));
            } else {
                setStatus("No solution exists for this puzzle.");
            }
        } else if (label == "Reset") {
            sudoku.resetBoard();
            isSolved = false;
            timer.restart();
            selectedRow = -1;
            selectedCol = -1;
            setStatus("Board reset to original state.");
        } else if (label == "Hint") {
            if (sudoku.giveHint()) {
                setStatus("Hint given!");
            } else {
                setStatus("No more hints available.");
            }
        } else if (label == "Generate") {
            sudoku.generatePuzzle("medium");
            isSolved = false;
            timer.restart();
            setStatus("Medium puzzle generated!");
        } else if (label == "Validate") {
            if (sudoku.validateBoard()) {
                setStatus("Puzzle is valid! ✓");
            } else {
                setStatus("Puzzle has conflicts! ✗");
            }
        } else if (label == "Save") {
            if (sudoku.savePuzzle("solved_output.txt")) {
                setStatus("Saved to solved_output.txt ✓");
            } else {
                setStatus("Failed to save puzzle.");
            }
        } else if (label == "Load") {
            std::vector<std::vector<int>> loadedBoard;
            FileManager fm;
            if (fm.readPuzzle("sample_input.txt", loadedBoard)) {
                for (int r = 0; r < 9; ++r)
                    for (int c = 0; c < 9; ++c)
                        sudoku.setCell(r, c, loadedBoard[r][c]);
                setStatus("Loaded: sample_input.txt ✓");
            } else {
                setStatus("Failed to load puzzle.");
            }
        }
    }

    /**
     * @brief Handle keyboard input for cell editing.
     */
    void handleKeyPress(sf::Keyboard::Key key) {
        if (selectedRow < 0 || selectedCol < 0) return;

        // Don't allow editing original cells
        if (sudoku.getOriginalBoard()[selectedRow][selectedCol] != 0) return;

        int num = -1;
        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
            num = key - sf::Keyboard::Num0;
        } else if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9) {
            num = key - sf::Keyboard::Numpad0;
        } else if (key == sf::Keyboard::Delete || key == sf::Keyboard::BackSpace) {
            num = 0;
        }

        if (num >= 0 && num <= 9) {
            sudoku.setCell(selectedRow, selectedCol, num);
            isSolved = false;
        }
    }

    /**
     * @brief Format time in seconds.
     */
    static std::string formatTime(double seconds) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << seconds << "s";
        return oss.str();
    }

    /**
     * @brief Main application loop.
     */
    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;

                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                        }
                        break;

                    case sf::Event::MouseMoved: {
                        int mx = event.mouseMove.x;
                        int my = event.mouseMove.y;
                        for (auto& btn : buttons) {
                            btn.isHovered = btn.shape.getGlobalBounds().contains(mx, my);
                        }
                        for (auto& btn : difficultyButtons) {
                            btn.isHovered = btn.shape.getGlobalBounds().contains(mx, my);
                        }
                        break;
                    }

                    case sf::Event::KeyPressed:
                        handleKeyPress(event.key.code);
                        break;

                    default:
                        break;
                }
            }

            // Update timer
            float elapsed = timer.getElapsedTime().asSeconds();
            std::string timerStr = "Time: " + formatTime(elapsed);
            timerText.setString(timerStr);

            // Render
            window.clear(COLOR_BG);
            drawGrid();
            drawButtons();
            window.draw(timerText);
            window.draw(statusText);
            window.display();
        }
    }
};

// ============================================================================
// Main function for GUI mode
// ============================================================================
int main() {
    std::cout << "Starting Sudoku Solver GUI...\n";
    
    try {
        SudokuGUI app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#else
// ============================================================================
// Fallback when SFML is not available
// ============================================================================
#include <iostream>

int main() {
    std::cout << "\n=== Sudoku Solver GUI ===\n\n";
    std::cout << "GUI mode requires SFML library.\n\n";
    std::cout << "To install SFML:\n";
    std::cout << "  macOS:  brew install sfml\n";
    std::cout << "  Ubuntu: sudo apt install libsfml-dev\n\n";
    std::cout << "To compile with GUI:\n";
    std::cout << "  g++ -std=c++17 *.cpp -o SudokuSolverGUI -lsfml-graphics -lsfml-window -lsfml-system\n\n";
    std::cout << "Or use the console mode: ./SudokuSolver\n";
    return 1;
}
#endif // SFML_FOUND

