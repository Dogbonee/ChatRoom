//
// Created by hunde on 5/8/2024.
//

#include "TextContainer.h"

#include <iostream>

#include "ResourceLoader.h"

TextContainer::TextContainer() : m_spacing(10), m_textHeight(50), m_scrollIndex(0)
{
    m_container.setSize(sf::Vector2f(700, 700));
    m_container.setOrigin(m_container.getSize().x/2, 0);
    m_container.setOutlineColor(sf::Color::Black);
    m_container.setFillColor(sf::Color::Transparent);
    m_container.setOutlineThickness(2);
    m_container.setPosition(400, 50);
}

void TextContainer::ManageTextContainer(sf::RenderWindow *window, sf::Event event)
{
    switch(event.type)
    {
        case sf::Event::MouseWheelScrolled:
            if(event.mouseWheelScroll.delta < 0 && m_texts.size() > 14 && m_scrollIndex < m_texts.size()-14)
            {
                m_scrollIndex++;
                Scroll(-1);
            }else if(event.mouseWheelScroll.delta > 0 && m_scrollIndex > 0)
            {
                m_scrollIndex--;
                Scroll(1);
            }
            break;
    }
}

void TextContainer::Scroll(int up)
{
    for(auto& text : m_texts)
    {
        text.move(0, m_textHeight * up);
    }
}

void TextContainer::PushText(sf::String str)
{
    sf::Text text;
    text.setFont(Resources::CursorFont);
    text.setString(str);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Black);


    if(m_texts.empty())
    {
        text.setPosition(m_container.getGlobalBounds().left + 10, m_container.getGlobalBounds().top + m_spacing);
    }else
    {
        text.setPosition(m_container.getGlobalBounds().left + 10, m_texts[m_texts.size()-1].getPosition().y + m_textHeight);
        //If the texts size is greater than the capacity of the container (14), then we scroll once and add to the scroll index
        if(m_texts.size() >= 14)
        {
            text.move(0, -m_textHeight);
            m_scrollIndex++;
            Scroll(-1);
        }
    }
    m_texts.emplace_back(text);
    if(m_scrollIndex < static_cast<int>(m_texts.size())-14)
    {
        Scroll(m_scrollIndex - (static_cast<int>(m_texts.size())-14));
        m_scrollIndex = static_cast<int>(m_texts.size())-14;
    }
    m_spacing += m_textHeight;

}

void TextContainer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_container);
    for(int i = m_scrollIndex; i < m_scrollIndex + 14; i++)
    {
        try
        {
            target.draw(m_texts.at(i));
        }catch(std::exception& e){}
    }
}
