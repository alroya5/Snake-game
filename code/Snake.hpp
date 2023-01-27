//
// Created by smile on 14/01/2023.
//

#ifndef BASICS_PONG_EXAMPLE_SNAKE_HPP
#define BASICS_PONG_EXAMPLE_SNAKE_HPP

#include <basics/Canvas>
#include <basics/Texture_2D>
#include <basics/Vector>

#include <basics/Scene>
#include "Sprite.hpp"


namespace Snake{
    using basics::Point2f;
    class Snake {
        private:
            //cabeza serpiente
            float x,y;

            //velocidad serpiente
            float speed;

            //dirección
            std::string direction;

            //longitud de la serpiente
            int length;




        public:
            Snake(){
                x = 585;
                y = 500;
                direction = "RIGHT";
                length = 1;
                speed = 200.f;


            }
            //mover la serpiente (el cuerpo)
            void move(example::Sprite *snake) ;
            //cambiar la dirección de la serpiente
            void changeDirection(std::string direction);

            //obtiene la posicion de la serpiente horizontalmente
            float getSnakePositionX();
            //obtiene la posición de la serpiente verticalmente
            float getSnakePositionY();
            //obtiene la velocidad de la serpiente
            float getSnakeSpeed();
            //saca la dirección de la serpiente
            char getDirection();
            //saca la longitud de la serpiente
            int getSnakeLength();

            //aumentar tamaño de la serpiente (al comer)
            void increaseLength(int suma);
            //Revisar las colisiones(muerte)
            bool checkCollision();





        };

}
#endif //BASICS_PONG_EXAMPLE_SNAKE_HPP


