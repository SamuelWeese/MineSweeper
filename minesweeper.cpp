#include "minesweeper.h"
#include <cassert>
#include <random>
#include <cstring>
#include <SFML/Graphics.hpp>

#define pixelWidth 16
#define cleanTile 1*pixelWidth,0*pixelWidth,2*pixelWidth,1*pixelWidth
#define emptyTile 2*pixelWidth,0*pixelWidth,1*pixelWidth,1*pixelWidth
//#define flagTile 1*pixelWidth, 1*pixelWidth, 2*pixelWidth,2*pixelWidth
#define badFlag 1*pixelWidth, 2*pixelWidth, 2*pixelWidth,3*pixelWidth
#define flagTile 1*pixelWidth, 2*pixelWidth, 2*pixelWidth,3*pixelWidth
bool safetyCheck(long value, long min, long max)
{
    if (value >= max) return false;
    if (value < min) return false;
    return true;
}

minesweeper::minesweeper(sf::RenderWindow *aWindow, u_int size, u_int bombs)
{
    this->playing = true;
    this->size = size;
    this->bombs = bombs;
    if (size*size < bombs)
        this->bombs = size * size;

    this->padding = 0;
    this->boardState = std::vector<std::vector<u_int>>(size, std::vector<u_int>(size, 0));

    this->boardVisible = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
    srand(999);


    this->window = aWindow;

    if (!spriteSheet.loadFromFile("/tmp/sprite_sheet2.png"))
    {
        throw std::runtime_error("Bad tile sprite texture path.");
    }
    // REDO to show multiple solving at same time
    auto windowDimension = window->getSize();
    float scaleX, scaleY; // windowSize - padding * 2 is to ensure tile only in the "play space"
    scaleX = (float)windowDimension.x/(float)(pixelWidth*size) - padding*2;
    scaleY = (float)windowDimension.y/(float)(pixelWidth*size) - padding*2;
    int positionX, positionY;
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        this->spriteVector.push_back(std::vector<sf::Sprite>() );
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            this->spriteVector[x_iter].push_back(sf::Sprite());
            positionX = (windowDimension.x -(padding*2)) / size * x_iter + padding;
            positionY = (windowDimension.y -(padding*2)) / size * y_iter + padding;

            spriteVector[x_iter][y_iter].setPosition(positionX, positionY);
            spriteVector[x_iter][y_iter].scale(scaleX, scaleY);
            spriteVector[x_iter][y_iter].setTexture(this->spriteSheet);
            spriteVector[x_iter][y_iter].setTextureRect(sf::IntRect(cleanTile));

        }
    }
}

minesweeper::~minesweeper()
{
}

void minesweeper::createRBoard(int seed)
{
    this->seedRBoard(seed);
    this->numberBoard();
}

void minesweeper::seedRBoard(int seed)
{
    if (seed)
        srand(seed);
    u_int x, y;
    u_int currentBombs = 0;
    while (currentBombs < bombs)
    {
        x = rand()%(size);
        y = rand()%(size);
        if (this->boardState[x][y] == bomb)
            continue;
        this->boardState[x][y] = bomb;
        currentBombs++;

    }
}

void minesweeper::numberBoard()
{
    u_int index_X, index_Y;
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            if (this->boardState[x_iter][y_iter] != bomb)
                continue;
            for (int x_iter_iter = -1; x_iter_iter <= 1; x_iter_iter++)
            {
                index_X = x_iter_iter + x_iter;
                if(!safetyCheck(index_X, 0, size))
                    continue;
                for (int y_iter_iter = -1; y_iter_iter <= 1; y_iter_iter++)
                {
                    if(x_iter_iter == 0 && y_iter_iter == 0)
                        continue;
                    index_Y = y_iter_iter + y_iter;
                    if(!safetyCheck(index_Y, 0, size))
                        continue;
                    if (this->boardState[index_X][index_Y] == bomb)
                        continue;
                    boardState[index_X][index_Y]++;
                }
            }
        }
    }
}

