#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include "lionheart.h"

// A few globals, just to be Old School
Space b[ROWS][COLS];
char c[ROWS][COLS];
int numplayers;
int numTurns;
string tla[4];
string color[4];
Box startBox[4];
stringstream sout;
string winner;


void unit2thing(Unit *u, int r, int c) {
    if (!u) {
        b[r][c].state = empty;
        return;
    }
    if (u->getDead()) {
        b[r][c].state = empty;
        return;
    }
    b[r][c].state = unit;
    b[r][c].rank = u->getRank();
    b[r][c].owner = u->getTla();
    b[r][c].facing = u->getDir();
    b[r][c].hp = u->getHp();
}

void makeB(Unit *u[NUM]) {
    int i, j;
    for (i = 0; i < ROWS; ++i) {
        for (j = 0; j < COLS; ++j) {
            b[i][j].state = empty;
            if (c[i][j] == 'X')b[i][j].state = rock;
        }
    }

    for (i = 0; i < NUM; ++i) {
        if (u[i])unit2thing(u[i], u[i]->getR(), u[i]->getC());
    }
}

bool nextToBarrier(Unit *u) {
    int r, c;
    r = u->getR();
    c = u->getC();
    if (r - 1 > 0 && c - 1 > 0 && b[r - 1][c - 1].state == rock)return true;
    if (r - 1 > 0 && b[r - 1][c].state == rock)return true;
    if (r - 1 > 0 && c + 1 <= COLS && b[r - 1][c + 1].state == rock)return true;
    if (c - 1 > 0 && b[r][c - 1].state == rock)return true;
    if (c + 1 <= COLS && b[r][c + 1].state == rock)return true;
    if (r + 1 <= ROWS && c - 1 > 0 && b[r + 1][c - 1].state == rock)return true;
    if (r + 1 <= ROWS && b[r + 1][c].state == rock)return true;
    if (r + 1 <= ROWS && c + 1 <= COLS && b[r + 1][c + 1].state == rock)return true;
    return false;
}

bool localSearch(Direction map[ROWS][COLS], Direction map2[ROWS][COLS], int r, int c) {
    if (map[r][c] != none)return false;
    if (b[r][c].state == rock)return false;

    if (r - 1 >= 0 && map2[r - 1][c] != none) {
        map[r][c] = up;
        return true;
    }
    if (c + 1 < COLS && map2[r][c + 1] != none) {
        map[r][c] = toRight;
        return true;
    }
    if (r + 1 < ROWS && map2[r + 1][c] != none) {
        map[r][c] = dn;
        return true;
    }
    if (c - 1 >= 0 && map2[r][c - 1] != none) {
        map[r][c] = toLeft;
        return true;
    }

    return false;
}


Direction pathDirFor(int sr, int sc, int er, int ec) {
    Direction map[ROWS][COLS], map2[ROWS][COLS];

    int i, j;
    bool changes = true;

    for (i = 0; i < ROWS; ++i) {
        for (j = 0; j < COLS; ++j) {
            map[i][j] = none;
            map2[i][j] = none;
        }
    }
    map[er][ec] = toRight;
    map2[er][ec] = toRight;


    while (changes) {
        changes = false;

        for (i = 0; i < ROWS; ++i) {
            for (j = 0; j < COLS; ++j) {
                if (localSearch(map, map2, i, j))changes = true;
            }
        }
        for (i = 0; i < ROWS; ++i) {
            for (j = 0; j < COLS; ++j) {
                map2[i][j] = map[i][j];
            }
        }
    }
    return map[sr][sc];
}


float getDist(int r, int c, int tr, int tc) {
    return sqrt((double) (tr - r) * (tr - r) + (tc - c) * (tc - c));
}

