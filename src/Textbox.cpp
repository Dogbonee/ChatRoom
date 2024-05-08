//
// Created by 24crickenbach on 5/8/2024.
//

#include "Textbox.h"

#include "ResourceLoader.h"

Textbox::Textbox() : m_bActive(false)
{
    m_textBoxOutline.setSize(sf::Vector2f(700, 70));
    m_textBoxOutline.setOrigin(m_textBoxOutline.getSize().x/2, m_textBoxOutline.getSize().y/2);
    m_textBoxOutline.setOutlineColor(sf::Color::Black);
    m_textBoxOutline.setFillColor(sf::Color::Transparent);
    m_textBoxOutline.setOutlineThickness(2);
    m_textBoxOutline.setPosition(400, 820);

    m_cursor.setFont(Resources::CursorFont);
    m_cursor.setString("l");
    m_cursor.setCharacterSize(50);
    m_cursor.setFillColor(sf::Color::Black);
    m_cursor.setPosition(m_textBoxOutline.getGlobalBounds().left + 10, m_textBoxOutline.getGlobalBounds().top + 5);

    m_textCursor.loadFromSystem(sf::Cursor::Text);
    m_normalCursor.loadFromSystem(sf::Cursor::Arrow);
}

bool Textbox::ManageTextBox(sf::RenderWindow* window, sf::Vector2i mousePos)
{
    if(m_textBoxOutline.getGlobalBounds().contains(sf::Vector2f(mousePos)))
    {
        window->setMouseCursor(m_textCursor);
        return true;
    }
    window->setMouseCursor(m_normalCursor);
    return false;
}

void Textbox::ManageCursorBlink(float dt)
{

    static float counter = 0.f;
    counter += dt;
    m_cursor.setFillColor(sf::Color(0,0,0, ((counter < m_cursorBlinkRate/2) * 255) * m_bActive) );
    if(counter > m_cursorBlinkRate)
    {
        counter = 0;
    }

}

void Textbox::SetActive(bool bIsActive)
{
    m_bActive = bIsActive;
}

void Textbox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_textBoxOutline);
    target.draw(m_cursor);
}
