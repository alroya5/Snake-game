//
// Created by smile on 14/01/2023.
//

#include "Snake.hpp"
#include "Game_Scene.hpp"

namespace Snake
{

    void Snake:: changeDirection(std::string newDirection)
    {
        direction = newDirection;
    }
    void Snake::increaseLength(int suma)
    {
        length+=suma;
    }
    float Snake::getSnakePositionX()
    {

        return x;

    }
    float Snake::getSnakePositionY()
    {

        return y;

    }
    float Snake::getSnakeSpeed() {
        return  speed;
    }
    int Snake::getSnakeLength() {
        return  length;
    }

    void Snake::move(example::Sprite *snake)
    {
        //dependiendo del valor de direction se cambia la velocidad del sprite
        if (direction=="UP")
        {
            snake->set_speed({0.f,300.f});

        }
        else if(direction=="DOWN")
        {
            snake->set_speed({0.f,-300.f});

        }
        else if(direction=="RIGHT")
        {
            snake->set_speed({300.f,0.f});
        }
        else if(direction=="LEFT")
        {
            snake->set_speed({-300.f,0.f});
        }

    }


}