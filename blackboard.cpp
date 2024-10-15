#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <cmath>
#include <cctype> 
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define BLUE   "\033[34m"
#define YELLOW "\033[33m"
#define BLACK  "\033[30m"
#define WHITE  "\033[37m"
#define CYAN   "\033[36m"
#define MAGENTA "\033[35m"
#define PURPLE "\033[35m"
#define RESET  "\033[0m"

class Cell;

class Figure{
public:
    static int id;

    virtual ~Figure() { id--; }

    virtual void add(vector<vector<weak_ptr<Cell>>>* grid) = 0;
    virtual const string get_info() = 0;
    virtual const bool get_placement() = 0;
    virtual bool operator==(const Figure& other) const = 0;
    virtual char get_symbol() const = 0;
    virtual const char* get_color() const = 0;
    virtual int get_id() const = 0;
    virtual string get_type() const = 0;
    virtual void set_color(pair<char, const char*> color) = 0;
};

int Figure::id = 0;

class Cell{
private:
    vector<Figure*> figures;
    char symbol;
    const char* color;
public:
    Cell(const char* symbol, const char* color) : symbol(*symbol), color(color) {}

    void addFigure(Figure* figure) {
        figures.push_back(figure);
    }

    Figure* lastFigure(){
        if(!figures.empty()){
            return figures.back();
        }
        return nullptr;
    }
    
    char get_symbol(){
        return figures.back()->get_symbol();
    }

    const char* get_color(){
        return figures.back()->get_color();
    }
};

class Square: public Figure{
private: 
    int size;
    int s_id;
    tuple<int,int> coordinates; // coordinates of square top_left
    bool if_outside;
    pair<char, const char*> color;
    char display_char;
    const char* display_color;
    vector<shared_ptr<Cell>> occupiedCells;
    bool fill;

public:
    Square(bool fill, pair<char, const char*> color, const int& size, const int& x, const int& y): size(size), coordinates(make_tuple(x,y)), s_id(id++),
     display_char(color.first), display_color(color.second), fill(fill) { }

    Square(Square&& other, const int& new_size) : size(new_size), coordinates(std::move(other.coordinates)), s_id(other.s_id),
     display_char(other.display_char), display_color(other.display_color), fill(other.fill) 
    {
        other.size = 0;
    }

    void add(vector<vector<weak_ptr<Cell>>>* grid) override{
        if (size <= 0){
            cerr << "Please, provide positive numbers for size\n";
            return;
        } 

        int x = get<0>(coordinates); 
        int y = get<1>(coordinates); 

        int up_bound = grid[0].size();
        int right_bound = grid[0][0].size();

        if_outside = y+size > up_bound || y < 0 || x + size < 0 || x > right_bound; 

        if (if_outside){
            return;
        }

        for (int i = 0; i < size; i++) {
            int current_row = y - i;

            if (current_row >= 0 && current_row < up_bound) {
                if (i==0 || i == size-1) {
                    for (int j = x; j < x + size; j++) {
                        if (j >= 0 && j < right_bound){
                            shared_ptr<Cell> cell = (*grid)[up_bound - current_row - 1][j].lock();

                            if (!cell) {
                                cell = make_shared<Cell>(&display_char, display_color);
                                (*grid)[up_bound - current_row - 1][j] = cell;
                                occupiedCells.push_back(cell);  
                            }
                            cell->addFigure(this);
                        }
                    }
                    continue;
                }
            }

            if (current_row >= 0 && current_row < up_bound){
                if (x >= 0 && x < right_bound) {
                    shared_ptr<Cell> cell = (*grid)[up_bound - current_row - 1][x].lock();

                    if (!cell) {
                        cell = make_shared<Cell>(&display_char, display_color);
                        (*grid)[up_bound - current_row - 1][x] = cell;
                        occupiedCells.push_back(cell);
                    }
                    cell->addFigure(this);
                }

                if (x + size - 1 >= 0 && x + size - 1 < right_bound) {
                    shared_ptr<Cell> cell = (*grid)[up_bound - current_row - 1][x+size-1].lock();

                    if (!cell) {
                        cell = make_shared<Cell>(&display_char, display_color);
                        (*grid)[up_bound - current_row - 1][x+size-1] = cell;
                        occupiedCells.push_back(cell);
                    }
                    cell->addFigure(this);
                }
            }
        }
            
        if (fill) {
            for (int i = 1; i < size - 1; i++) {
                int current_row = y - i;
                if (current_row >= 0 && current_row < up_bound) {
                    for (int j = x; j < x + size; j++) {
                        if (j >= 0 && j < right_bound) {
                            shared_ptr<Cell> cell = (*grid)[up_bound - current_row - 1][j].lock();

                            if (!cell) {
                                cell = make_shared<Cell>(&display_char, display_color);
                                (*grid)[up_bound - current_row - 1][j] = cell;
                                occupiedCells.push_back(cell);
                            }
                            cell->addFigure(this);
                        }
                    }
                }
            }
        }
    }

