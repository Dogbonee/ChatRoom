//
// Created by hunde on 5/8/2024.
//

#include "Label.h"

#include "ResourceLoader.h"

Label::Label()
{
    setFont(Resources::CursorFont);
    setCharacterSize(40);
    setFillColor(sf::Color::Black);
}

void Label::SetOriginToCenter()
{
    setOrigin(getLocalBounds().width/2, getLocalBounds().height/2);
}

void Label::SetPositionWithCenter(const sf::Vector2f &position)
{
    setPosition(position);
    SetOriginToCenter();
}
