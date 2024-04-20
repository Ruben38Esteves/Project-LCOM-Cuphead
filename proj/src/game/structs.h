#ifndef STRUCTS_H_
#define STRUCTS_H_

struct game
{
    state state;
    player player;
    //board board;
    int turn;
};


struct user
{
    char name[20];
    char symbol;
    int score;
};

struct player
{
    int life;
    int damage;
    int x;
    int y;
};
struct bullet
{
    int x;
    int y;
    int damage;
    float xDirection;
    float yDirection;
};

struct enemy
{
    int life;
    int damage;
    int x;
    int y;
};

struct mouseLocation
{
    int x;
    int y;
    int weight;
    int height;
};


/*
struct board
{
    int width;
    int height;
    char **board;
};
*/

#endif