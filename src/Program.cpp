//
// Created by hunde on 3/30/2024.
//

#include "Program.h"

Program::Program(StateMachine* p_sm, sf::RenderWindow* p_rw) : State(p_sm, p_rw)
{
    m_chatBorder.setSize(sf::Vector2f(700, 700));
    m_chatBorder.setOrigin(m_chatBorder.getSize().x/2, 0);
    m_chatBorder.setOutlineColor(sf::Color::Black);
    m_chatBorder.setFillColor(sf::Color::Transparent);
    m_chatBorder.setOutlineThickness(2);
    m_chatBorder.setPosition(400, 50);
}

Program::~Program() = default;

void Program::Update()
{
    m_dt = m_clock.restart().asSeconds();
    m_textBox.ManageCursorBlink(m_dt);
    HandleEvents();
    Render();
}


void Program::Render()
{
    p_window->clear(sf::Color(220,220,220));
    p_window->draw(m_textBox);
    p_window->draw(m_chatBorder);
    p_window->display();
}

void Program::HandleEvents()
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
            case sf::Event::MouseMoved:
                m_textBox.ManageTextBox(p_window, sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                break;
            case sf::Event::MouseButtonPressed:
                m_textBox.SetActive(m_textBox.ManageTextBox(p_window, sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
                break;
        }
    }
}
