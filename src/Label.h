//
// Created by hunde on 5/8/2024.
//

#ifndef LABEL_H
#define LABEL_H
#include <SFML/Graphics/Text.hpp>


class Label : public sf::Text{


public:
    Label();
    void SetOriginToCenter();
    void SetPositionWithCenter(const sf::Vector2f& position);
};



#endif //LABEL_H
