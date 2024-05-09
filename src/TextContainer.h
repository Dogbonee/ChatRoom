//
// Created by hunde on 5/8/2024.
//

#ifndef TEXTCONTAINER_H
#define TEXTCONTAINER_H

#include <SFML/Graphics.hpp>

class TextContainer : public sf::Drawable{

    sf::RectangleShape m_container;

    std::vector<sf::Text> m_texts;
    float m_spacing;
    float m_textHeight;

public:
    TextContainer();
    void PushText(sf::String str);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};



#endif //TEXTCONTAINER_H
