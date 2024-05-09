//
// Created by 24crickenbach on 5/8/2024.
//

#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H
#include <memory>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Cursor.hpp>

namespace Resources {
    inline sf::Font CursorFont;

    inline sf::Cursor NormalCursor;
    inline sf::Cursor HandCursor;
    inline sf::Cursor TextCursor;


    void LoadResources();
}


#endif