void minesweeper::activateTile(u_int x, u_int y)
{
    if(!playing) return;
    if(!safetyCheck(x, 0, size))
        return;
    if(!safetyCheck(y, 0, size))
        return;
    coordPair Pair;
    Pair.x = x;
    Pair.y = y;
    for (auto pair_iter : this->flags)
    {
        if (pair_iter.x == x && pair_iter.y == y)
            return;
    }
    u_int value = boardState[x][y];
    u_int xCoord, yCoord; // refer to graphics coords on the sprite sheet
    switch(value)
    {
    case 0:
        groupShow(x,y);
        xCoord = 1 * pixelWidth;
        yCoord = 5 * pixelWidth;
        spriteVector[x][y].setTextureRect(sf::IntRect(emptyTile));
        return;
    case bomb:
        boardVisible[x][y] = true;
        this->endGame(false);
        xCoord = 1 * pixelWidth;
        yCoord = 4 * pixelWidth;
        break;
    case 1:
        xCoord = 0 * pixelWidth;
        yCoord = 0 * pixelWidth;
        break;
    case 2:
        xCoord = 0 * pixelWidth;
        yCoord = 1 * pixelWidth;
        break;
    case 3:
        xCoord = 0 * pixelWidth;
        yCoord = 2 * pixelWidth;
        break;
    case 4:
        xCoord = 0 * pixelWidth;
        yCoord = 3 * pixelWidth;
        break;
    case 5:
        xCoord = 0 * pixelWidth;
        yCoord = 4 * pixelWidth;
        break;
    case 6:
        xCoord = 0 * pixelWidth;
        yCoord = 5 * pixelWidth;
        break;
    case 7:
        xCoord = 0;
        yCoord = 6 * pixelWidth;
        break;
    case 8:
        xCoord = 0;
        yCoord = 7 * pixelWidth;
        break;
    default:
        boardVisible[x][y] = true;
    }
    spriteVector[x][y].setTextureRect(sf::IntRect(xCoord, yCoord, xCoord+1 * pixelWidth, yCoord+1 * pixelWidth));
    boardVisible[x][y] = true;
}

void minesweeper::groupShow(u_int x, u_int y)
{
    if(!safetyCheck(x, 0, size))
        return;
    if(!safetyCheck(y, 0, size))
        return;

    if (this->boardVisible[x][y] != 0)
        return;
    this->boardVisible[x][y] = true;

    u_int xIndex, yIndex;
    for (short x_iter = -1; x_iter <= 1; x_iter++)
    {
        for (short y_iter = -1; y_iter <= 1; y_iter++)
        {
            xIndex = x+x_iter;
            yIndex = y+y_iter;
            if(!safetyCheck(xIndex, 0, size))
                continue;
            if(!safetyCheck(yIndex, 0, size))
                continue;
            activateTile(xIndex, yIndex);
        }
    }

}

void minesweeper::draw()
{
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            window->draw(spriteVector[x_iter][y_iter]);
            // Something to draw UI over the top
        }
    }
}


void minesweeper::endGame(bool winner)
{
    playing = false;
    //this->showEnd(winner);
}

#include <iostream>
void minesweeper::print()
{
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            if(!safetyCheck(x_iter, 0, size))
                continue;
            if(!safetyCheck(y_iter, 0, size))
                continue;
            if (!boardVisible[x_iter][y_iter])
                std::cout << " ";
            if (boardVisible[x_iter][y_iter])
                std::cout << boardState[x_iter][y_iter];

        }
        std::cout << '\n';
    }
}

void minesweeper::printAnswer()
{
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            if(!safetyCheck(x_iter, 0, size))
                continue;
            if(!safetyCheck(y_iter, 0, size))
                continue;
            std::cout << boardState[x_iter][y_iter];

        }
        std::cout << '\n';
    }
}

