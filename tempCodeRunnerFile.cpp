 void undo(){
        if(figures.size() > 0){
            figures.pop_back();
            grid = previous;
        }
        else cout << "No figures on board\n";
    }