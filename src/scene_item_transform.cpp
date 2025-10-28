
#include "scene_item_transform.hpp"

SceneItemTransform::SceneItemTransform(){
    cropBottom = Transform("cropBottom");
    cropLeft = Transform("cropLeft");
    cropRight = Transform("cropRight");
    cropTop = Transform("cropTop");
    height = Transform("height");
    width = Transform("width");
    positionX = Transform("positionX");
    positionY = Transform("positionY");
    rotation = Transform("rotation");
    scaleX = Transform("scaleX");
    scaleY = Transform("scaleY");
    sourceHeight = Transform("sourceHeight");
    sourceWidth = Transform("sourceWidth");
}

std::string SceneItemTransform::to_str() const {
    return cropBottom.to_str()
     + cropLeft.to_str()
     + cropRight.to_str()
     + cropTop.to_str()
     + height.to_str()
     + width.to_str()
     + positionX.to_str()
     + positionY.to_str()
     + rotation.to_str()
     + scaleX.to_str()
     + scaleY.to_str()
     + sourceHeight.to_str()
     + sourceWidth.to_str(); 
}