void minesweeper::mouse(sf::Event anEvent)
{
    u_int x = anEvent.mouseButton.x;
    u_int y = anEvent.mouseButton.y;
    auto windowSize = this->window->getSize();
    u_int tileLength = ((windowSize.x - this->padding*2) / size);
    u_int tileHeight = ((windowSize.y - this->padding*2) / size);
    u_int x_iter = x / tileLength;
    u_int y_iter = y / tileHeight;
    std::cout << "x:" <<x_iter << '\t' << "y:" << y_iter << '\t' << "Val:" << boardState[x_iter][y_iter] << '\n';
    if (anEvent.mouseButton.button == sf::Mouse::Right)
        return this->flag(x_iter, y_iter);
    if (anEvent.mouseButton.button == sf::Mouse::Left)
        return this->activateTile(x_iter, y_iter);

}

#include <chrono>
void minesweeper::keys(sf::Event anEvent)
{
    auto key = anEvent.key.code;
    switch (key) {
    case (sf::Keyboard::R):
        this->resetBoardState();
        this->createRBoard();
        break;
    }
    return;
}

void minesweeper::cleanBoardVisuals()
{
    for (u_int x_iter = 0; x_iter < size; x_iter++)
    {
        for (u_int y_iter = 0; y_iter < size; y_iter++)
        {
            spriteVector[x_iter][y_iter].setTextureRect(sf::IntRect(cleanTile));
        }
    }
}

void minesweeper::resetBoardState()
{
    this->playing = true;
    this->boardState = std::vector<std::vector<u_int>>(size, std::vector<u_int>(size, 0));
    this->boardVisible = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
    this->cleanBoardVisuals();
    this->cleanBoardFlags();
}


void minesweeper::cleanBoardFlags()
{
    this->flags = std::vector<coordPair>{};
}
void minesweeper::flag(u_int x, u_int y)
{
    if(!safetyCheck(x, 0, size))
        return;
    if(!safetyCheck(y, 0, size))
        return;
    if (boardVisible[x][y])
        return;
    // TODO FIX
    coordPair Pair;
    Pair.x = x;
    Pair.y = y;
    long aPair = 0;
    for (auto pair_iter : this->flags)
    {
        if (pair_iter.x == x && pair_iter.y == y)
        {
            this->spriteVector[x][y].setTextureRect(sf::IntRect(cleanTile));
            this->flags.erase(flags.begin()+aPair);
            return;
        }
        aPair++;
    }
    this->flags.push_back(Pair);
    spriteVector[x][y].setTextureRect(sf::IntRect(flagTile));
}
// TODO with professor interview live
// implement his solution, not any old one
void minesweeper::solve()
{

}
void minesweeper::solveGraphically()
{

}
void minesweeper::playEndAnimation()
{
    for (u_long iter_I = 0; iter_I < this->boardState.size(); iter_I++)
    {
        for (u_long iter_J = 0; iter_J < this->boardState[iter_I].size(); iter_J++)
        {
            for (auto pair_iter : this->flags)
            {

            }
        }
    }
    for (auto pair_iter : this->flags)
    {
        if (boardState[pair_iter.x][pair_iter.y] != 9)
        {
            popReveal(pair_iter, badFlag);
        }
    }
}
// TODO animation
void minesweeper::popReveal(coordPair aPair, int a, int b, int c, int d)
{
    return;
}
/*
bool minesweeper::checkSolution(std::vector<std::vector<int>>)
{

}
std::vector<std::vector<u_short>> minesweeper::generateBoardState()
{
    std::vector<std::vector<u_short>> retArr = this->boardState;
    for (int x = 0; x < this->boardState.length(); x++)
    {
        for (int y = 0; y < this->boardState[x].length(); y++)
        {
            if (!boardVisible[x][y])
            {
                retArr[x][y] = 10;
            }
        }
    }
}
*/
