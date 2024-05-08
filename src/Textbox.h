//
// Created by 24crickenbach on 5/8/2024.
//

#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <SFML/Graphics.hpp>


class Textbox : public sf::Drawable{

    bool m_bActive;

    sf::RectangleShape m_textBoxOutline;
    sf::Text m_cursor;

    sf::Cursor m_textCursor;
    sf::Cursor m_normalCursor;

    float m_cursorBlinkRate = 1.0f;

public:
    Textbox();
    bool ManageTextBox(sf::RenderWindow* window, sf::Vector2i mousePos);
    void ManageCursorBlink(float dt);
    void SetActive(bool bIsActive);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};





#endif //TEXTBOX_H
