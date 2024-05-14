//
// Created by 24crickenbach on 5/8/2024.
//

#include "Textbox.h"

#include <iostream>

#include "ResourceLoader.h"

Textbox::Textbox(const sf::Vector2f& size, const sf::Vector2f& position) : m_bActive(false), m_blinkCounter(0)
{
    m_textBoxTexture.loadFromFile("../assets/rectBox.png");
    m_textBoxSprite.setTexture(m_textBoxTexture);
    m_textBoxSprite.setOrigin(m_textBoxSprite.getGlobalBounds().width/2, m_textBoxSprite.getGlobalBounds().height/2);
    m_textBoxSprite.setScale((size.x/400)*0.05,0.05);
    m_textBoxSprite.setPosition(position);

    m_cursor.setFont(Resources::CursorFont);

    m_cursor.setString("l");
    m_cursor.setCharacterSize(50);
    m_cursor.setFillColor(sf::Color::Black);
    m_cursor.setOrigin(m_cursor.getLocalBounds().width/2, m_cursor.getLocalBounds().height/2);
    m_cursor.setPosition(m_textBoxSprite.getGlobalBounds().left + size.x/8, m_textBoxSprite.getGlobalBounds().top + m_textBoxSprite.getGlobalBounds().height/2-15);

    m_text.setFont(Resources::CursorFont);
    m_text.setString("");
    m_text.setCharacterSize(40);
    m_text.setFillColor(sf::Color::Black);
    m_text.setPosition(m_textBoxSprite.getGlobalBounds().left + size.x/8, m_textBoxSprite.getGlobalBounds().top + m_textBoxSprite.getGlobalBounds().height/2-25);

}

void Textbox::ManageTextBox(sf::RenderWindow* window, sf::Event event)
{
    switch(event.type)
    {
        case sf::Event::MouseMoved:
            if(m_textBoxSprite.getGlobalBounds().contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
            {
                window->setMouseCursor(Resources::TextCursor);
            }else
            {
                window->setMouseCursor(Resources::NormalCursor);
            }
        break;
        case sf::Event::MouseButtonPressed:
            m_bActive = m_textBoxSprite.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
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
    }else if(m_text.getLocalBounds().width + 50 <= m_textBoxSprite.getGlobalBounds().width - 140 && unicode != '\r')
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
    target.draw(m_textBoxSprite);
    target.draw(m_cursor);
    target.draw(m_text);
}
