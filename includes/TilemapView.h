#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#define DIRECTION_NORTH 1
#define DIRECTION_SOUTH 2
#define DIRECTION_EAST 3
#define DIRECTION_WEST 4

class TilemapView {
public:
    virtual void positionTile(const int col, const int row, const float tw, const float th, float &targetx, float &targety) const = 0;
    virtual void computeTileWalking(int &col, int &row, const int direction) const = 0;
};

#endif // TILEMAPVIEW_H
