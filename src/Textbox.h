//
// Created by 24crickenbach on 5/8/2024.
//

#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <SFML/Graphics.hpp>


class Textbox : public sf::Drawable{

    bool m_bActive;

    sf::Texture m_textBoxTexture;
    sf::Sprite m_textBoxSprite;
    sf::FloatRect m_textBoxSpriteBounds;
    sf::Text m_cursor;
    float m_blinkCounter;

    sf::Text m_text;
    std::string m_textBoxText;
    int m_textRenderOffset;


    float m_cursorBlinkRate = 1.0f;

public:
    Textbox(const sf::Vector2f& size, const sf::Vector2f& position);
    void ManageTextBox(sf::RenderWindow* window, sf::Event event);
    void ProcessText(sf::Uint32 unicode);
    void ScrollText(int right);
    void ManageCursorBlink(float dt);
    void SetActive(bool bIsActive);
    void ClearString();
    const std::string& GetString();
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};





#endif //TEXTBOX_H
