// this file was submitted by <Derek> <Withers> - <drw>

#include "drw.h"
#include <cmath>
#include <iostream>


void drw::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
    bool done = false;
    int tr, tc;
    Direction td;
    while (!done) {
        tr = minR + rand() % (maxR - minR);
        tc = minC + rand() % (maxC - minC);
        if (sitrep.board[tr][tc].state == empty)done = true;
    }
    int rdist = ROWS / 2 - tr;
    int cdist = COLS / 2 - tc;
    if (abs(rdist) < abs(cdist)) {
        if (cdist > 0)td = toRight;
        else td = toLeft;
    } else {
        if (rdist > 0)td = up;
        else td = dn;
    }
    row = tr;
    col = tc;
    facing = td;
}


// tell someone what you want to do
Action drw::Recommendation(SitRep sitrep) {


    Action a;
    Action b;
    int way;
    // first, try to attack in front of you
    int tr = row, tc = col;
    int kr, kc, fr, fc;
//Archer attack
    if (rank == archer) {
        switch (facing) {
            case up:
                kr = tr - 3;
                fr = tr - 1;
                kc = tc - 1;
                fc = tc + 1;
                break;
            case dn:
                kr = tr + 1;
                fr = tr + 3;
                kc = tc - 1;
                fc = tc + 1;
                break;
            case toLeft:
                kr = tr - 1;
                fr = tr + 1;
                kc = tc - 3;
                fc = tc - 1;
                break;
            case toRight:
                kr = tr - 1;
                fr = tr + 1;
                kc = tc + 1;
                fc = tc + 3;
                break;
            case none:
                break;
        }
        if (kr < 0)kr = 0;
        if (fr >= ROWS)fr = ROWS - 1;
        if (kc < 0)kc = 0;
        if (fc >= COLS)fc = COLS - 1;

        for (int i = kr; i <= fr; i++) {
            for (int k = kc; k <= fc; k++) {
                if (sitrep.board[i][k].state == unit) {
                    tr = i;
                    tc = k;
                }
            }
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
        // Every one else attack
    else {
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
//Crown runs away from nearest enemy
    if (rank == crown) {
        if (facing == sitrep.nearestEnemy.direction || rand() % 2) {
            a.action = turn;
            a.dirToFace = sitrep.nearestEnemy.direction;
            b.dirToFace = a.dirToFace;
            while (b.dirToFace == a.dirToFace) {
                /*if (a.dir == lt)a.dir = up;
                else if (a.dir == rt)a.dir = dn;
                else if (a.dir == up)a.dir = rt;
                else if (a.dir == dn)a.dir = lt;*/
                way = rand() % 4;
                switch (way) {
                    case 0:
                        b.dirToFace = toLeft;
                        break;
                    case 1:
                        b.dirToFace = toRight;
                        break;
                    case 2:
                        b.dirToFace = up;
                        break;
                    case 3:
                        b.dirToFace = dn;
                        break;
                    default:
                        b.dirToFace = none;
                        break;
                }
            }
            a.dirToFace = b.dirToFace;
        }
        else {
            a.action = moveFwd;
            a.maxDist = 1;
            if (rank == knight || rank == crown)a.maxDist = HORSESPEED;
            if (rank == knight || rank == crown)a.maxDist = HORSESPEED - 1;
        }
        return a;
    }
        //Archers head toward Enemy Crown
    else if (rank == archer) {
        if (facing == sitrep.nearestEnemyCrown.direction) {
            a.action = moveFwd;
            a.maxDist = 1;
            if (rank == knight || rank == crown)a.maxDist = HORSESPEED;
            return a;

            //if not pointed in the right direction, turn
        }
        else {
            a.action = turn;
            a.dirToFace = sitrep.nearestEnemyCrown.direction;
            return a;
        }
    }
        //All other ranks move toward nearest enemy.
        //if I'm pointed in the right direction, move
    else if (facing == sitrep.nearestEnemy.direction) {
        a.action = moveFwd;
        a.maxDist = 1;
        if (rank == knight || rank == crown)a.maxDist = HORSESPEED;
        if (rank == knight || rank == crown)a.maxDist = HORSESPEED - 1;
        return a;

        //if not pointed in the right direction, turn
    }
    else {
        a.action = turn;
        a.dirToFace = sitrep.nearestEnemy.direction;
        return a;
    }

    //otherwise, just do nothing
    a.action = nothing;
    return a;

}
