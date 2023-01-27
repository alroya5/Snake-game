/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Game_Scene.hpp"
#include "Snake.hpp"
#include "Intro_Scene.hpp"
#include "Menu_Scene.hpp"

#include <cstdlib>
#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace example
{
    // ---------------------------------------------------------------------------------------------
    // ID y ruta de las texturas que se deben cargar para esta escena. La textura con el mensaje de
    // carga está la primera para poder dibujarla cuanto antes:

    Game_Scene::Texture_Data Game_Scene::textures_data[] =
    {
        { ID(loading),     "game-scene/loading.png"        },
        { ID(hbar),        "game-scene/horizontal-bar.png" },
//        { ID(player-bar),  "game-scene/players-bar.png"    },
        { ID(ball),        "game-scene/ball.png"           },
        { ID(gamePad),     "game-scene/arrows.png"         },
        { ID(snake_head),  "game-scene/SnakeHead.png"      },
        { ID(snake_bod),  "game-scene/SnakeBody.png"      },
        { ID(board_tile),  "game-scene/GameTile.png"       },
        { ID(board_border),"game-scene/GameBorder.png"     },
        { ID(food),       "game-scene/Food.png"           },
        { ID(Up),         "game-scene/Up.png"             },
        { ID(Down),       "game-scene/Down.png"           },
        { ID(Left),       "game-scene/Left.png"           },
        { ID(Right),      "game-scene/Right.png"          },
        { ID(LeftBorder), "game-scene/vertical-border.png"},
        { ID(RightBorder), "game-scene/vertical-border.png"},
        { ID(PauseButton), "game-scene/pause-icon.png"},
        { ID(HelpTexture), "menu-scene/HowToSnake.png"},


    };

    // Pâra determinar el número de items en el array textures_data, se divide el tamaño en bytes
    // del array completo entre el tamaño en bytes de un item:

    unsigned Game_Scene::textures_count = sizeof(textures_data) / sizeof(Texture_Data);

    // ---------------------------------------------------------------------------------------------
    // Definiciones de los atributos estáticos de la clase:


    //crea una nueva serpiente
    Snake::Snake snake;

    // ---------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        // Se establece la resolución virtual (independiente de la resolución virtual del dispositivo).
        // En este caso no se hace ajuste de aspect ratio, por lo que puede haber distorsión cuando
        // el aspect ratio real de la pantalla del dispositivo es distinto.

        canvas_width  = 1280;
        canvas_height =  720;

        // Se inicia la semilla del generador de números aleatorios:

        srand (unsigned(time(nullptr)));

        // Se inicializan otros atributos:

        initialize ();
    }

    // ---------------------------------------------------------------------------------------------
    // Algunos atributos se inicializan en este método en lugar de hacerlo en el constructor porque
    // este método puede ser llamado más veces para restablecer el estado de la escena y el constructor
    // solo se invoca una vez.

    bool Game_Scene::initialize ()
    {
        state     = LOADING;
        suspended = true;
        gameplay  = UNINITIALIZED;

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        suspended = true;               // Se marca que la escena ha pasado a primer plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::resume ()
    {
        suspended = false;              // Se marca que la escena ha pasado a segundo plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)               // Se descartan los eventos cuando la escena está LOADING
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing ();           // Se empieza a jugar cuando el usuario toca la pantalla por primera vez
            }
            else switch (event.id)
            {
                case ID(touch-started):
                case ID(touch-moving):
                {
                    //se guarda la localización y la id del toque del jugador
                    touch_location =
                            {
                                    *event[ID(x)].as< var::Float > (),
                                    *event[ID(y)].as< var::Float > ()
                            };


                        //si toca en alguno de los sprites de flechas se cambia la dirección de la serpiente
                        if (Up->contains (touch_location))
                        {
                            snake.changeDirection("UP");
                            snake_head->set_speed({0.f,snake.getSnakeSpeed()});


                            break;
                        }
                        else if(Down->contains (touch_location))
                        {
                            snake.changeDirection("DOWN");
                            snake_head->set_speed({0.f,-snake.getSnakeSpeed()});


                            break;
                        }
                        else if(Left->contains (touch_location))
                        {
                            snake.changeDirection("LEFT");
                            snake_head->set_speed({-snake.getSnakeSpeed(),0.f});

                            break;
                        }
                        else if(Right->contains (touch_location))
                        {
                            snake.changeDirection("RIGHT");
                            snake_head->set_speed({snake.getSnakeSpeed(),0.f});

                            break;
                        }
                        //si se toca el botón de pausa se pausará el juego
                        else if(Pause->contains(touch_location))
                        {
                            pauseGame();

                        }


                }

                case ID(touch-ended):       // El usuario deja de tocar la pantalla
                {

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if(!pause){
            if (!suspended) switch (state)
                {
                    case LOADING: load_textures  ();     break;
                    case RUNNING: run_simulation (time); break;
                    case ERROR:   break;
                }
            if (!font)
            {
                Graphics_Context::Accessor context = director.lock_graphics_context ();

                if (context)
                {
                    font.reset (new Raster_Font("fonts/impact.fnt", context));
                }
                //se renderizan los sprites del cuerpo de la serpiente
                for(auto & sprite : snakeBody)
                {
                    sprite.render(*CanvasPointer);

                }
            }
        }
        else{

            return;
        }

    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render (Context & context)
    {
        if (!suspended)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            CanvasPointer=canvas;

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {

                //Text_Layout points()
                canvas->clear ();
                //meter texto
                switch (state)
                {
                    case LOADING: render_loading   (*canvas); break;
                    case RUNNING: render_playfield (*canvas); break;
                    case ERROR:   break;
                }


            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    // En este método solo se carga una textura por fotograma para poder pausar la carga si el
    // juego pasa a segundo plano inesperadamente. Otro aspecto interesante es que la carga no
    // comienza hasta que la escena se inicia para así tener la posibilidad de mostrar al usuario
    // que la carga está en curso en lugar de tener una pantalla en negro que no responde durante
    // un tiempo.

    void Game_Scene::load_textures ()
    {
        if (textures.size () < textures_count)          // Si quedan texturas por cargar...
        {
            // Las texturas se cargan y se suben al contexto gráfico, por lo que es necesario disponer
            // de uno:

            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                // Se carga la siguiente textura (textures.size() indica cuántas llevamos cargadas):

                Texture_Data   & texture_data = textures_data[textures.size ()];
                Texture_Handle & texture      = textures[texture_data.id] = Texture_2D::create (texture_data.id, context, texture_data.path);

                // Se comprueba si la textura se ha podido cargar correctamente:

                if (texture) context->add (texture); else state = ERROR;

                // Cuando se han terminado de cargar todas las texturas se pueden crear los sprites que
                // las usarán e iniciar el juego:
            }
        }
        else
        if (timer.get_elapsed_seconds () > 1.f)         // Si las texturas se han cargado muy rápido
        {                                               // se espera un segundo desde el inicio de
            create_sprites ();                          // la carga antes de pasar al juego para que
            restart_game   ();                          // el mensaje de carga no aparezca y desaparezca
                                                        // demasiado rápido.
            state = RUNNING;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::create_sprites ()
    {

        //se crean los sprites del hud, el botón de pausa y las flechas
        Sprite_Handle up_handle(new Sprite( textures[ID(Up)].get () ));
        Sprite_Handle left_handle(new Sprite( textures[ID(Left)].get () ));
        Sprite_Handle down_handle(new Sprite( textures[ID(Down)].get () ));
        Sprite_Handle right_handle(new Sprite( textures[ID(Right)].get () ));
        Sprite_Handle pause_handle(new Sprite(textures[ID(PauseButton)].get()));





        // Se crean y configuran los sprites del fondo:

        Sprite_Handle    top_bar(new Sprite( textures[ID(hbar)].get () ));

        Sprite_Handle bottom_bar(new Sprite( textures[ID(hbar)].get () ));
        Sprite_Handle leftBorder(new Sprite(textures[ID(LeftBorder)].get()));
        Sprite_Handle rightBorder(new Sprite(textures[ID(RightBorder)].get()));
        Sprite_Handle boardBorder(new Sprite( textures[ID(board_border)].get () ));
        Sprite_Handle boardTile(new Sprite( textures[ID(board_tile)].get () ));

        //se posicionan los sprites en su debido lugar
           top_bar->set_anchor   (TOP | LEFT);
           top_bar->set_position ({ 0, canvas_height });
           leftBorder->set_anchor   ( LEFT);
           leftBorder->set_position ({ 0, canvas_height/2 });
           rightBorder->set_anchor   ( RIGHT);
           rightBorder->set_position ({ canvas_width/2, canvas_height/2 });

        bottom_bar->set_anchor   (BOTTOM | LEFT);
        bottom_bar->set_position ({ 0, 0 });
        up_handle->set_anchor(BOTTOM|RIGHT);
        up_handle->set_position({canvas_width/2.f+405,canvas_height/2.f-100});

        down_handle->set_anchor(BOTTOM|RIGHT);
        down_handle->set_position({canvas_width/2.f+405,canvas_height/2.f-300});

        left_handle->set_anchor(BOTTOM|RIGHT);
        left_handle->set_position({canvas_width/2.f+200,canvas_height/2.f-300});

        right_handle->set_anchor(BOTTOM|RIGHT);
        right_handle->set_position({canvas_width/2.f+600,canvas_height/2.f-300});

        pause_handle->set_anchor(TOP|LEFT);
        pause_handle->set_position({50,canvas_height-100});


        //se guardan los sprites en el vector de sprites
        sprites.push_back (   top_bar);
        sprites.push_back (bottom_bar);
        sprites.push_back(boardTile);
        sprites.push_back(boardBorder);
        sprites.push_back(up_handle);
        sprites.push_back(down_handle);
        sprites.push_back(left_handle);
        sprites.push_back(right_handle);
        sprites.push_back(rightBorder);
        sprites.push_back(leftBorder);
        sprites.push_back(pause_handle);


        //se crean
        Sprite_Handle         ball_handle(new Sprite( textures[ID(ball)      ].get () ));
        Sprite_Handle         head_handle(new Sprite( textures[ID(snake_head)].get () ));
        Sprite_Handle         food_handle(new Sprite( textures[ID(food)].get () ));

        //se guardan los sprites en el vector de sprites
        sprites.push_back (        ball_handle);
        sprites.push_back (        head_handle);
        sprites.push_back (        food_handle);

        // Se guardan punteros a los sprites que se van a usar frecuentemente:

        top_border    =             top_bar.get ();
        bottom_border =          bottom_bar.get ();
        snake_head    =          head_handle.get();
        food          =          food_handle.get();
        Up            =            up_handle.get();
        Down          =          down_handle.get();
        Left          =          left_handle.get();
        Right         =         right_handle.get();
        sideL_border = leftBorder.get();
        sideR_border = rightBorder.get();
        Pause=pause_handle.get();




    }

    // ---------------------------------------------------------------------------------------------
    // Juando el juego se inicia por primera vez o cuando se reinicia porque el jugador pierde, se
    // llama a este método para restablecer la posición y velocidad de los sprites:

    void Game_Scene::restart_game()
    {
                foodSpawn();
                snake_head->set_position({canvas_width / 4.f, canvas_height / 2.f});
                snake_head->set_speed    ({ 0.f, 0.f });



        gameplay = WAITING_TO_START;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {

        // Se actualiza el estado de todos los sprites:

        for (auto & sprite : sprites)
        {
            sprite->update (time);
        }


        update_user ();



    }


    void Game_Scene::update_user ()
    {
        if (snake_head->intersects (*top_border))
        {
            // Si el player está tocando el borde superior, se muere:
            Pause->hide();
            snake_head->set_position_y (top_border->get_bottom_y () - snake_head->get_height () / 2.f);
            snake_head->set_speed_y (0);
            renderGamover();
        }
        else
        if (snake_head->intersects (*bottom_border))
        {
            // Si el player está tocando el borde inferior, se muere:
            Pause->hide();
            snake_head->set_position_y (bottom_border->get_top_y () + snake_head->get_height () / 2.f);
            snake_head->set_speed_y (0);
            renderGamover();
        }
        else
        if (snake_head->intersects (*sideL_border))
        {
            // Si el player está tocando el borde izquierdo, se muere:
            Pause->hide();
            snake_head->set_position_x (sideL_border->get_right_x() + snake_head->get_width() / 2.f);
            snake_head->set_speed_x (0);
            renderGamover();
        }
        else
        if (snake_head->intersects (*sideR_border))
        {
            // Si el player está tocando el borde derecho, se muere:
            Pause->hide();
            snake_head->set_position_x (sideR_border->get_left_x() - snake_head->get_width() / 2.f);
            snake_head->set_speed_x (0);
            renderGamover();
        }
        else

        if (snake_head->intersects (*food))
        {
            //si el jugador toca la comida se respawnea la misma, se añade uno al marcador y se llama a la función de crecer
            foodSpawn();
            snake.increaseLength(1);
            growSnake();
        }



    }



    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render_loading (Canvas & canvas)
    {
        Texture_2D * loading_texture = textures[ID(loading)].get ();


        if (loading_texture)
        {

            canvas.fill_rectangle
            (
                //se renderiza el canvas
                { canvas_width * .5f, canvas_height * .5f },
                { loading_texture->get_width (), loading_texture->get_height () },
                  loading_texture
            );
        }
    }

    // ---------------------------------------------------------------------------------------------
    // Simplemente se dibujan todos los sprites que conforman la escena.

    void Game_Scene::render_playfield (Canvas & canvas)
    {
        for (auto & sprite : sprites)
        {
            sprite->render (canvas);


        }
        if (font)
        {
            // Se dibujan textos con diferentes puntos de anclaje a partir de una cadena simple:

            Text_Layout sample_text(*font, L"Score: " +  to_wstring(snake.getSnakeLength()));



            canvas.draw_text ({ canvas_width-canvas_width/4, canvas_height-canvas_height/8 }, sample_text,TOP | RIGHT);



        }
    }

    void Game_Scene::foodSpawn()
    {
        //se instancian las posiciones iniciales y finales
        int posIni=50;
        int posFin=450;
        Point2f foodPosition;
        int x_food,y_food;
        //se randomizan
        x_food = (rand()%(posFin-posIni)+posIni);
        y_food = (rand()%(posFin-posIni)+posIni);
        //si tienen estos parámetros se vuelve a instanciar
        if(y_food<80){
            foodSpawn();
        }
        if(x_food<100){
            foodSpawn();
        }
        //se setea la posición
        foodPosition={x_food,y_food};
        food->set_position(foodPosition);


    }

    void Game_Scene::growSnake()
    {

        //si el vector del cuerpo es menor que 0 se comprueba la posición de la cabeza
        if(snakeBody.size()<=0)
        {
            //se consigue la textura
            Texture_2D *bodyTexture=(textures[ID(snake_bod)].get ());

                //se selecciona la posición
                Sprite newSegment(bodyTexture);
                newSegment.set_position(snake_head->get_position());
                //se pone al final del vector
                snakeBody.push_back(newSegment);



        }
        //sino se comprueba el último segmento del cuerpo
        else{

            float lastX=snakeBody[snakeBody.size()-1].get_position_x();
            float lastY=snakeBody[snakeBody.size()-1].get_position_y();
            Texture_2D *bodyTexture=(textures[ID(snake_bod)].get ());
                Sprite newSegment(bodyTexture);
                newSegment.set_position({lastX+100,lastY});

                snakeBody.push_back(newSegment);



        }



    }
    void Game_Scene::pauseGame()
    {
        if(pause){
            pause = false;
        }
        else{
            //se para el update, se saca el texto de pausa y se para la cabeza de la serpiente
            Text_Layout pause_text(*font, L"PAUSED");
            CanvasPointer->draw_text ({ canvas_width/4, canvas_height/2 }, pause_text,LEFT);
            pause = true;


        }

    }

    void Game_Scene::renderGamover()
    {

        //se para la cabeza de la serpiente
        snake_head->set_speed_x(0);
        snake_head->set_speed_y(0);
        //pause= true;
        //se saca el texto de gameover
        Text_Layout dieText(*font,L"GAME OVER");
        CanvasPointer->draw_text ({ canvas_width/4, canvas_height/2 }, dieText, true);
        //se resetea el marcador
        snake.increaseLength(-(snake.getSnakeLength()-1));
        //se vuelve al menú
        director.run_scene (shared_ptr< Scene >(new Menu_Scene));


    }





}
