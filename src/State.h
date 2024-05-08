//
// Created by hunde on 3/30/2024.
//

#ifndef STATE_H
#define STATE_H
#include <SFML/Graphics.hpp>

class StateMachine;



class State {


protected:

    StateMachine* p_stateMachine;
    sf::RenderWindow* p_window;


    virtual void HandleEvents();
    virtual void HandleKeyboardInput(sf::Keyboard::Key key);
    virtual void Render() = 0;

public:
    virtual ~State() = default;

    State(StateMachine* p_sm, sf::RenderWindow* p_rw);
    virtual void Update() = 0;

};



#endif //STATE_H
