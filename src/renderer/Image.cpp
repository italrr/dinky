#include <cmath>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftstroke.h>
#include FT_FREETYPE_H

#include "../dinky.hpp"
#include "../common/Tools.hpp"
#include "../common/Bitmap.hpp"

#define ENCODING_ASCII_RANGE_MIN 32
#define ENCODING_ASCII_RANGE_MAX 128

// width 215.9
// height 279.4

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

        printf("Font: loaded '%s' (%ipx)\n", filename.c_str(), size);

        fonts[name] = font;

        return true;
    }

    bool render(const std::string &name, const std::string &text, const DNK::Vec2<unsigned> &pos, const DNK::Color &color, DNK::Bitmap *target){
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
            target->pasteAndShade(&glyph->bmp, p.x, p.y, color);
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

    void Line(int x1, int y1, int x2, int y2, const DNK::Color &color, DNK::Bitmap &target){
        int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
        dx = x2 - x1; dy = y2 - y1;
        dx1 = std::abs(dx);
        dy1 = std::abs(dy);
        px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
        if (dy1 <= dx1){
            if (dx >= 0){
                x = x1; y = y1; xe = x2;
            }else{
                x = x2;
                y = y2;
                xe = x1;
            }
            if(x < 0 || x >= target.width || y < 0 || y >= target.height){
                return;
            }                   
            target.pixels[x + y * target.width] = color;
            for (i = 0; x<xe; i++){
                x = x + 1;
                if (px<0){
                    px = px + 2 * dy1;
                }else{
                    if((dx<0 && dy<0) || (dx>0 && dy>0)){
                        y = y + 1;
                    }else{
                        y = y - 1;
                    }
                    px = px + 2 * (dy1 - dx1);
                }
                if(x < 0 || x >= target.width || y < 0 || y >= target.height){
                    continue;
                }              
                target.pixels[x + y * target.width] = color;
            }
        }else{
            if (dy >= 0){
                x = x1;
                y = y1;
                ye = y2;
            }else{
                x = x2;
                y = y2;
                ye = y1;
            }
            if(x < 0 || x >= target.width || y < 0 || y >= target.height){
                return;
            }               
            target.pixels[x + y * target.width] = color;
            for (i = 0; y<ye; i++){
                y = y + 1;
                if (py <= 0){
                    py = py + 2 * dx1;
                }else{
                    if((dx<0 && dy<0) || (dx>0 && dy>0)){
                        x = x + 1;
                    }else{
                        x = x - 1;
                    }
                    py = py + 2 * (dx1 - dy1);
                }     
                if(x < 0 || x >= target.width || y < 0 || y >= target.height){
                    continue;
                }                  
                target.pixels[x + y * target.width] = color;
            }
        }

    }

    void Rectangle(int x, int y, int w, int h, const DNK::Color &color, DNK::Bitmap &target){
        // TOP
        Shape::Line(x, y, x + w, y, color, target);
        // LEFT
        Shape::Line(x, y, x, y+h, color, target);
        // RIGHT
        Shape::Line(x+w, y, x+w, y+h, color, target);
        // BOTTOM
        Shape::Line(x, y+h, x + w, y+h, color, target);
    }

    void FilledRect(int x, int y, int w, int h, const DNK::Color &color, DNK::Bitmap &target){
        for(int xi = 0; xi < w; ++xi){
            for(int yi = 0; yi < h; ++yi){
                int fx = x+xi;
                int fy = y+yi;
                if(fx < 0 || fx >= target.width || fy < 0 || fy >= target.height){
                    continue;
                }
                int index = (fx) + (fy) * target.width;
                target.pixels[index] = color;
            }
        }
    }

    void Circle(int x, int y, int r, int verts, const DNK::Color &color, DNK::Bitmap &target){
        int n = 360 / verts;
        for(int i = n; i <= 360; i += n){
            int n0 = i - n;
            int n1 = i;
            
            int x1 = r * DNK::Math::cos(DNK::Math::rads(n0)) + x;
            int y1 = r * DNK::Math::sin(DNK::Math::rads(n0)) + y;

            int x2 = r * DNK::Math::cos(DNK::Math::rads(n1)) + x;
            int y2 = r * DNK::Math::sin(DNK::Math::rads(n1)) + y;

            Shape::Line(x1, y1, x2, y2, color, target);
        }
    }


    void FilledCircle(int x, int y, int r, int verts, const DNK::Color &color, DNK::Bitmap &target){
        int d = r * 2;
        for(int xi = 0; xi < d; ++xi){
            for(int yi = 0; yi < d; ++yi){
                int fx = xi - r;
                int fy = yi - r;
                auto mod = DNK::Math::sqrt(fx*fx + fy*fy);
                if(mod <= r){
                    int _x = xi + x;
                    int _y = yi + y;
                    if(_x < 0 || _x >= target.width || _y < 0 || _y >= target.height){
                        continue;
                    }
                    int index = (_x) + (_y) * target.width;
                    target.pixels[index] = color;
                }
            }
        }
    }    
    
}

