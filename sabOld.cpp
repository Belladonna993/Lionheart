#include "sab.h"
#include <cmath>
#include <iostream>


void sab::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
    bool done = false;
    int targetRow, targetCol;
    Direction toFace;
    while (!done) {
        targetRow = minR + rand() % (maxR - minR);
        targetCol = minC + rand() % (maxC - minC);
        if (sitrep.board[targetRow][targetCol].state == empty)done = true;
    }
    int rdist = ROWS / 2 - targetRow;
    int cdist = COLS / 2 - targetCol;
    if (abs(rdist) < abs(cdist)) {
        if (cdist > 0)toFace = right;
        else toFace = left;
    } else {
        if (rdist > 0)toFace = up;
        else toFace = dn;
    }
    row = targetRow;
    col = targetCol;
    facing = toFace;
}


// tell someone what you want to do
Action sab::Recommendation(SitRep sitrep) {

    // this code is provided as an example only
    // use at your own risk
    Action action;

    // first, try to attack in front of you
    int tr = row, tc = col;
    switch (facing) {
        case up:
            tr--;
            break;
        case dn:
            tr++;
            break;
        case right:
            tc++;
            break;
        case left:
            tc--;
            break;
        case none:
            break;
    }
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS) {
        if (sitrep.board[tr][tc].state == unit) {
            if (sitrep.board[tr][tc].owner != tla) {
                action.action = attack;
                action.archerRow = tr;
                action.archerCol = tc;
                return action;
            }
        }
    }
    // there is not an enemy in front of me
    // so head to the nearest enemy
    if (facing == sitrep.nearestEnemy.dirFor) {
        action.action = moveFwd;
        action.maxDist = 1;
        if (rank == knight || rank == crown)action.maxDist = HORSESPEED;
        return action;
    } else {
        action.action = turn;
        action.dirToFace = sitrep.nearestEnemy.dirFor;
        return action;
    }
    action.action = nothing;
    return action;

}