    const string get_info() override {
        stringstream info;
        info << "Square: id(" << s_id << "), size( " << size << " ), coordinates( " << get<0>(coordinates) << "," << get<1>(coordinates)
         << " ), color( " << display_char << " ), filled( "<< (fill ? "yes" : "no") <<" )\n";
        cout << info.str();
        return info.str();
    }

    const bool get_placement() override{
        return if_outside;
    }

    bool operator==(const Figure& other) const override{
        const Square* otherFigure = dynamic_cast<const Square*>(&other);
        if (otherFigure){
            return size == otherFigure->size && coordinates == otherFigure->coordinates;
        }
        return false;
    }

    char get_symbol() const override{
        return display_char;
    }

    const char* get_color() const override{
        return display_color;
    }

    int get_id()const override{
        return s_id;
    }
    
    string get_type() const override{
        return "Square";
    }

    void set_color(pair<char, const char*> new_color) override{
        display_color = new_color.second;
        display_char = new_color.first;
    }
};

// class Triangle: public Figure{
// private:
//     int height;
//     int s_id;
//     tuple<int,int> coordinates;
//     bool if_outside;
// public:
//     Triangle(const int& height, const int& x, const int& y): height(height), coordinates(make_tuple(x,y)), s_id(id++) {}

//     void add(vector<vector<char>>* grid) override{
//         if (height <= 0) return;
        
//         int x = get<0>(coordinates); 
//         int y = get<1>(coordinates); 

//         int up_bound = grid[0].size();
//         int right_bound = grid[0][0].size();

//         if_outside = y+height > up_bound || y < 0 || x + height/2 < 0 || x-height/2 > right_bound; 

//         if (if_outside){
//             return;
//         }

//         for (int i = 0; i < height; ++i) {
//             int leftMost = x - i;    
//             int rightMost = x + i;   
//             int posY = y - i;        

            
//             if ((posY) >= 0 && (posY) <= up_bound) {
//                 if (leftMost >= 0 && leftMost < right_bound) {
//                     (*grid)[up_bound - posY][leftMost] = '*';
//                 }
            
//                 if (rightMost >= 0 && rightMost < right_bound) {
//                     (*grid)[up_bound - posY][rightMost] = '*';
//                 }
//             }
//         }

    
//         int baseY = y - height + 1; 
//         for (int j = 0; j < 2 * height - 1; ++j) {
//             int baseX = x + height - 1 - j;
//             if (baseX >= 0 && baseX < right_bound && (baseY) >= 0 && (baseY) <= up_bound) {
//                 (*grid)[up_bound-baseY][baseX] = '*'; 
//             }
//         }
//     }

//     const string get_info() override {
//         stringstream info;
//         info << "Triangle: id(" << s_id << "), height( " << height << " ), coordinates( " << get<0>(coordinates) << "," << get<1>(coordinates) << " )\n";
//         cout << info.str();
//         return info.str();
//     }

//     const bool get_placement() override{
//         return if_outside;
//     }

//     bool operator==(const Figure& other) const override{
//         const Triangle* otherFigure = dynamic_cast<const Triangle*>(&other);
//         if (otherFigure){
//             return height == otherFigure->height && coordinates == otherFigure->coordinates;
//         }
//         return false;
//     }
// };

