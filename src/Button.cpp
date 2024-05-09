//
// Created by 24crickenbach on 5/9/2024.
//

#include "Button.h"

#include "ResourceLoader.h"

Button::Button() : m_normalColor(sf::Color::Black), m_hoverColor(200,200,200)
{

    setSize(sf::Vector2f(200,100));
    setOutlineColor(m_normalColor);
    setFillColor(sf::Color::Transparent);
    setOutlineThickness(4);
    setOrigin(getSize().x/2, getSize().y/2);

}

void Button::ManageButton(sf::RenderWindow *window, sf::Event event)
{
    switch(event.type)
    {
        case sf::Event::MouseMoved:
            m_bIsHovered = getGlobalBounds().contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
            if(m_bIsHovered)
            {
                setOutlineColor(m_hoverColor);
                window->setMouseCursor(Resources::HandCursor);
            }else
            {
                setOutlineColor(m_normalColor);
            }
        break;
        case sf::Event::MouseButtonReleased:
            if(m_bIsHovered)
            {
                Activate();
            }
        break;


    }
}

void Button::Activate()
{
    if(m_callback)
    {
        m_callback();
    }
}

void Button::SetCallback(std::function<void()> &callback)
{
    m_callback = callback;
}

void Button::SetButtonPosition(const sf::Vector2f &position)
{
    setPosition(position);
    m_buttonLabel.SetPositionWithCenter(position);
}

void Button::SetButtonLabelText(sf::String text)
{
    m_buttonLabel.setString(text);
}

void Button::SetButtonLabelSize(unsigned int characterSize)
{
    m_buttonLabel.setCharacterSize(characterSize);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(static_cast<sf::RectangleShape>(*this));
    target.draw(m_buttonLabel);
}
