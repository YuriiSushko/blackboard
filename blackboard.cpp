#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <cmath>
#include <cctype> 
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

class Circle: public Figure{
private:
    int radius;
    int s_id;
    tuple<int, int> coordinates;
public:
    Circle(const int& radius, const int& x, const int& y): radius(radius), coordinates(make_tuple(x,y)), s_id(id++) {}

    void add(vector<vector<char>>* grid) override{
        if (radius <= 0){
            cerr << "Please, provide positive numbers for size\n";
            return;
        }
        
        int x = get<0>(coordinates); 
        int y = get<1>(coordinates); 

        int up_bound = grid[0].size();
        int right_bound = grid[0][0].size();

        for (double t = 0.0; t < 6.3; t+=0.1){
            int x_draw = (int)(radius * sin(t) + x);
            int y_draw = (int)(radius * cos(t) + y);
            
            if(x_draw>0 && x_draw < right_bound && up_bound - y_draw > 0 && y_draw>0){
                (*grid)[up_bound - y_draw][x_draw] = '*';                
            }
        }
    }

    void get_info() override {
        cout << "Circle: id(" << s_id << "), radius(" << radius << "), coordinates(" << get<0>(coordinates) << "," << get<1>(coordinates) << ")\n";
    }
};

class Line: public Figure{
private:
    int length;
    int angle;
    int s_id;
    tuple<int, int> coordinates;
public:
    Line(const int& length, const int& angle, const int& x, const int& y): length(length), angle(angle), coordinates(make_tuple(x,y)), s_id(id++) {}

    void add(vector<vector<char>>* grid) override{
        if (length < 0 || angle < 0) {
            cerr << "Please, provide positive numbers for size\n";
            return;
        } 

        int x = get<0>(coordinates); 
        int y = get<1>(coordinates); 

        int up_bound = grid[0].size();
        int right_bound = grid[0][0].size();

        double rad_angle = angle * 3.14 / 180.0;

        for (int i = 0; i < length; ++i) {
            int x_draw = x + (int)(i * cos(rad_angle));
            int y_draw = y + (int)(i * sin(rad_angle));

            if (x_draw > 0 && x_draw < right_bound && up_bound - y_draw > 0 && y_draw > 0) {
                (*grid)[up_bound - y_draw][x_draw] = '*';
            }
        }
    }

    void get_info() override {
        cout << "Line: id(" << s_id << "), length(" << length << "), angle(" << angle << "), coordinates(" << get<0>(coordinates) << "," << get<1>(coordinates) << ")\n";
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


class Blackboard
{
private:
    const int BOARD_WIDTH = 90;
    const int BOARD_HEIGHT = 50;

    vector<vector<char>> grid;
    vector<unique_ptr<Figure>> figures;
    vector<vector<char>> previous;
public:
    Blackboard() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

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

    void add_circle(const int& radius, const int& x, const int& y){
        previous = grid;

        auto figure = make_unique<Circle>(radius, x, y);
        figure->add(&grid);
        figures.push_back(move(figure));        
    }

    void add_line(const int& length, const int& angle, const int& x, const int& y){
        previous = grid;

        auto figure = make_unique<Line>(length, angle, x, y);
        figure->add(&grid);
        figures.push_back(move(figure));     
    }

    void shapes() {
        cout << MAGENTA << "Square:" << YELLOW << " size, coordinates" << BLUE << "[x,y]" << RESET << " of top left edge\n";
        cout << MAGENTA << "Circle:" << YELLOW << " radius, coordinates" << BLUE << "[x,y]" << RESET << " of centre\n";
        cout << MAGENTA << "Triangle:" << YELLOW << " height, coordinates" << BLUE << "[x,y]" << RESET << " of upmost edge\n";
        cout << MAGENTA << "Line:" << YELLOW << " length, angle, coordinates" << BLUE << "[x,y]" << RESET << " of left edge\n";
    };

    void list(){
        if(figures.size() > 0){
            for (auto& shape: figures){
                shape->get_info();
            }
        }
        else cout << " No figures on board\n";        
    }

    void undo(){
        if(figures.size() > 0){
            figures.pop_back();
            grid = previous;
        }
        else cout << "No figures on board\n";
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


class Parser {
private:
    Blackboard blackboard;
    vector<string> split(string& str, const string& delimiter) {
        vector<std::string> tokens;
        size_t pos = 0;
        string token;
        while ((pos = str.find(delimiter)) != string::npos) {
            token = str.substr(0, pos);
            tokens.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tokens.push_back(str);

        return tokens;
    }

public:
    void parse_command(string& command_line) {
    vector<string> parts = split(command_line, " ");
    if (parts.empty()) {
        cout << "Start typing" << endl;
        return;
    }

    string command = parts[0];

    if (command == "draw"){
        blackboard.draw();
    }
    else if (command == "list"){
        blackboard.list();
    }
    else if (command == "shapes"){
        blackboard.shapes();
    }
    else if (command == "undo"){
        blackboard.undo();
    }
    else if (command == "clear"){
        blackboard.clear();
    }
    else if (command == "save"){
        if(parts.size() > 1)
            blackboard.save(parts[1]);
        else 
            cout << "Please provide a filename to save.\n";
    } 
    else if (command == "load"){
        if(parts.size() > 1)
            blackboard.load(parts[1]);
        else 
            cout << "Please provide a filename to load.\n";
    }
    else if(command == "add"){
        if(parts.size() < 3) {
            cout << "Oups! It's incorrect command usage. Type shapes command to see correct usage\n";
            return;
        }
        string figure = parts[1];

        if (figure == "square" && parts.size() == 5){
            try {
                int size = stoi(parts[2]);
                int x = stoi(parts[3]);
                int y = stoi(parts[4]);
                blackboard.add_square(size, x, y);
            } catch (exception& e) {
                cout << "Please, provide only valid integers\n";
            }
        }

        else if (figure == "circle" && parts.size() == 5){
            try {
                int radius = stoi(parts[2]);
                int x = stoi(parts[3]);
                int y = stoi(parts[4]);
                blackboard.add_circle(radius, x, y);
            } catch (exception& e) {
                cout << "Please, provide only valid integers\n";
            }
        }

        else if (figure == "triangle" && parts.size() == 5){
            try {
                int height = stoi(parts[2]);
                int x = stoi(parts[3]);
                int y = stoi(parts[4]);
                blackboard.add_triangle(height, x, y);
            } catch (exception& e) {
                cout << "Please, provide only valid integers\n";
            }
        }

        else if (figure == "line" && parts.size() == 6){
            try {
                int length = stoi(parts[2]);
                int angle = stoi(parts[3]);
                int x = stoi(parts[4]);
                int y = stoi(parts[5]);
                blackboard.add_line(length, angle, x, y);
            } catch (exception& e) {
                cout << "Please, provide only valid integers\n";
            }
        }
        else {
            cout << "No such figure, enter 'shapes' to see available figures\n";
        }
    }
    else {
        cout << "No such command. Available commands are:\n"
             << "draw\nlist\nshapes\nundo\nclear\nsave\nload\nadd\n";
    }
}
};

class Engine {
public:
    void run() {
        string input;
        Parser parser;
        while (true) {
            cout << "Enter command: ";
            getline(cin, input);

            if (input == "exit") {
                break;
            }
            parser.parse_command(input);
        }
    }
};

int main() {
    Engine engine;

    engine.run();

    return 0;
}