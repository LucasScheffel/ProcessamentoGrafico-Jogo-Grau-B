#include "TilemapView.h"
#include <iostream>
using namespace std;

class DiamondView : public TilemapView {
public:
    void positionTile(const int col, const int row, const float tw, const float th, float &targetx, float &targety) const {
        targetx = col * tw / 2 + row * tw/2;
        targety = col * th / 2 - row * th / 2;
    }
    
    void computeTileWalking(int &col, int &row, const int direction) const {
        switch(direction){
            case DIRECTION_NORTH: 
                row --;
                break;
            case DIRECTION_EAST: 
                col ++;
                break;
            case DIRECTION_SOUTH: 
                row ++;
                break;
            case DIRECTION_WEST: 
                col --;
                break;
        }
    } 
    
};