Location getNearestEnemyCrown(Unit *u[], int m) {
    int r, c;
    int tr, tc;
    string tla;
    int i;
    float minDist = ROWS * COLS;
    Location l;
    int mini = 0;
    r = u[m]->getR();
    c = u[m]->getC();
    tla = u[m]->getTla();
    for (i = 0; i < NUM; ++i) {
        if (i != m && u[i] && !u[i]->getDead() && u[i]->getTla() != tla && u[i]->getRank() == crown) {
            tr = u[i]->getR();
            tc = u[i]->getC();
            if (getDist(r, c, tr, tc) < minDist) {
                mini = i;
                minDist = getDist(r, c, tr, tc);
            }
        }
    }
    l.row = u[mini]->getR();
    l.col = u[mini]->getC();
    l.direction = pathDirFor(r, c, l.row, l.col);
    return l;
}

Location getNearestEnemy(Unit *u[], int m) {
    int r, c;
    int tr, tc;
    string tla;
    int i;
    float minDist = ROWS * COLS;
    Location l;
    int mini = 0;
    r = u[m]->getR();
    c = u[m]->getC();
    tla = u[m]->getTla();
    for (i = 0; i < NUM; ++i) {
        if (i != m && u[i] && !u[i]->getDead() && u[i]->getTla() != tla) {
            tr = u[i]->getR();
            tc = u[i]->getC();
            if (getDist(r, c, tr, tc) < minDist) {
                mini = i;
                minDist = getDist(r, c, tr, tc);
            }
        }
    }
    l.row = u[mini]->getR();
    l.col = u[mini]->getC();
    l.direction = pathDirFor(r, c, l.row, l.col);
    return l;

}

Direction manhattenDirFor(Unit *u, int tr, int tc) {
    int dr, dc;
    dr = tr - (u->getR());
    dc = tc - (u->getC());
    if (abs(dr) > abs(dc)) {
        if (dr >= 0)return dn;
        else return up;
    } else {
        if (dc >= 0)return toRight;
        else return toLeft;
    }
}

Direction anyDir(Unit *u) {
    int r, c;
    int tr, tc;
    r = u->getR();
    c = u->getC();
    tr = r - 1;
    tc = c;
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS && b[tr][tc].state == empty)return up;
    tr = r + 1;
    tc = c;
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS && b[tr][tc].state == empty)return dn;
    tr = r;
    tc = c - 1;
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS && b[tr][tc].state == empty)return toLeft;
    tr = r;
    tc = c + 1;
    if (tr >= 0 && tr < ROWS && tc >= 0 && tc < COLS && b[tr][tc].state == empty)return toRight;
    return none;
}


SitRep makeSitRep(Unit *u[NUM], int m) {
    if (!u[m])exit(5);
    SitRep s;
    int i, j;
    for (i = 0; i < ROWS; ++i)for (j = 0; j < COLS; ++j)s.board[i][j] = b[i][j];
    s.nearestEnemyCrown = getNearestEnemyCrown(u, m);
    s.nearestEnemy = getNearestEnemy(u, m);
    s.anyOpenSpace = anyDir(u[m]);
    s.turnNumber = numTurns;
    return s;
}

int totalHp(Unit *u[], int m) {
    int i, sum = 0;
    for (i = 0; i < NUM; ++i)if (u[i] && u[i]->getTla() == tla[m]) sum += u[i]->getHp();
    return sum;
}