// class Circle: public Figure{
// private:
//     int radius;
//     int s_id;
//     tuple<int, int> coordinates;
//     bool if_outside;
// public:
//     Circle(const int& radius, const int& x, const int& y): radius(radius), coordinates(make_tuple(x,y)), s_id(id++) {}

//     void add(vector<vector<char>>* grid) override{
//         if (radius <= 0){
//             cerr << "Please, provide positive numbers for size\n";
//             return;
//         }
        
//         int x = get<0>(coordinates); 
//         int y = get<1>(coordinates); 

//         int up_bound = grid[0].size();
//         int right_bound = grid[0][0].size();

//         if_outside = y+radius > up_bound || y-radius < 0 || x + radius < 0 || x - radius  > right_bound; 

//         if (if_outside){
//             return;
//         }

//         for (double t = 0.0; t < 6.3; t+=0.1){
//             int x_draw = (int)(radius * sin(t) + x);
//             int y_draw = (int)(radius * cos(t) + y);
            
//             if(x_draw>0 && x_draw < right_bound && up_bound - y_draw > 0 && y_draw>0){
//                 (*grid)[up_bound - y_draw][x_draw] = '*';                
//             }
//         }
//     }

//     const string get_info() override {
//         stringstream info;
//         info << "Circle: id(" << s_id << "), radius( " << radius << " ), coordinates( " << get<0>(coordinates) << "," << get<1>(coordinates) << " )\n";
//         cout << info.str();
//         return info.str();
//     }

//     const bool get_placement() override{
//         return if_outside;
//     }

//     bool operator==(const Figure& other) const override{
//         const Circle* otherFigure = dynamic_cast<const Circle*>(&other);
//         if (otherFigure){
//             return radius == otherFigure->radius && coordinates == otherFigure->coordinates;
//         }
//         return false;
//     }
// };

// class Line: public Figure{
// private:
//     int length;
//     int angle;
//     int s_id;
//     tuple<int, int> coordinates;
//     bool if_outside;
// public:
//     Line(const int& length, const int& angle, const int& x, const int& y): length(length), angle(angle), coordinates(make_tuple(x,y)), s_id(id++) {}

//     void add(vector<vector<char>>* grid) override{
//         if (length < 0 || angle < 0) {
//             cerr << "Please, provide positive numbers for size\n";
//             return;
//         } 

//         int x = get<0>(coordinates); 
//         int y = get<1>(coordinates); 

//         int up_bound = grid[0].size();
//         int right_bound = grid[0][0].size();

//         double rad_angle = angle * 3.14 / 180.0;

//         if_outside = y + (int)(length * sin(rad_angle)) > up_bound || y-(int)(length * sin(rad_angle)) < 0 || x + (int)(length * cos(rad_angle)) < 0 || x > right_bound; 

//         if (if_outside){
//             return;
//         }

//         for (int i = 0; i < length; ++i) {
//             int x_draw = x + (int)(i * cos(rad_angle));
//             int y_draw = y + (int)(i * sin(rad_angle));

//             if (x_draw > 0 && x_draw < right_bound && up_bound - y_draw > 0 && y_draw > 0) {
//                 (*grid)[up_bound - y_draw][x_draw] = '*';
//             }
//         }
//     }

//     const string get_info() override {
//         stringstream info;
//         info << "Line: id(" << s_id << "), length( " << length << " ), angle( " << angle << " ), coordinates( " << get<0>(coordinates) << "," << get<1>(coordinates) << " )\n";
//         cout << info.str();
//         return info.str();
//     }

//     const bool get_placement() override{
//         return if_outside;
//     }

//     bool operator==(const Figure& other) const override{
//         const Line* otherFigure = dynamic_cast<const Line*>(&other);
//         if (otherFigure){
//             return angle == otherFigure->angle && length == otherFigure->length && coordinates == otherFigure->coordinates;
//         }
//         return false;
//     }
// };

class Blackboard
{
private:
    const int BOARD_WIDTH = 90;
    const int BOARD_HEIGHT = 50;

