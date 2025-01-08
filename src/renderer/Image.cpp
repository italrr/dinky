#include <cmath>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftstroke.h>
#include FT_FREETYPE_H

#include "../dinky.hpp"
#include "../common/Tools.hpp"
#include "Bitmap.hpp"

#define ENCODING_ASCII_RANGE_MIN 32
#define ENCODING_ASCII_RANGE_MAX 128

namespace FontRender {
    
    struct Glyph {
        DNK::Bitmap bmp;
        unsigned glyph;
        int horBearingY;
        int avgBearingY;
        DNK::Vec2<float> coors;
        DNK::Vec2<float> size;
        DNK::Vec2<float> orig;
        DNK::Vec2<float> index;       
        int symbol; 

        void solid(int w, int h, unsigned char* src){
            int channels = 4; 
            auto bsize = w * h * channels;
            unsigned char *buffer = new unsigned char[bsize];
            memset(buffer, 0, bsize);
            for(int i = 0; i < w*h; ++i){
                buffer[i*4 + 0] = src[i];
                buffer[i*4 + 1] = src[i];
                buffer[i*4 + 2] = src[i];
                buffer[i*4 + 3] = src[i];
            }
            this->bmp.load(w, h, 4, buffer);
            delete buffer;
        }           
    };

    struct Font {
        std::string name;
        std::unordered_map<unsigned, std::shared_ptr<Glyph>> map;
        std::unordered_map<unsigned, unsigned> ASCIITrans;
        int vertAdvance;
        unsigned advanceX;
        int horiBearingY;
        int avgBearingY;
        unsigned size;
    };

    static std::unordered_map<std::string, std::shared_ptr<FontRender::Font>> fonts;

    bool load(const std::string &name, const std::string &filename, unsigned size){

        if(!DNK::File::exists(filename)){
            fprintf(stderr, "Font::load: Failed to load font '%s': It doesn't exist\n",  filename.c_str());
            return false;
        }


        auto font = std::make_shared<FontRender::Font>();
        font->name = name;
        font->size = size;

        FT_Library library; 
        FT_Face face;
        FT_Stroker stroker;


        if(FT_Init_FreeType(&library)){
            fprintf(stderr, "Font::load: Failed to start FreeType: FT_Init_FreeType\n");
            return false;
        }

        if(FT_New_Face(library, filename.c_str(), 0, &face)) {
            fprintf(stderr, "Font::genMapping: Failed loading font '%s: FT_New_Face\n", filename.c_str());
            return false;
        }	

        if(FT_Set_Char_Size(face, 0, size << 6, 96, 96)){
            fprintf(stderr, "Font::genMapping: Failed loading font '%s': FT_Set_Char_Size\n", filename.c_str());	
            return false;
        }
        std::vector<FT_UInt> mapping;


        for(unsigned i = ENCODING_ASCII_RANGE_MIN; i < ENCODING_ASCII_RANGE_MAX; ++i){
            auto cindex = FT_Get_Char_Index(face, i);
            mapping.push_back(cindex);
            font->ASCIITrans[i] = cindex;
        }

        unsigned maxHeight = 0;
        FT_Stroker_New(library, &stroker);
        FT_Stroker_Set(stroker, (int)(2.0f * 64.0f), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);        

        font->vertAdvance = face->size->metrics.height >> 6;
        font->advanceX = face->glyph->advance.x >> 6;
        font->horiBearingY = face->glyph->metrics.horiBearingY >> 6;

        for(unsigned i = 0; i < mapping.size(); ++i){
            FT_Glyph glyph, stroke;
            std::shared_ptr<Glyph> sg = std::make_shared<Glyph>();
            auto id = mapping[i];
            sg->symbol = i;
            // Load glyph
            if(FT_Load_Glyph(face, id, FT_LOAD_DEFAULT)){
                fprintf(stderr, "Font::genMapping: Failed to load glyph '%c' for font '%s'\n", (char)i, filename.c_str());
                continue;
            }
            // Get base glyph
            if(FT_Get_Glyph(face->glyph, &glyph)){
                fprintf(stderr, "Font::genMapping: Failed to load glyph '%c' for font '%s'\n", (char)i, filename.c_str());
                continue;
            }     
            // Get stroke glyph
            if(FT_Get_Glyph(face->glyph, &stroke)){
                fprintf(stderr, "Font::genMapping: Failed to load glyph '%c' for font '%s'\n", (char)i, filename.c_str());
                continue;
            }             
            FT_Glyph_StrokeBorder(&stroke, stroker, 0, 1);
            font->map[id] = sg;


            FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, true);
            FT_BitmapGlyph bitmap = reinterpret_cast<FT_BitmapGlyph>(glyph);
            sg->solid(bitmap->bitmap.width, bitmap->bitmap.rows, bitmap->bitmap.buffer);
            sg->orig.x = bitmap->left;
            sg->orig.y = bitmap->top;
            sg->size.x = std::max((float)(face->glyph->advance.x >> 6), (float)bitmap->bitmap.width);
            sg->size.y = std::max((float)(face->glyph->advance.y >> 6), (float)bitmap->bitmap.rows);
            sg->horBearingY = face->glyph->metrics.horiBearingY >> 6;

            FT_Done_Glyph(glyph);
            FT_Done_Glyph(stroke);
        }