void display(Unit *u[]) {
    int i, j;
    int lsize;
    int rsize;
    if (ANSI)lsize = (numplayers * 3) + ((numplayers - 1) * 5);
    else lsize = 29;
    rsize = (COLS - lsize) / 2;
    if (!TINYMAP) { rsize = (COLS * 2 - lsize) / 2; }

    // don't display anything if we are doing the rankings
    if (AUTOTOURNEY)return;

    for (i = 0; i < NUM; ++i)if (u[i])unit2thing(u[i], u[i]->getR(), u[i]->getC());

    // throw a legend out at the top
    sout << "\n";
    sout << "\n";
    color[0] = "\033[0;31m";
    color[1] = "\033[0;35m";
    color[2] = "\033[0;36m";
    color[3] = "\033[0;37m";
    sout << "turn: " << numTurns << "  ";
    if (numTurns > 9)rsize--;
    if (numTurns > 99)rsize--;
    for (i = 0; i < rsize - 9; ++i)sout << " ";
    if (ANSI) for (i = 0; i < numplayers; ++i) sout << color[i] << tla[i] << " " << totalHp(u, i) << "\033[0m     ";
    else sout << tla[0] << " (lowercase) " << totalHp(u, 0) << "   " << tla[1] << " (ALLCAPS) " << totalHp(u, 1);
    sout << "\n ";
    for (i = 0; i < COLS; ++i)sout << "-";
    if (!TINYMAP) for (i = 0; i < COLS; ++i)sout << "-";
    sout << "\n";

    // down to business
    for (i = 0; i < ROWS; ++i) {
        sout << "|";
        for (j = 0; j < COLS; ++j) {
            // if it's open space
            if (b[i][j].state == empty) {
                if (DOTS) sout << ".";
                else sout << " ";
                if (!TINYMAP)sout << " ";
                continue;
            }

            // if it's a rock
            if (b[i][j].state == rock) {
                sout << "X";
                if (!TINYMAP)sout << " ";
                continue;
            }

            //else it's a unit
            //turn on some colors if ANSI set
            if (ANSI) {
                if (b[i][j].owner == tla[0]) sout << "\033[0;31m";
                if (b[i][j].owner == tla[1]) sout << "\033[0;35m";
                if (b[i][j].owner == tla[2]) sout << "\033[0;36m";
                if (b[i][j].owner == tla[3]) sout << "\033[0;37m";
            }

            //spit out the appropirate unit character
            if (ANSI || b[i][j].owner == tla[0]) {
                switch (b[i][j].rank) {
                    case infantry:
                        sout << "i";
                        break;
                    case archer:
                        sout << "a";
                        break;
                    case knight:
                        sout << "k";
                        break;
                    case crown:
                        sout << "*";
                        break;
                    default:
                        sout << "?";
                        break;
                }
            } else {
                switch (b[i][j].rank) {
                    case infantry:
                        sout << "I";
                        break;
                    case archer:
                        sout << "A";
                        break;
                    case knight:
                        sout << "K";
                        break;
                    case crown:
                        sout << "*";
                        break;
                    default:
                        sout << "?";
                        break;
                }
            }

            //indicate direction if there is space
            if (!TINYMAP) {
                switch (b[i][j].facing) {
                    case up:
                        sout << "^";
                        break;
                    case dn:
                        sout << "v";
                        break;
                    case toRight:
                        sout << ">";
                        break;
                    case toLeft:
                        sout << "<";
                        break;
                    default:
                        sout << "?";
                        break;
                }
            }

            //turn off the colors if we set them earlier
            if (ANSI)sout << "\033[0m";
        }
        sout << "|";
        sout << "\n";
    }
    sout << " ";
    for (i = 0; i < COLS; ++i)sout << "-";
    if (!TINYMAP) for (i = 0; i < COLS; ++i)sout << "-";
    sout << "\n";
    return;
}


void readMap() {
    int i, j, rows, cols;
    ifstream fin;
    fin.open(INFILE);
    if (fin.fail()) {
        cout << "error opening file: check to make sure the map file is in the same\n";
        cout << "directory as the executable.  exiting.\n";
        exit(3);
    }
    fin >> rows;
    fin >> cols;
    fin >> numplayers;
    if (rows != ROWS || cols != COLS) {
        cout << "error reading map: ROW/COL mismatch with #define in unit.h.  exiting.\n";
        exit(1);
    }
    if (numplayers > 4) {
        cout << "error reading map: max 4 players (more requested).  exiting.\n";
        exit(2);
    }
    if (NUM % numplayers != 0) {
        cout << "error reading map: total number of units can't be evenly divided.  exiting.\n";
        exit(3);
    }
    for (i = 0; i < numplayers; ++i) {
        fin >> startBox[i].minr;
        fin >> startBox[i].maxr;
        fin >> startBox[i].minc;
        fin >> startBox[i].maxc;
    }

    // max 2 players for non-ANSI terminal
    if (numplayers > 2 && !ANSI)numplayers = 2;

    for (i = 0; i < ROWS; ++i) {
        for (j = 0; j < COLS; ++j) {
            b[i][j].state = empty;
            fin >> c[i][j];
            if (c[i][j] == 'X') {
                b[i][j].facing = none;
                b[i][j].state = rock;
            }
        }
        //cout << endl;
    }
    fin.close();
}


