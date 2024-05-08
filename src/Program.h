//
// Created by hunde on 3/30/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include "State.h"
#include "Textbox.h"

class Program : public State{


    Textbox m_textBox;
    sf::RectangleShape m_chatBorder;

    sf::Clock m_clock;
    float m_dt;


    void Update() override;
    void Render() override;
    void HandleEvents() override;

public:
    Program(StateMachine* p_sm, sf::RenderWindow* p_rw);
    ~Program();

};



#endif //PROGRAM_H