    map<string, pair<char, const char*>> ALLOWED_COLORS = {
        {"red", {'r', RED}},
        {"green", {'g', GREEN}},
        {"blue", {'b', BLUE}},
        {"yellow", {'y', YELLOW}},
        {"black", {'k', BLACK}},
        {"white", {'w', WHITE}},
        {"cyan", {'c', CYAN}},
        {"magenta", {'m', MAGENTA}},
        {"purple", {'p', PURPLE}}
    };
    Figure* selected_figure;
    vector<vector<weak_ptr<Cell>>> grid;;
    vector<unique_ptr<Figure>> figures;
    vector<vector<weak_ptr<Cell>>> previous;
public:
    Blackboard() : grid(BOARD_HEIGHT, vector<weak_ptr<Cell>>(BOARD_WIDTH)), 
                    previous(BOARD_HEIGHT, vector<weak_ptr<Cell>>(BOARD_WIDTH)) {}


    void draw() {
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << RED << "-" << RESET; 
        }
        cout << "\n";
        for (auto& row : grid) {
            cout << RED << '|' << RESET;
            for (auto& c : row) {

                shared_ptr<Cell> cell = c.lock();

                if (cell!=nullptr){
                    cout << cell->get_color() << cell->get_symbol(); 
                }
                else{
                    cout << " ";
                }
            }
            cout  << RED << " |\n" << RESET;
        }
        cout << " ";
        for (int j = 0; j<= BOARD_WIDTH; j++){
            cout << RED << "-" << RESET; 
        }
        cout << '\n';
    }

    void add_square(string fill, const string& color, const int& size, const int& x, const int& y){
        previous = grid;
        bool flag;

        if (fill == "fill"){
            flag = true;
        }
        else flag = false;

        auto new_figure = make_unique<Square>(flag,ALLOWED_COLORS[color],size, x, y);

        for (const auto& figure : figures) {
            Square* existing_square = dynamic_cast<Square*>(figure.get());
            if (existing_square && *existing_square == *new_figure) {
                cout << "Same figure exists\n";
                return;
            }
        }
        new_figure->add(&grid);

        if (new_figure.get()->get_placement()){
            cout << "Figure is outside the box\n";
            return;
        }

        figures.push_back(move(new_figure));
    } 

    // void add_triangle(const int& height, const int& x, const int& y){
    //     previous = grid;

    //     auto new_figure = make_unique<Triangle>(height, x, y);

    //     for (const auto& figure : figures) {
    //         Triangle* existing_triangle = dynamic_cast<Triangle*>(figure.get());
    //         if (existing_triangle && *existing_triangle == *new_figure) {
    //             cout << "Same figure exists\n";
    //             return;
    //         }
    //     }
    //     new_figure->add(&grid);

    //     if (new_figure.get()->get_placement()){
    //         cout << "Figure is outside the box\n";
    //         return;
    //     }

    //     figures.push_back(move(new_figure));  
    // } 

    // void add_circle(const int& radius, const int& x, const int& y){
    //     previous = grid;

    //     auto new_figure = make_unique<Circle>(radius, x, y);

    //     for (const auto& figure : figures) {
    //         Circle* existing_circle = dynamic_cast<Circle*>(figure.get());
    //         if (existing_circle && *existing_circle == *new_figure) {
    //             cout << "Same figure exists\n";
    //             return;
    //         }
    //     }
    //     new_figure->add(&grid);

    //     if (new_figure.get()->get_placement()){
    //         cout << "Figure is outside the box\n";
    //         return;
    //     }

    //     figures.push_back(move(new_figure));       
    // }

    // void add_line(const int& length, const int& angle, const int& x, const int& y){
    //     previous = grid;

    //     auto new_figure = make_unique<Line>(length, angle, x, y);

    //     for (const auto& figure : figures) {
    //         Line* existing_line = dynamic_cast<Line*>(figure.get());
    //         if (existing_line && *existing_line == *new_figure) {
    //             cout << "Same figure exists\n";
    //             return;
    //         }
    //     }
    //     new_figure->add(&grid);

    //     if (new_figure.get()->get_placement()){
    //         cout << "Figure is outside the box\n";
    //         return;
    //     }

    //     figures.push_back(move(new_figure));     
    // }

    void shapes() {
        cout << MAGENTA << "Square:" << YELLOW << " size, coordinates" << BLUE << "[x,y]" << RESET << " of top left edge\n";
        cout << MAGENTA << "Circle:" << YELLOW << " radius, coordinates" << BLUE << "[x,y]" << RESET << " of centre\n";
        cout << MAGENTA << "Triangle:" << YELLOW << " height, coordinates" << BLUE << "[x,y]" << RESET << " of upmost edge\n";
        cout << MAGENTA << "Line:" << YELLOW << " length, angle, coordinates" << BLUE << "[x,y]" << RESET << " of left edge\n";
    };

    const vector<string> list(){
        vector<string> figures_info;
        if(figures.size() > 0){
            for (auto& shape: figures){
                figures_info.push_back(shape->get_info());
            }
        }
        else cout << " No figures on board\n";

        return figures_info;        
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
    }

    void select_figure_by_coord(const int& x, const int& y){
        shared_ptr<Cell> cell = grid[BOARD_HEIGHT-y][x].lock(); 

        if (cell!=nullptr){
            selected_figure = cell->lastFigure();
            cout << "Selected ";
            selected_figure->get_info();
        }
        else{
            cout << "There is no figure on coordinates\n";
        }
    }

    void select_by_id(const int& id){
        if(figures.size() > 0){
            for (auto& shape: figures){
                if (id == shape->get_id()){
                    selected_figure = shape.get();
                    cout << "Selected: ";
                    shape->get_info();
                }
            }
        }
        else cout << "No figures with that id\n";
    }

    void remove(){
        if(figures.size() > 0){
            for(int i = 0; i < figures.size(); i++){
                if (selected_figure->get_id() == figures[i]->get_id()){
                    selected_figure->get_info();
                    figures.erase(figures.begin() + i);                  
                    cout << "Was removed\n";
                }
            }
        }
    }

    void edit(const int& size){
        if (auto selected_square = dynamic_cast<Square*>(selected_figure)) {
            auto new_square = make_unique<Square>(move(*selected_square), size);
            
            new_square->add(&grid);

            if (new_square.get()->get_placement()){
                cout << "Figure is outside the box\n";
                new_square.release();
                return;
            }
            figures.push_back(move(new_square));

            if(figures.size() > 0){
                for(int i = 0; i < figures.size(); i++){
                    if (selected_figure->get_id() == figures[i]->get_id()){
                        figures.erase(figures.begin() + i);
                    }
                }
            }

        } else {
        cout << "Selected figure is not a square.\n"; }
    }

    void paint(const string& new_color){
        selected_figure->set_color(ALLOWED_COLORS[new_color]);
    }
};