void printTlaList() {
    int i;
    for (i = 0; i < NUMTLAS; ++i) {
        cout << tlalist[i] << " ";
        if (tlalist[i] == "zac")cout << "\n\n";
        if (tlalist[i] == "tra")cout << "\n\n";
        if ((i + 1) % 10 == 0)cout << endl;
    }
    cout << endl;
}


bool checksOut(string tla) {
    int i;
    for (i = 0; i < NUMTLAS; ++i) {
        if (tlalist[i] == tla)return true;
    }
    return false;
}

Unit *newUnit(string tla, Rank rank) {
    int hp;
    switch (rank) {
        case infantry:
            hp = INFANTRYHP;
            break;
        case archer:
            hp = ARCHERHP;
            break;
        case knight:
            hp = KNIGHTHP;
            break;
        case crown:
            hp = CROWNHP;
            break;
    }

    if (tla == "xxx")return new Unit(0, 0, hp, up, rank, false, tla);

    if (tla == "drw")return new drw(0, 0, hp, up, rank, false, tla);
    if (tla == "dww")return new dww(0, 0, hp, up, rank, false, tla);
    if (tla == "smb")return new smb(0, 0, hp, up, rank, false, tla);
    if (tla == "zzz")return new zzz(0, 0, hp, up, rank, false, tla);
    if (tla == "sab")return new sab(0, 0, hp, up, rank, false, tla);


    cout << "error making soldier.  quitting.\n";
    exit(0);
}

void updateB(Unit *u) {
    if (!u)exit(5);
    b[u->getR()][u->getC()].state = unit;
    b[u->getR()][u->getC()].rank = u->getRank();
    b[u->getR()][u->getC()].owner = u->getTla();
    b[u->getR()][u->getC()].facing = u->getDir();
    b[u->getR()][u->getC()].hp = u->getHp();
}


void checkPlacement(Unit *u, Unit t, int minr, int maxr, int minc, int maxc) {
    int tr, tc;
    tr = u->getR();
    tc = u->getC();
    int i;

    if (b[tr][tc].state == rock) {
        cout << "error: " << u->getTla() << " placed unit on a rock. exiting.\n";
        exit(6);
    }
    if (b[tr][tc].state == unit) {
        cout << "error: " << u->getTla() << " placed unit on another unit. exiting.\n";
        exit(6);
    }
    if (tr < minr || tr > maxr || tc < minc || tc > maxc) {
        cout << "error: " << u->getTla() << " placed unit outside of box. exiting.\n";
        cout << tr << " " << tc << endl;
        exit(6);
    }
    if (t.getHp() != u->getHp() || t.getRank() != u->getRank()
        || t.getDead() != u->getDead() || t.getTla() != u->getTla()) {
        cout << "error: " << u->getTla() << " performed illegal unit modification during placement. exiting.\n";
        exit(7);
    }

}


void checkNoMods(Unit *u, Unit t) {
    int tr, tc;

    if (t.getHp() != u->getHp() || t.getDir() != u->getDir() || t.getRank() != u->getRank()
        || t.getDead() != u->getDead() || t.getTla() != u->getTla()
        || t.getR() != u->getR() || t.getC() != u->getC()) {
        cout << "error: " << u->getTla() << " performed illegal unit modification during recommendation. exiting.\n";
        exit(8);
    }

}

