
#pragma once

#include <iostream>

class Position {
private:
    float x;
    bool is_x_set = false;
    float y;
    bool is_y_set = false;
public:
    inline Position(): x(0.f), y(0.f) {}
    inline Position(const float& _x, const float& _y): x(_x), is_x_set(true), y(_y), is_y_set(true) {}
    inline Position const& set(const float& _x, const float& _y)
    {
        x = _x;
        is_x_set = true;
        y = _y;
        is_y_set = true;
        return *this;
    };
    inline std::string to_str() const
    {
        return 
            (is_x_set ? ",\"x\":" + std::to_string(x) : "") +
            (is_y_set ? ",\"y\":" + std::to_string(y) : "");
    }
    inline bool is_set() const
    {
        return is_x_set || is_y_set;
    }
};

class Font {
private:
    std::string face;
    bool is_face_set = false;
    
    int size;
    bool is_size_set = false;
public:
    inline Font(): face("Sans Serif"), size(72) {}
    inline Font(const std::string& _face, const int& _size): face(_face), is_face_set(true), size(_size), is_size_set(true) {}
    inline Font(const int& _size): size(_size), is_size_set(true) {}
    inline std::string to_str() const
    { 
        return
            (is_face_set ? ",\"face\":\"" + face + "\"" : "") +
            (is_size_set ? ",\"size\":" + std::to_string(size) : "");
    }
};

class TextSettings {
private:
    std::string content;
    bool is_content_set = false;

    Font font;
    bool is_font_set = false;

    std::string alignment = "center";
    bool is_alignment_set = true;
public:
    Position position;
    inline TextSettings(): content(""), font() {}
    inline TextSettings(const std::string& _content, const Font& _font): content(_content), is_content_set(true), font(_font), is_font_set(true) {}
    inline void set_content(const std::string& _content)
    {
        content = _content;
        is_content_set = true;
    }
    inline std::string to_str() const
    {
        return 
            (is_content_set ? ",\"text\":\"" + content + "\"": "") +
            (is_font_set ? ",\"font\":{" + font.to_str().substr(1) + "}" : "") +
            (is_alignment_set ? ",\"alignment\":\"" + alignment + "\"" : "") +
            (position.is_set() ? ",\"position\":{" + position.to_str().substr(1) + "}" : "");
    }
};
