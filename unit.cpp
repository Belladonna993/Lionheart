#include "unit.h"
#include <cmath>

float dist(int ar, int ac, int br, int bc) {
    return sqrt((double) (br - ar) * (br - ar) + (bc - ac) * (bc - ac));
}

Unit::Unit() {
    row = 0;
    col = 0;
    hp = 0;
    facing = up;
    rank = infantry;
    dead = true;
}

Unit::Unit(int ir, int ic, int ihp, Direction idir, Rank irank, bool idead, string itla) {
    row = ir;
    col = ic;
    hp = ihp;
    facing = idir;
    rank = irank;
    dead = idead;
    tla = itla;
}

int Unit::getR() { return row; }

int Unit::getC() { return col; }

int Unit::getHp() { return hp; }

Direction Unit::getDir() { return facing; }

Rank Unit::getRank() { return rank; }

bool Unit::getDead() { return dead; }

string Unit::getTla() { return tla; }

void Unit::Turn(Direction newDir) { facing = newDir; }

void Unit::Move(int dist) {
    switch (facing) {
        case up:
            row -= dist;
            break;
        case dn:
            row += dist;
            break;
        case toLeft:
            col -= dist;
            break;
        case toRight:
            col += dist;
            break;
        case none:
            break;
    }
}

int Unit::Attack() {
    int i = 0, hits = 0, d;
    int rolls;
    rolls = hp;
    if (rank == crown || rank == knight)rolls *= 2;

    int tr = row, tc = col;
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

    for (i = 0; i < rolls; ++i) {
        d = rand() % 6;
        if (rank == archer && (d == 3 || d == 4))hits++;
        else if (d < 3)hits++;
    }

    //if I am an archer or infantry,
    //and there is only one of me,
    //and i rolled 1 hit,
    //then i am allowed to roll again
    if ((rank == archer || rank == infantry) && hp == 1 && hits == 1)hits += rand() % 2;

    return hits;
}

void Unit::Suffer(int hits) {
    if (rank == crown || rank == knight)hits /= 2;
    hp -= hits;
    if (hp <= 0) {
        dead = true;
    }
}

void Unit::Place(int minR, int maxR, int minC, int maxC, SitRep sitrep) {
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
Action Unit::Recommendation(SitRep sitrep) {

    // this code is provided as an example only
    // use at your own risk
    Action a;

    // first, try to attack in front of you
    int tr = row, tc = col;
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
    // there is not an enemy in front of me
    // so just do something random
    if (facing == sitrep.nearestEnemy.direction) {
        a.action = moveFwd;
        a.maxDist = 1;
        if (rank == knight || rank == crown)a.maxDist = HORSESPEED;
        return a;
    } else {
        a.action = turn;
        a.dirToFace = sitrep.nearestEnemy.direction;
        return a;
    }
    a.action = nothing;
    return a;

}



