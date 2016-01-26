#include <math.h>

// Assignment of values to board piece colors
const int WHITE = 2;
const int BLACK = 1;
const int BLANK = 0;

struct Square{
public:
    Square();
    int value; // Value to keep track of piece
};

Square::Square(){
    value = BLANK;
}