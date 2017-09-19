#pragma once

#include "Object.hpp"

class Item : public Object {
public:
    Item() {
        layer = 50;
    }

    void Update(sf::Time timeElapsed) override { };
    void Interact(Object *) override { };
};