void setUpBoard(Unit *u[NUM]) {
    Unit t;
    readMap();
    bool gotNames = false;
    int i;
    int pnum;
    int minr, maxr, minc, maxc;

    for (i = 0; i < NUM; ++i)u[i] = 0;

    if (!AUTOTOURNEY) {
        for (i = 0; i < numplayers; ++i) {
            tryagain:
            cout << "tla" << i << ": ";
            cin >> tla[i];
            if (tla[i] == "?") {
                printTlaList();
                goto tryagain;
            }
            if (!checksOut(tla[i])) {
                cout << "That tla is not in the list.\n";
                cout << "type '?' to see a full list.\n";
                goto tryagain;
            }
        }
    }

    // make all the crowns
    for (i = 0; i < numplayers; ++i) {
        pnum = i % numplayers;
        minr = startBox[pnum].minr;
        maxr = startBox[pnum].maxr;
        minc = startBox[pnum].minc;
        maxc = startBox[pnum].maxc;
        u[i] = newUnit(tla[pnum], crown);
        t = *u[i];
        u[i]->Place(minr, maxr, minc, maxc, makeSitRep(u, i));
        checkPlacement(u[i], t, minr, maxr, minc, maxc);
        updateB(u[i]);
    }
    // make all the knights
    for (; i < (numplayers + NUMKNIGHTS); ++i) {
        pnum = i % numplayers;
        pnum = i % numplayers;
        minr = startBox[pnum].minr;
        maxr = startBox[pnum].maxr;
        minc = startBox[pnum].minc;
        maxc = startBox[pnum].maxc;
        u[i] = newUnit(tla[pnum], knight);
        t = *u[i];
        u[i]->Place(minr, maxr, minc, maxc, makeSitRep(u, i));
        checkPlacement(u[i], t, minr, maxr, minc, maxc);
        updateB(u[i]);
    }
    // make all the archers
    for (; i < (numplayers + NUMKNIGHTS + NUMARCHERS); ++i) {
        pnum = i % numplayers;
        minr = startBox[pnum].minr;
        maxr = startBox[pnum].maxr;
        minc = startBox[pnum].minc;
        maxc = startBox[pnum].maxc;
        u[i] = newUnit(tla[pnum], archer);
        t = *u[i];
        u[i]->Place(minr, maxr, minc, maxc, makeSitRep(u, i));
        checkPlacement(u[i], t, minr, maxr, minc, maxc);
        updateB(u[i]);
    }
    // make all the infantry
    for (; i < NUM; ++i) {
        pnum = i % numplayers;
        minr = startBox[pnum].minr;
        maxr = startBox[pnum].maxr;
        minc = startBox[pnum].minc;
        maxc = startBox[pnum].maxc;
        u[i] = newUnit(tla[pnum], infantry);
        t = *u[i];
        u[i]->Place(minr, maxr, minc, maxc, makeSitRep(u, i));
        checkPlacement(u[i], t, minr, maxr, minc, maxc);
        updateB(u[i]);
    }

    return;
}

//return the number of spaces the unit can move without hitting someting,
//up to dist
int clear(Unit *u[], int m, int dist) {
    if (!u[m])exit(5);
    int i;
    int goDist = 0;
    //initialize x and y to the unit's location
    int tr = u[m]->getR();
    int tc = u[m]->getC();

    for (i = 0; i < dist; ++i) {
        //move tr and tc to be in front of the unit's location
        switch (u[m]->getDir()) {
            case up:
                tr--;
                break;
            case dn:
                tr++;
                break;
            case toLeft:
                tc--;
                break;
            case toRight:
                tc++;
                break;
            case none:
                break;
        }

        // is that space off the board?
        if (tr < 0 || tr >= ROWS || tc < 0 || tc >= COLS)break;

        // is there a something there?
        if (b[tr][tc].state != empty)break;

        //nothing there?  bump the goDist
        ++goDist;
    }

    return goDist;
}