class FileSystem {
private:
    string path;
    vector<vector<char>> grid;
    Blackboard* blackboard;
public:
    FileSystem(const string& path, Blackboard* blackboard): path(path), blackboard(blackboard){}

    void save(){
        ofstream file;
        file.open(path);
        vector<string> figure_list = blackboard->list();

        if (figure_list.size() != 0){
            for (auto figure : figure_list){
                file << figure;
            }
        }
        else file << 0;

        cout << "File has been successfully saved!\n";
        file.close();
    }

    void load(){
        string line;
        ifstream file (path);

        blackboard->clear();

        if (file.is_open()){
            while (getline(file, line)) {
                if(line[0] == '0'){
                    return;
                }
                else{

                    string figure;
                    string label, first_param, second_param;
                    string id;
                    char paren_close, comma;
                    int third_param, x, y;

                    stringstream ss(line);
                    ss >> figure;
                    figure.pop_back();
                    ss >> id;
                    
                    if (figure == "Square"){
                        ss >> label >> first_param >> paren_close >> comma;
                        ss >> label >> second_param >> paren_close >> comma;
                        ss >> label >> third_param >> paren_close >> comma;
                        ss >> label >> x >> comma >> y >> paren_close;
                        cout << first_param << "|" << x << "|" << y << endl; 
                        blackboard->add_square(first_param, second_param, third_param, x, y);
                    }
                    // else if (figure == "Circle"){
                    //     ss >> label >> first_param >> paren_close >> comma;
                    //     ss >> label >> x >> comma >> y >> paren_close;
                    //     blackboard->add_circle(first_param, x, y);
                    // }
                    // else if (figure == "Triangle"){
                    //     ss >> label >> first_param >> paren_close >> comma;
                    //     ss >> label >> x >> comma >> y >> paren_close;
                    //     blackboard->add_triangle(first_param, x, y);
                    // }
                    // else if (figure == "Line"){
                    //     ss >> label >> first_param >> paren_close >> comma;
                    //     ss >> label >> second_param >> paren_close >> comma;
                    //     ss >> label >> x >> comma >> y >> paren_close;
                    //     blackboard->add_line(first_param, second_param, x, y);
                    // }
                    else {
                        cout << "File structure damaged\n";
                        file.close();
                        return;
                    }
                }
            }
        }
        else cout << "Unable to open file\n"; 
        file.close();
    }
};

