//
// Created by 24crickenbach on 5/9/2024.
//

#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include <SFML/Graphics.hpp>

#include "Label.h"

class Button : public sf::RectangleShape{

    Label m_buttonLabel;
    bool m_bIsHovered;
    std::function<void()> m_callback;
    sf::Color m_hoverColor;
    sf::Color m_normalColor;

public:
    Button();
    void ManageButton(sf::RenderWindow* window, sf::Event event);
    void Activate();
    void SetCallback(std::function<void()>& callback);
    void SetButtonPosition(const sf::Vector2f& position);
    void SetButtonLabelText(sf::String text);
    void SetButtonLabelSize(unsigned int characterSize);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};



#endif //BUTTON_H
