//
// Created by 24crickenbach on 5/8/2024.
//

#include "Textbox.h"

#include <iostream>

#include "ResourceLoader.h"

Textbox::Textbox(const sf::Vector2f& size, const sf::Vector2f& position) : m_bActive(false), m_blinkCounter(0)
{
    m_textBoxOutline.setSize(size);
    m_textBoxOutline.setOrigin(size.x/2, size.y/2);
    m_textBoxOutline.setOutlineColor(sf::Color::Black);
    m_textBoxOutline.setFillColor(sf::Color::Transparent);
    m_textBoxOutline.setOutlineThickness(2);
    m_textBoxOutline.setPosition(position);

    m_cursor.setFont(Resources::CursorFont);
    m_cursor.setString("l");
    m_cursor.setCharacterSize(50);
    m_cursor.setFillColor(sf::Color::Black);
    m_cursor.setPosition(m_textBoxOutline.getGlobalBounds().left + 10, m_textBoxOutline.getGlobalBounds().top + 5);

    m_text.setFont(Resources::CursorFont);
    m_text.setString("");
    m_text.setCharacterSize(40);
    m_text.setFillColor(sf::Color::Black);
    m_text.setPosition(m_textBoxOutline.getGlobalBounds().left + 10, m_textBoxOutline.getGlobalBounds().top + 10);

    m_textCursor.loadFromSystem(sf::Cursor::Text);
    m_normalCursor.loadFromSystem(sf::Cursor::Arrow);
}

void Textbox::ManageTextBox(sf::RenderWindow* window, sf::Event event)
{
    switch(event.type)
    {
        case sf::Event::MouseMoved:
            if(m_textBoxOutline.getGlobalBounds().contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
            {
                window->setMouseCursor(m_textCursor);
            }else
            {
                window->setMouseCursor(m_normalCursor);
            }
        break;
        case sf::Event::MouseButtonPressed:
            m_bActive = m_textBoxOutline.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
        break;
        case sf::Event::TextEntered:
            if(m_bActive)
            {
                ProcessText(event.text.unicode);
            }
        break;

    }
}

void Textbox::ProcessText(sf::Uint32 unicode)
{

    if(unicode == 8)
    {
        m_text.setString(m_text.getString().substring(0, m_text.getString().getSize()-1));
    }else if(m_text.getLocalBounds().width + 50 <= m_textBoxOutline.getLocalBounds().width && unicode != '\r')
    {
        m_text.setString(m_text.getString() + unicode);
    }
    m_cursor.setPosition(m_text.getGlobalBounds().left + m_text.getGlobalBounds().width, m_cursor.getPosition().y);

}

void Textbox::ManageCursorBlink(float dt)
{

    
    m_blinkCounter += dt;
    m_cursor.setFillColor(sf::Color(0,0,0, ((m_blinkCounter < m_cursorBlinkRate/2) * 255) * m_bActive) );
    if(m_blinkCounter > m_cursorBlinkRate)
    {
        m_blinkCounter = 0;
    }

}

void Textbox::SetActive(bool bIsActive)
{
    m_bActive = bIsActive;
}

void Textbox::ClearString()
{
    m_text.setString("");
}

const sf::String & Textbox::GetString()
{
    return m_text.getString();
}

void Textbox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_textBoxOutline);
    target.draw(m_cursor);
    target.draw(m_text);
}