        for(auto &it : font->map){
            font->avgBearingY = std::max((float)font->avgBearingY, (float)it.second->horBearingY);
        }


        FT_Stroker_Done(stroker);
        FT_Done_Face(face);
        FT_Done_FreeType(library);

        printf("Font: loaded '%s'\n", filename.c_str());

        fonts[name] = font;

        return true;
    }

    bool render(const std::string &name, const std::string &text, const DNK::Vec2<unsigned> &pos, const DNK::Color &color, DNK::Bitmap &target){
        if(fonts.count(name) == 0){
            fprintf(stderr, "Font::render: Failed to find font '%s'\n", name.c_str());
            return false;
        }
        auto &font = fonts[name];
        DNK::Vec2<int> cursor { 0 , 0 };
        for(int i = 0; i < text.size(); ++i){
			char current =  text[i];
            auto &glyph = font->map[font->ASCIITrans[current]];
            auto p = pos + DNK::Vec2<unsigned>(cursor.x + glyph->orig.x, cursor.y + (- glyph->horBearingY) + font->avgBearingY);
            target.pasteAndShade(glyph->bmp, p.x, p.y, color);
            cursor.x += glyph->size.x;
        }
        return true;
    }

    DNK::Vec2<unsigned> getDimensions(const std::string &name, const std::string &text){
        DNK::Vec2<unsigned> dims(0);
        if(fonts.count(name) == 0){
            fprintf(stderr, "Font::render: Failed to find font '%s'\n", name.c_str());
            return dims;
        }
        auto &font = fonts[name];
        dims.y = font->avgBearingY;
        for(int i = 0; i < text.size(); ++i){
			char current =  text[i];
            auto &glyph = font->map[font->ASCIITrans[current]];
            dims.x += glyph->size.x;
        }        
        return dims;
    }
}


namespace Shape {

}

// width 215.9
// height 279.4

bool RenderImage(const std::shared_ptr<DNK::Document> &doc, const std::string &filename, const DNK::Vec2<int> &expSize){

    FontRender::load("arial12", "lib/font/arial.ttf", 64);

    DNK::Bitmap canvas;
    DNK::Vec2<int> cursor;
    DNK::Color color;
    DNK::Vec2<int> minSize = {
        (int)DNK::Math::max( static_cast<float>(expSize[0] * doc->settings.dpi), static_cast<float>(5 * doc->settings.dpi) ),
        (int)DNK::Math::max( static_cast<float>(expSize[1] * doc->settings.dpi), static_cast<float>(1 * doc->settings.dpi) )
    };


    printf("Target: Bitmap %ix%i RGBA\n", minSize[0], minSize[1]);


    canvas.build(
        DNK::Colors::White,  // TODO: use settings
        DNK::ImageFormat::RGBA,
        minSize[0],
        minSize[1]
    );
    canvas.fill(DNK::Colors::White);
    FontRender::render("arial12", "THIS is a test text", {0, 0}, DNK::Color(0.0f, 0.0f, 0.0f, 1.0f),  canvas);


    
    canvas.write(filename);

    return true;
}