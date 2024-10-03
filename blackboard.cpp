#include <iostream>
using namespace std;
#include <vector>

class blackboard
{
private:
    const int BOARD_WIDTH = 160;
    const int BOARD_HEIGHT = 50;

    vector<vector<char>> grid;
    vector<Figure> figures;
public:
    blackboard() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}
    

};

class Figure{
public:
    virtual void draw(){};
    virtual void shapes(){};
    
};

int main() {
   
    return 0;
}