class Parser {
private:
    Blackboard* blackboard;
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
     Parser(Blackboard* blackboard) : blackboard(blackboard) {}

    void parse_command(string& command_line) {
    vector<string> parts = split(command_line, " ");
    if (parts.empty()) {
        cout << "Start typing" << endl;
        return;
    }

    string command = parts[0];

    if (command == "draw"){
        blackboard->draw();
    }
    else if (command == "list"){
        blackboard->list();
    }
    else if (command == "shapes"){
        blackboard->shapes();
    }
    else if (command == "undo"){
        blackboard->undo();
    }
    else if (command == "clear"){
        blackboard->clear();
    }
    else if (command == "remove"){
        blackboard->remove();
    }
    else if(command == "edit"){
        blackboard->edit(stoi(parts[1]));
    }
    else if(command == "paint"){
        blackboard->paint(parts[1]);
    }
    else if (command == "select"){
        if(parts.size() == 3){
            blackboard->select_figure_by_coord(stoi(parts[1]), stoi(parts[2]));
        }
        else{ blackboard->select_by_id(stoi(parts[1])); }
    }
    else if (command == "save"){
        if(parts.size() > 1){
            FileSystem fs(parts[1], blackboard);
            fs.save();
        }
        else cout << "Please provide a filename to save.\n";
    } 
    else if (command == "load"){
        if(parts.size() > 1){
            FileSystem fs(parts[1], blackboard);
            fs.load();
        }            
        else cout << "Please provide a filename to load.\n";
    }
    else if(command == "add"){
        if(parts.size() < 4) {
            cout << "Oups! It's incorrect command usage. Type shapes command to see correct usage\n";
            return;
        }
        string figure = parts[1];
        string fill = parts[2];
        string color = parts[3];
        

        if (figure == "square" && parts.size() == 7){
            try {
                int size = stoi(parts[4]);
                int x = stoi(parts[5]);
                int y = stoi(parts[6]);
                blackboard->add_square(fill,color, size, x, y);
            } catch (exception& e) {
                cout << "Please, provide only valid integers\n";
            }
        }

        // else if (figure == "circle" && parts.size() == 5){
        //     try {
        //         int radius = stoi(parts[2]);
        //         int x = stoi(parts[3]);
        //         int y = stoi(parts[4]);
        //         blackboard->add_circle(radius, x, y);
        //     } catch (exception& e) {
        //         cout << "Please, provide only valid integers\n";
        //     }
        // }

        // else if (figure == "triangle" && parts.size() == 5){
        //     try {
        //         int height = stoi(parts[2]);
        //         int x = stoi(parts[3]);
        //         int y = stoi(parts[4]);
        //         blackboard->add_triangle(height, x, y);
        //     } catch (exception& e) {
        //         cout << "Please, provide only valid integers\n";
        //     }
        // }

        // else if (figure == "line" && parts.size() == 6){
        //     try {
        //         int length = stoi(parts[2]);
        //         int angle = stoi(parts[3]);
        //         int x = stoi(parts[4]);
        //         int y = stoi(parts[5]);
        //         blackboard->add_line(length, angle, x, y);
        //     } catch (exception& e) {
        //         cout << "Please, provide only valid integers\n";
        //     }
        // }
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
        Blackboard blackboard;
        string input;
        Parser parser(&blackboard);
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