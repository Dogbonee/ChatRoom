//
// Created by 24crickenbach on 5/8/2024.
//

#include "Textbox.h"

#include <iostream>

#include "ResourceLoader.h"

Textbox::Textbox(const sf::Vector2f& size, const sf::Vector2f& position) : m_bActive(false), m_blinkCounter(0), m_textRenderOffset(0)
{
    m_textBoxTexture.loadFromFile("../assets/rectBox.png");
    m_textBoxSprite.setTexture(m_textBoxTexture);
    m_textBoxSprite.setOrigin(m_textBoxSprite.getGlobalBounds().width/2, m_textBoxSprite.getGlobalBounds().height/2);
    m_textBoxSprite.setScale((size.x/400)*0.05,0.05);
    m_textBoxSprite.setPosition(position);


    //PNG is 8192x2774, sprite frame is 7100x1570
    //These values compute the value of the actual frame
    m_textBoxSpriteBounds = m_textBoxSprite.getGlobalBounds();
    m_textBoxSpriteBounds.left += m_textBoxSpriteBounds.width/16;
    m_textBoxSpriteBounds.width *= 7.0f/8.0f;
    m_textBoxSpriteBounds.top += m_textBoxSpriteBounds.height * 28.f/99.f;
    m_textBoxSpriteBounds.height *= 56.f/99.f;



    m_cursor.setFont(Resources::CursorFont);

    m_cursor.setString("l");
    m_cursor.setCharacterSize(50);
    m_cursor.setFillColor(sf::Color::Black);
    m_cursor.setOrigin(m_cursor.getGlobalBounds().width/2, m_cursor.getGlobalBounds().height/2);
    m_cursor.setPosition(m_textBoxSpriteBounds.left + 25, m_textBoxSpriteBounds.top+15);

    m_text.setFont(Resources::CursorFont);
    m_text.setString("");
    m_text.setCharacterSize(40);
    m_text.setFillColor(sf::Color::Black);
    m_text.setPosition(m_textBoxSpriteBounds.left + 25, m_textBoxSpriteBounds.top+3);



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
    //Backspace
    if(unicode == 8)
    {
        if(m_textBoxText.empty())return;
        m_textBoxText.pop_back();
        if(m_textRenderOffset > 0)
        {
            ScrollText(1);
            m_textRenderOffset--;
        }
        m_text.setString(m_textBoxText.substr(m_textRenderOffset));
    }
    //Push char
    else if(unicode != '\r')
    {
        m_textBoxText += unicode;
        m_text.setString(m_textBoxText.substr(m_textRenderOffset));
        while(m_text.getGlobalBounds().width + 50 >= m_textBoxSpriteBounds.width)
        {
            ScrollText(-1);
            m_textRenderOffset++;
            m_text.setString(m_textBoxText.substr(m_textRenderOffset));
        }
    }
    //Move cursor
    m_cursor.setPosition(m_text.getGlobalBounds().left + m_text.getGlobalBounds().width, m_cursor.getPosition().y);

}

void Textbox::ScrollText(int right)
{
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
    m_textBoxText.clear();
    m_textRenderOffset = 0;
}

const std::string& Textbox::GetString()
{
    return m_textBoxText;
}

void Textbox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_textBoxSprite);
    target.draw(m_cursor);
    target.draw(m_text);
}
