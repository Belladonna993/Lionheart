#include "smb.h"
#include <cmath>
#include <iostream>


void smb::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
    bool done = false;
    int tr, tc, i = 1, j = 1;
    Direction td;
    while (!done) {
        if (minC + i < maxC) {
            tr = minR + j;
            tc = minC + i;
            i++;
        }
        else {
            j++;
            i = 1;
        }

        if (sitrep.board[tr][tc].state == empty)done = true;
    }
    int rdist = ROWS / 2 - tr;
    int cdist = COLS / 2 - tc;
    if (abs(rdist) < abs(cdist)) {
        if (cdist > 0)td = toRight;
        else td = toLeft;
    }
    else {
        if (rdist > 0)td = up;
        else td = dn;
    }
    row = tr;
    col = tc;
    facing = td;
}


// tell someone what you want to do
Action smb::Recommendation(SitRep sitrep) {

    // this code is provided as an example only
    // use at your own risk
    Action a;

    // first, try to attack in front of you
    int tr = row, tc = col;
    if (sitrep.board[row][col].rank != archer) {
        switch (facing) {
            case up:
                tr--;
                break;
            case dn:
                tr++;
                break;
            case toRight:
                tc++;
                break;
            case toLeft:
                tc--;
                break;
            case none:
                break;
        }
        if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS) {
            if (sitrep.board[tr][tc].state == unit) {
                if (sitrep.board[tr][tc].owner != tla) {
                    a.action = attack;
                    a.archerRow = tr;
                    a.archerCol = tc;
                    return a;
                }
            }
        }
    }
    // there is not an enemy in front of me
    // so head to the nearest enemy
    Direction dirt = sitrep.nearestEnemy.direction;
    Direction soil;

    if (dirt == up) soil = dn;
    if (dirt == dn) soil = up;
    if (dirt == toLeft) soil = toRight;
    if (dirt == toRight) soil = toLeft;

    if (facing == sitrep.nearestEnemy.direction && rank != crown) {
        a.action = moveFwd;
        a.maxDist = 1;
        if (rank == knight) a.maxDist = HORSESPEED;
        return a;
    }
    else if (facing == dirt && rank == crown) {

        a.action = turn;
        if (dirt == up) a.dirToFace = dn;
        if (dirt == dn) a.dirToFace = up;
        if (dirt == toLeft) a.dirToFace = toRight;
        if (dirt == toRight) a.dirToFace = toLeft;
        return a;
    }
    else if (facing == soil && rank == crown) {
        a.action = moveFwd;
        a.maxDist = HORSESPEED;
        return a;
    }
    else {
        a.action = turn;
        a.dirToFace = sitrep.nearestEnemy.direction;
        return a;
    }
    a.action = nothing;
    return a;

}