void arrowSuffer(Unit *u[], int m, int ar, int ac, int hits) {
    int sr, sc, er, ec;

    if (!u[m])exit(5);
    int i;
    //initialize x and y to the unit's location
    int tr = u[m]->getR();
    int tc = u[m]->getC();

    //find the box the archer can target
    switch (u[m]->getDir()) {
        case up:
            sr = tr - 3;
            er = tr - 1;
            sc = tc - 1;
            ec = tc + 1;
            break;
        case dn:
            sr = tr + 1;
            er = tr + 3;
            sc = tc - 1;
            ec = tc + 1;
            break;
        case toLeft:
            sr = tr - 1;
            er = tr + 1;
            sc = tc - 3;
            ec = tc - 1;
            break;
        case toRight:
            sr = tr - 1;
            er = tr + 1;
            sc = tc + 1;
            ec = tc + 3;
            break;
        case none:
            return;
    }

    if (sr < 0)sr = 0;
    if (er >= ROWS)er = ROWS - 1;
    if (sc < 0)sc = 0;
    if (ec >= COLS)ec = COLS - 1;

    //return if the target is not in range
    if (ar < sr || ar > er || ac < sc || ac > ec)return;



    //check to see if any living unit occupies space [ar,ac]
    for (i = 0; i < NUM; ++i) {
        if (u[i] && !u[i]->getDead() && u[i]->getR() == ar && u[i]->getC() == ac) {
            //if so, make it suffer
            u[i]->Suffer(hits);
            if (u[i]->getDead()) {
                delete u[i];
                u[i] = 0;
            }
        }
    }

    //nobody there? then just return
    return;
}


//make an attacked unit suffer hits
void makeSuffer(Unit *u[], int m, int hits) {
    if (!u[m])exit(5);
    int i;
    //initialize x and y to the unit's location
    int tr = u[m]->getR();
    int tc = u[m]->getC();

    //move tr and tc to be in front of the unit's location
    switch (u[m]->getDir()) {
        case up:
            tr--;
            break;
        case dn:
            tr++;
            break;
        case toLeft:
            tc--;
            break;
        case toRight:
            tc++;
            break;
        case none:
            break;
    }

    //check to see if any living unit occupies space [x,y]
    for (i = 0; i < NUM; ++i) {
        if (u[i] && !u[i]->getDead() && u[i]->getR() == tr && u[i]->getC() == tc) {
            //if so, make it suffer
            u[i]->Suffer(hits);
            if (u[i]->getDead()) {
                delete u[i];
                u[i] = 0;
            }
        }
    }

    //nobody there? then return false
    return;
}


bool oneLeft(Unit *u[NUM]) {
    int i, j;
    string firstTla;
    string secondTla;
    int maxHp = 0;
    int maxPlayer = 0;
    int numWinners = 0;

    // out of time?
    if (numTurns > MAXTURNS) {
        for (i = 0; i < numplayers; ++i) {
            if (totalHp(u, i) > maxHp) {
                maxHp = totalHp(u, i);
                maxPlayer = i;
            }
        }
        for (i = 0; i < numplayers; ++i) {
            if (totalHp(u, i) == maxHp) {
                if (!AUTOTOURNEY)cout << tla[i] << " has the most armies! (" << totalHp(u, i) << ")\n";
                if (AUTOTOURNEY)winner = tla[i];
                numWinners++;
            }
        }
        if (numWinners > 1) {
            if (!AUTOTOURNEY)cout << "It's a TIE!\n";
            if (AUTOTOURNEY)winner = "tie";
        }
        return true;
    }

    //get the first crown tla
    for (i = 0; i < NUM; ++i) {
        if (u[i] && u[i]->getRank() == crown) {
            firstTla = u[i]->getTla();
            break;
        }
    }
    if (i == NUM) {
        //both kings are dead
        if (!AUTOTOURNEY)cout << "All crowns die! Everyone wins. \n";
        if (AUTOTOURNEY)winner = "nil";
        return true;
    }
    //get the second crown tla
    for (i = 0; i < NUM; ++i) {
        if (u[i] && u[i]->getRank() == crown && u[i]->getTla() != firstTla) {
            secondTla = u[i]->getTla();
            break;
        }
    }
    if (i == NUM) {
        //only one king lives
        if (!AUTOTOURNEY)cout << "The one true crown! " << firstTla << " wins. \n";
        if (AUTOTOURNEY)winner = firstTla;
        return true;
    }

    //get the first tla
    for (i = 0; i < NUM; ++i) {
        if (u[i]) {
            firstTla = u[i]->getTla();
            break;
        }
    }

    //look for a different tla
    for (i = 0; i < NUM; ++i) {
        if (u[i] && u[i]->getTla() != firstTla) {
            return false;
        }
    }

    if (!AUTOTOURNEY)cout << firstTla << " wins!\n";
    //there can be only one
    if (AUTOTOURNEY)winner = firstTla;
    return true;
}


