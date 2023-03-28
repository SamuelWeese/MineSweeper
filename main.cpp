#include <iostream>
#include "minesweeper.h"

int main()
{
    const static int windowSizeHeight = 720;
    const static int windowSizeLength = 720;
    sf::RenderWindow window(sf::VideoMode(windowSizeLength, windowSizeHeight), "SFML window");
    minesweeper aGame(&window, 10, 25);
    aGame.createRBoard();
    aGame.printAnswer();
    window.setFramerateLimit(60);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                break;
            case sf::Event::MouseButtonPressed:
                aGame.mouse(event);
                break;
            case sf::Event::KeyPressed:
                aGame.keys(event);
                break;
            }

            if (event.type == sf::Event::Closed)
                window.close();

        }

        window.clear();

        aGame.draw();
        window.display();
    }

    return 0;
}
