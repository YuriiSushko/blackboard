#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
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
    static int id;

    virtual ~Figure() { id--; }

    virtual void add(vector<vector<char>>* grid) = 0;
    virtual void get_info() = 0;
};

int Figure::id = 0;

class Square: public Figure{
private: 
    int size;
    int s_id;
    tuple<int,int> coordinates; // coordinates of square top_left
public:
    Square(const int& size, const int& x, const int& y): size(size), coordinates(make_tuple(x,y)), s_id(id++) { }

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
        cout << "Square: id(" << s_id << "), size(" << size << "), coordinates(" << get<0>(coordinates) << "," << get<1>(coordinates) << ")\n";
    }
};

class Triangle: public Figure{
private:
    int height;
    int s_id;
    tuple<int,int> coordinates;
public:
    Triangle(const int& height, const int& x, const int& y): height(height), coordinates(make_tuple(x,y)), s_id(id++) {}

    void add(vector<vector<char>>* grid) override{
        if (height <= 0) return;
        
        int x = get<0>(coordinates); 
        int y = get<1>(coordinates); 

        int up_bound = grid[0].size();
        int right_bound = grid[0][0].size();

        for (int i = 0; i < height; ++i) {
            int leftMost = x - i;    
            int rightMost = x + i;   
            int posY = y - i;        

            
            if ((posY) >= 0 && (posY) <= up_bound) {
                if (leftMost >= 0 && leftMost < right_bound) {
                    (*grid)[up_bound - posY][leftMost] = '*';
                }
            
                if (rightMost >= 0 && rightMost < right_bound) {
                    (*grid)[up_bound - posY][rightMost] = '*';
                }
            }
        }

    
        int baseY = y - height + 1; 
        for (int j = 0; j < 2 * height - 1; ++j) {
            int baseX = x + height - 1 - j;
            if (baseX >= 0 && baseX < right_bound && (baseY) >= 0 && (baseY) <= up_bound) {
                (*grid)[up_bound-baseY][baseX] = '*'; 
            }
        }
    }

    void get_info() override {
        cout << "Triangle: id(" << s_id << "), height(" << height << "), coordinates(" << get<0>(coordinates) << "," << get<1>(coordinates) << ")\n";
    }
};

class FileSystem {
private:
    string path;
    vector<vector<char>> grid;
public:
    FileSystem(const string& path, vector<vector<char>>& grid): path(path), grid(grid){}

    void save(){
        ofstream file;
        file.open(path);

       for (auto& row : grid) {
            for (char c : row) {
                file << c;
            }
            file << "\n";
        }

        cout << "File has been successfully saved!\n";
        file.close();
    }

    vector<vector<char>> load(){
        string line;
        ifstream file (path);

        grid.clear();

        if (file.is_open()){
            while (getline(file, line)) {
                vector<char> row(line.begin(), line.end());
                grid.push_back(row);
            }
        }
        else cout << "Unable to open file"; 
        file.close();
        
        return grid;
    }
};


class blackboard
{
private:
    const int BOARD_WIDTH = 90;
    const int BOARD_HEIGHT = 50;

    vector<vector<char>> grid;
    vector<unique_ptr<Figure>> figures;
    vector<vector<char>> previous;
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
        previous = grid;

        auto figure = make_unique<Square>(size, x, y);
        figure->add(&grid);
        figures.push_back(move(figure));
    } 

    void add_triangle(const int& height, const int& x, const int& y){
        previous = grid;

        auto figure = make_unique<Triangle>(height, x, y);
        figure->add(&grid);
        figures.push_back(move(figure));
    } 

    void shapes() {
        cout << MAGENTA << "Square:" << YELLOW << " size, coordinates" << BLUE << "[x,y]" << RESET << " of top left edge\n";
        cout << MAGENTA << "Circle:" << YELLOW << " radius, coordinates" << BLUE << "[x,y]" << RESET << " of centre\n";
        cout << MAGENTA << "Triangle:" << YELLOW << " height, side length, coordinates" << BLUE << "[x,y]" << RESET << " of upmost edge\n";
        cout << MAGENTA << "Line:" << YELLOW << " length, angle, coordinates" << BLUE << "[x,y]" << RESET << " of left edge\n";
    };

    void list(){
        for (auto& shape: figures){
            shape->get_info();
        }
    }

    void undo(){
        figures.pop_back();
        grid = previous;
    }

    void clear(){
        figures.clear();
        grid = vector<vector<char>>(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));
    }

    void save(const string& filepath){
        FileSystem fs(filepath, grid);
        fs.save();
    }

    void load(const string& filepath){
        FileSystem fs(filepath, grid);
        grid = fs.load();
    }
};

int main() {
    blackboard blackboard;

    blackboard.add_square(9, 40, 21);
    blackboard.add_square(3, 43, 18);
    blackboard.add_triangle(5, 44, 27);
    
    blackboard.draw();
    blackboard.list();
    return 0;
}