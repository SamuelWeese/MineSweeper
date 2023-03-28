#ifndef MINESWEEPER_H
#define MINESWEEPER_H
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#define bomb 9

struct coordPair
{
    u_int x;
    u_int y;
};



class minesweeper
{
    std::vector<std::vector<u_int>>boardState;
    std::vector<std::vector<bool>> boardVisible;
    u_int size;
    u_int bombs;
    std::vector<coordPair> flags;
    bool playing;

    u_int padding;

    sf::RenderWindow *window;
    sf::Texture spriteSheet;
    std::vector<std::vector<sf::Sprite>> spriteVector;
public:
    minesweeper(sf::RenderWindow *aWindow, u_int size = 10, u_int bombs = 10);
    ~minesweeper();
    void createRBoard(int seed = 0);
    void seedRBoard(int seed = 0);
    void numberBoard();


    void solve();
    void solveGraphically();
    bool checkSolution(std::vector<std::vector<u_short>>);
    std::vector<std::vector<u_short>> generateBoardState();


    void draw();
    void print();
    void printAnswer();

    void mouse(sf::Event);
    void keys(sf::Event );

    void cleanBoardVisuals();
    void resetBoardState();
    void cleanBoardFlags();

    void groupShow(u_int x, u_int y);
    void activateTile(u_int x, u_int y);
    void flag(u_int x, u_int y);


    void endGame(bool winner);

};

#endif // MINESWEEPER_H
