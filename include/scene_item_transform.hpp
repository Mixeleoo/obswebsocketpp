#ifndef SCENE_ITEM_TRANSFORM_HPP
#define SCENE_ITEM_TRANSFORM_HPP

#include "transform.hpp"

class SceneItemTransform {
public:
    Transform cropBottom;
    Transform cropLeft;
    Transform cropRight;
    Transform cropTop;
    Transform height;
    Transform width;
    Transform positionX;
    Transform positionY;
    Transform rotation;
    Transform scaleX;
    Transform scaleY;
    Transform sourceHeight;
    Transform sourceWidth;

    SceneItemTransform();
    std::string to_str() const;
};

#endif