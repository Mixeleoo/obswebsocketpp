#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP


#include <iostream>

class Transform {
private:
    float value;
    bool is_value;
    std::string name;
public:
    Transform(): value(0.0f), is_value(false), name("") {}
    Transform(const std::string& name) : value(0.0f), is_value(false), name(name) {}
    inline void set(const float& newvalue) { value = newvalue; is_value = true; }
    inline float get() { return value; }
    inline std::string to_str() const { return is_value ? ",\"" + name + "\":" + std::to_string(value) : ""; }
};

#endif