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

    int m_scrollIndex;

public:
    TextContainer();
    void ManageTextContainer(sf::RenderWindow* window, sf::Event event);
    void Scroll(int up);
    void PushText(sf::String str);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};



#endif //TEXTCONTAINER_H
