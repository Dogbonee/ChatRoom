//
// Created by 24crickenbach on 5/8/2024.
//

#include "ResourceLoader.h"

void Resources::LoadResources()
{

    CursorFont.loadFromFile("C:/Windows/Fonts/calibri.ttf");

    NormalCursor.loadFromSystem(sf::Cursor::Arrow);
    HandCursor.loadFromSystem(sf::Cursor::Hand);
    TextCursor.loadFromSystem(sf::Cursor::Text);
}
