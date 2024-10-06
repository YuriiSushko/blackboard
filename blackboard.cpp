#include <iostream>
using namespace std;
#include <vector>

class Figure{
public:
    virtual void add(vector<vector<char>>* grid) = 0;
    virtual void shapes() = 0;
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

    void shapes() override {
        cout << "Square: size, x, y\n"; 
    }
};

class blackboard
{
private:
    const int BOARD_WIDTH = 26;
    const int BOARD_HEIGHT = 12;

    vector<vector<char>> grid;
    vector<Figure*> figures;
public:
    blackboard() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void draw() {
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << "-"; 
        }
        cout << "\n";
        for (auto& row : grid) {
            cout << '|';
            for (char c : row) {
                cout << c;
            }
            cout << " |\n";
        }
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << "-"; 
        }
    }

    void add_square(const int& size, const int& x, const int& y){
        Figure* figure = new Square(size, x, y);
        Square* new_square = dynamic_cast<Square*>(figure);
        
        if (new_square){
            new_square->add(&grid);
            figures.push_back(new_square);
        }
        else{
            cout << "Dynamic cast failure\n";
        }
    } 
    
};

int main() {
    blackboard blackboard;

    blackboard.add_square(9, 6, 10);
    blackboard.add_square(3, 9, 7);
    
    blackboard.draw();

    return 0;
}