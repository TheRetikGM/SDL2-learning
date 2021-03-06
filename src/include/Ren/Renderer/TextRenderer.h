#pragma once

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Ren/Core/Core.h"
#include "Ren/RenSDL/Texture.hpp"

namespace Ren
{
    struct Character {
        Texture2D texture;
        bool has_texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint32_t advance;
    }; 

    class TextRenderer
    {
    public:
        std::map<char, Character> m_Characters;
        unsigned int m_RowSpacing = 20;

        ~TextRenderer();
        static Ref<TextRenderer> Create() { return Ref<TextRenderer>(new TextRenderer()); }

        void Load(std::string font_path, unsigned int fontSize);
        void RenderText(std::string text, glm::vec2 pos, float scale, Color3 color = Colors3::White, int32_t layer = 0);
        glm::ivec2 GetStringSize(std::string str, float scale = 1.0f) const;
        unsigned int GetFontSize() const { return m_fontSize; }

    private:
        uint8_t m_fontSize;

        TextRenderer();
    };
}