struct DocumentHandle {
    DNK::Vec2<int> cursor;
    DNK::Color color;
    DNK::Vec2<int> minSize;
    std::string currentFont;
    int pixelSize;

    DocumentHandle(){
        currentFont = "default";
        cursor.set(0);
        color.set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void init(unsigned pixelSize){
        this->pixelSize = pixelSize;
        this->minSize.set(
            pixelSize * 15,
            pixelSize * 2
        );
    }
};

struct RenderProduct {
    std::shared_ptr<DNK::Bitmap> canvas;
    RenderProduct(){
        canvas = std::make_shared<DNK::Bitmap>();
    }
};

static RenderProduct render(const std::shared_ptr<DNK::Node> &node, DNK::Node *head, int wideSpace, DocumentHandle &handle){
    RenderProduct self;

    DNK::Vec2<int> margin (
        node->getNumberStyle("m") * handle.pixelSize,
        node->getNumberStyle("s") * handle.pixelSize
    );

    int avLinSpace = wideSpace - margin.x * 2;
    int avLinRSpace = wideSpace - margin.x * 2;

    switch(node->type){
        case DNK::NodeType::PANEL: {
            std::vector<RenderProduct> products;
            for(int i = 0; i < node->children.size(); ++i){
                products.push_back(
                    render(node->children[i], node.get(), avLinSpace, handle)
                );
            }
            int height = margin.y;
            for(int i = 0; i < products.size(); ++i){
                height += products[i].canvas->height;
            }
            self.canvas->build(DNK::Colors::White, DNK::ImageFormat::RGBA, handle.minSize.x, height);
            int y = 0;
            for(int i = 0; i < products.size(); ++i){
                auto &p = products[i];
                self.canvas->paste(
                    p.canvas.get(),
                    0,
                    y,
                    true
                );
                y += p.canvas->height;
            }
        } break;        
        case DNK::NodeType::TEXT: {
            auto tokens = DNK::String::split(node->str, ' ');
            DNK::Vec2<unsigned> cursor(margin.x, margin.y);
            auto empty = FontRender::getDimensions(handle.currentFont, "A");
            int lineHeight = DNK::Math::round(node->getNumberStyle("ln") != 0 ? (node->getNumberStyle("ln")+0.3f)*(float)empty.y : (float)empty.y*1.3f);
            auto advX = empty.x;
            auto advY = empty.y;
            int spacey = margin.y + lineHeight;
            // Get measurement
            for(int i = 0; i < tokens.size(); ++i){
                auto &t = tokens[i];
                auto dims = FontRender::getDimensions(handle.currentFont, t);
                if(cursor.x + dims.x > avLinRSpace){
                    cursor.y += lineHeight;
                    spacey += lineHeight;
                    cursor.x = margin.x;
                    cursor.x += advX + dims.x; 
                }else{                   
                    cursor.x += advX + dims.x;
                }
            }
            // Render
            cursor.set(margin.x, margin.y);
            self.canvas->build(DNK::Colors::White, DNK::ImageFormat::RGBA, handle.minSize.x, spacey);
            for(int i = 0; i < tokens.size(); ++i){
                auto &t = tokens[i];
                auto dims = FontRender::getDimensions(handle.currentFont, t);
                if(cursor.x + dims.x > avLinRSpace){
                    cursor.y += lineHeight;
                    cursor.x = margin.x;
                    FontRender::render(handle.currentFont, t, cursor, handle.color, self.canvas.get());
                    cursor.x += advX + dims.x;                   
                }else{
                    FontRender::render(handle.currentFont, t, cursor, handle.color, self.canvas.get());
                    cursor.x += advX + dims.x;
                }
            }          
        } break;
    }
    return self;
}

bool DNK::RenderImage(const std::shared_ptr<DNK::Document> &doc, const std::string &filename, unsigned pd){

    int pixelSize = 14 * pd;
    int fontSize = pixelSize / 2;

    // Load default font
    FontRender::load("default", "lib/font/arial.ttf", fontSize);


    DocumentHandle handle;
    handle.init(pixelSize);
    auto body = render(doc->body, NULL, handle.minSize.x, handle);





    
    // Write
    body.canvas->write(filename);
    printf("Target: IMAGE %ix%i RGBA\n", body.canvas->width, body.canvas->height);
    printf("Wrote '%s'\n", filename.c_str());

    return true;
}