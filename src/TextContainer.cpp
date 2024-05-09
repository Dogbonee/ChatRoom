//
// Created by hunde on 5/8/2024.
//

#include "TextContainer.h"

#include "ResourceLoader.h"

TextContainer::TextContainer() : m_spacing(10), m_textHeight(50)
{
    m_container.setSize(sf::Vector2f(700, 700));
    m_container.setOrigin(m_container.getSize().x/2, 0);
    m_container.setOutlineColor(sf::Color::Black);
    m_container.setFillColor(sf::Color::Transparent);
    m_container.setOutlineThickness(2);
    m_container.setPosition(400, 50);
}

void TextContainer::PushText(sf::String str)
{
    sf::Text text;
    text.setFont(Resources::CursorFont);
    text.setString(str);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::Black);
    text.setPosition(m_container.getGlobalBounds().left + 10, m_container.getGlobalBounds().top + m_spacing);
    m_spacing += m_textHeight;
    m_texts.emplace_back(text);
}

void TextContainer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_container);
    for(const auto& text : m_texts)
    {
        target.draw(text);
    }
}