void doTurn(Unit *u[NUM]) {
    Unit tu;
    int m;
    SitRep sitRep;
    Action a;
    int hits;
    int lr, lc;
    int i, j, t, next[NUM];

    //come up witha a randome ordering of pieces for this turn
    for (i = 0; i < NUM; ++i)next[i] = i;
    for (i = NUM - 1; i > 0; --i) {
        j = rand() % i;
        t = next[j];
        next[j] = next[i];
        next[i] = t;
    }

    for (i = 0; i < NUM; ++i) {
        m = next[i];
        if (oneLeft(u))return;;
        if (u[m] && !u[m]->getDead()) {
            makeB(u);
            sitRep = makeSitRep(u, m);
            int tseed = rand();  //Save the current state of rand...
            tu = *u[m];
            a = u[m]->Recommendation(sitRep);
            checkNoMods(u[m], tu);
            srand(tseed);  //...and reset it.
            switch (a.action) {
                case turn:
                    u[m]->Turn(a.dirToFace);
                    break;
                case moveFwd:
                    lr = u[m]->getR();
                    lc = u[m]->getC();
                    if (!clear(u, m, a.maxDist))break;
                    if (u[m]->getRank() == archer || u[m]->getRank() == infantry)
                        a.maxDist = 1;
                    u[m]->Move(clear(u, m, a.maxDist));
                    break;
                case attack:
                    hits = u[m]->Attack();
                    if (u[m]->getRank() == archer)
                        arrowSuffer(u, m, a.archerRow, a.archerCol, hits);
                    else
                        makeSuffer(u, m, hits);
                    break;
                case nothing:
                    break;
            }
        }
    }
    return;
}


//run an automatic game.  tla1 and tla2 should be set up prior to calling
int autoGame(Unit *u[]) {
    bool done = false;

    // set up the board
    setUpBoard(u);

    while (!done) {
        // do a turn
        ++numTurns;
        doTurn(u);
        if (oneLeft(u))done = true;
    }
    if (AUTOTOURNEY) {
        if (winner == tla[0])return 2;
        else if (winner == "tie")return 1;
    }
    return 0;
}

//run a whole tournament of each tla against AUTONUMMATCHES random others, output stats on each
void autoTourney(Unit *u[]) {
    int i, j, k;
    int wins;
    int w;
    for (i = 56; i < NUMTLAS; ++i) {
        if (noPlay[i])continue;
        wins = 0;
        tla[0] = tlalist[i];
        for (j = 0; j < AUTONUMMATCHES; ++j) {
            numTurns = 0;
            //k=rand()%NUMTLAS;
            //while(k==i || noPlay[k])k=rand()%NUMTLAS;
            k = 1;
            tla[1] = tlalist[k];
            w = autoGame(u);
            //cout << "\t"<<tla[0]<<" vs. "<<tla[1]<<": ";
            //cout << "winner: "<<winner<<"  -- ";
            //if(w==2)cout<<"win\n";
            //else if(w==1)cout<<"tie\n";
            //else cout<<"lose\n";
            wins += w;
        }
        cout << i << ": " << tla[0] << " ";
        cout << wins;
        cout << endl;
    }
    return;
}


int main() {


    Unit *u[NUM];
    char line[1024];
    bool done = false;
    numTurns = 0;

    srand(time(NULL));

    // if running an auto tourney, just do that and quit
    if (AUTOTOURNEY) {
        autoTourney(u);
        return 0;
    }

    // set up the board
    setUpBoard(u);
    display(u);
    cin.getline(line, 1023);
    cout << sout.str();
    sout.str("");

    while (!done) {
        // do a turn
        ++numTurns;
        doTurn(u);
        display(u);
        cin.getline(line, 1023);
        cout << sout.str();
        sout.str("");
        if (line[0] == 'q')done = true;
        if (oneLeft(u))done = true;
    }
    return 0;


}
	

 




