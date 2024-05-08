//
// Created by hunde on 3/30/2024.
//

#include "State.h"


State::State(StateMachine *p_sm, sf::RenderWindow *p_rw) : p_stateMachine(p_sm), p_window(p_rw)
{
}

void State::HandleEvents()
{
    for(sf::Event event{}; p_window->pollEvent(event);)
    {
        switch(event.type)
        {
            case sf::Event::Closed:
                p_window->close();
            break;
            case sf::Event::KeyPressed:
                HandleKeyboardInput(event.key.code);
            break;
        }
    }
}

void State::HandleKeyboardInput(sf::Keyboard::Key key)
{
}

