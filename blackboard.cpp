#include <iostream>
#include <memory>
#include <vector>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

class Figure{
public:
    virtual ~Figure() {};

    virtual void add(vector<vector<char>>* grid) = 0;
    virtual void get_info() = 0;
};

class Square: public Figure{
private: 
    int size;
    tuple<int,int> coordinates; // coordinates of square top_left
public:
    Square(const int& size, const int& x, const int& y): size(size), coordinates(make_tuple(x,y)) {}

    void add(vector<vector<char>>* grid) override{
        if (size <= 0){
            cerr << "Please, provide positive numbers for size\n";
            return;
        } 

        int x = get<0>(coordinates); 
        int y = get<1>(coordinates); 

        int up_bound = grid[0].size();
        int right_bound = grid[0][0].size();

        for (int i = 0; i < size; i++) {
            int current_row = y - i;

            if (current_row >= 0 && current_row < up_bound) {
                if (i==0 || i == size-1) {
                    for (int j = x; j < x + size; j++) {
                        if (j >= 0 && j <= right_bound){
                            (*grid)[up_bound - current_row-1][j] = '*';
                        }
                    }
                    continue;
                }
            }

            if (current_row >= 0 && current_row < up_bound){
                if (x >= 0 && x <= right_bound) {
                    (*grid)[up_bound - current_row - 1][x] = '*';
                }
                if (x + size - 1 >= 0 && x + size - 1 <= right_bound) {
                    (*grid)[up_bound - current_row - 1][x + size -1] = '*';
                }
            }
            
        }
    }

    void get_info() override {
        cout << "Square: id(" << 4 << "), size(" << size << ">>), coordinates(" << get<0>(coordinates) << "," << get<1>(coordinates) << ")\n";
    }
};

class blackboard
{
private:
    const int BOARD_WIDTH = 90;
    const int BOARD_HEIGHT = 50;

    vector<vector<char>> grid;
    vector<unique_ptr<Figure>> figures;
public:
    blackboard() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void draw() {
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << RED << "-" << RESET; 
        }
        cout << "\n";
        for (auto& row : grid) {
            cout << RED << '|' << RESET;
            for (char c : row) {
                cout << c;
            }
            cout  << RED << " |\n" << RESET;
        }
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << RED << "-" << RESET; 
        }
        cout << '\n';
    }

    void add_square(const int& size, const int& x, const int& y){
        auto figure = make_unique<Square>(size, x, y);
        figure->add(&grid);
        figures.push_back(move(figure));
    } 

    void shapes() {
        cout << MAGENTA << "Square:" << YELLOW << " size, coordinates" << BLUE << "[x,y]" << RESET << " of top left edge\n";
        cout << MAGENTA << "Circle:" << YELLOW << " radius, coordinates" << BLUE << "[x,y]" << RESET << " of centre\n";
        cout << MAGENTA << "Triangle:" << YELLOW << " height, side lenght, coordinates" << BLUE << "[x,y]" << RESET << " of upmost edge\n";
        cout << MAGENTA << "Line:" << YELLOW << " lenght, angle, coordinates" << BLUE << "[x,y]" << RESET << " of left edge\n";
    };


};

int main() {
    blackboard blackboard;

    blackboard.add_square(9, 40, 21);
    blackboard.add_square(3, 43, 18);
    
    blackboard.draw();
    blackboard.shapes();
    return 0;
}