#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../common/thirdparty/stb_image.h"
#include "../common/thirdparty/stb_image_write.h"
#include "../common/Tools.hpp"

#include "Bitmap.hpp"

DNK::Bitmap DNK::Bitmap::sub(const DNK::Rect<unsigned> &box){
    return this->sub(box.x, box.y, box.w, box.h);
}

DNK::Bitmap DNK::Bitmap::sub(unsigned x, unsigned y, unsigned width, unsigned height){
    DNK::Bitmap result;
    result.build(DNK::Color(0, 0, 0, 0), this->format, width, height);
    
    for(unsigned _x = 0; _x < width; ++_x){
        for(unsigned _y = 0; _y < height; ++_y){
            result.pixels[_x + _y * width] = this->pixels[(_x+x) + (_y+y) * this->width];
        }
        
    }
    
    return result;
}

DNK::Bitmap DNK::Bitmap::rotate(float angle){
    float cmod = DNK::Math::sqrt( (float)this->width * (float)this->width + (float)this->height * (float)this->height );
    float cangle = DNK::Math::atan((float)this->height, (float)this->width);
    
    int nW = DNK::Math::round(DNK::Math::abs(DNK::Math::cos(cangle + angle) * cmod));
    int nH = DNK::Math::round(DNK::Math::abs(DNK::Math::sin(cangle + angle) * cmod));

    auto result = DNK::Bitmap();
    result.build(DNK::Color(1.0f, 1.0f, 1.0f, 0.0f), this->format, nW, nH);
    
    for(int _x = 0; _x < this->width; ++_x){
        for(int _y = 0; _y < this->height; ++_y){
            
            int srcIndex = (_x + _y * this->width);
            
            
            double rx = (double)_x - ((double)this->width-1.0) * 0.5;
            double ry = (double)_y - ((double)this->height-1.0) * 0.5;
            
            double rmod = DNK::Math::sqrt(rx * rx + ry * ry);
            double rangle = DNK::Math::atan(ry, rx);
            
            double rX = DNK::Math::cos(rangle + angle) * rmod;
            double rY = DNK::Math::sin(rangle + angle) * rmod;
            
            double fX = DNK::Math::round(rX + ((double)result.width-1.0) * 0.5);
            double fY = DNK::Math::round(rY + ((double)result.height-1.0) * 0.5);  
            
            int index = ((int)fX + (int)fY * result.width);
            
            result.pixels[index] = this->pixels[srcIndex];
        }
    }		
    
    
    return result;
}

void DNK::Bitmap::paste(const DNK::Bitmap &src, const DNK::Vec2<unsigned> &vec){
    this->paste(src, vec.x, vec.y);
}

void DNK::Bitmap::paste(const DNK::Bitmap &src, unsigned x, unsigned y){
    this->paste(src, x, y, false);
}

void DNK::Bitmap::shade(const DNK::Color &color){
    for(unsigned i = 0; i < this->pixels.size(); ++i){
        auto &p = this->pixels[i];
        switch(this->format){
            case ImageFormat::RGBA: {
                p.r *= color.r;
                p.g *= color.g;
                p.b *= color.b;
                p.a *= color.a;
            } break;
        }
    }    
}

void DNK::Bitmap::resize(unsigned nwidth, unsigned nheight){
    auto cp = this->copy();
    this->clear();
    this->build(DNK::Colors::White, this->format, nwidth, nheight);
    this->paste(cp, {0, 0});
}

void DNK::Bitmap::pasteAndShade(const Bitmap &src, unsigned x, unsigned y, const DNK::Color &c){
    if(src.format != this->format) {
        printf("Bitmap paste: Cannot paste a Bitmap onto another Bitmap of a differing image format\n");
        return;
    }
    for(int _x = 0; _x < src.width; ++_x){
        for(int _y = 0; _y < src.height; ++_y){
            
            if(_x + x >= this->width || _y + y >= this->height) continue;
            
            int srcIndex = (_x + _y * src.width);
            int index = (_x + x + (_y + y) * width);
            
            if(channels == 4) {
                float srcRed = src.pixels[srcIndex].r * c.r;
                float srcGreen = src.pixels[srcIndex].g * c.g;
                float srcBlue =  src.pixels[srcIndex].b * c.b;
                float srcAlpha = src.pixels[srcIndex].a;
                
                float targetRed = this->pixels[index].r;
                float targetGreen = this->pixels[index].g;
                float targetBlue = this->pixels[index].b;
                
                this->pixels[index].r = (srcRed * srcAlpha) + (targetRed * (1.0f - srcAlpha));
                this->pixels[index].g = (srcGreen * srcAlpha) + (targetGreen * (1.0f - srcAlpha));
                this->pixels[index].b = (srcBlue * srcAlpha) + (targetBlue * (1.0f - srcAlpha));
                this->pixels[index].a = 1.0f;
            }
            
        }
    }
}

void DNK::Bitmap::paste(const Bitmap &src, unsigned x, unsigned y, bool alphaBlend){
    if(src.format != this->format) {
        printf("Bitmap paste: Cannot paste a Bitmap onto another Bitmap of a differing image format\n");
        return;
    }
    for(int _x = 0; _x < src.width; ++_x){
        for(int _y = 0; _y < src.height; ++_y){
            
            if(_x + x >= this->width || _y + y >= this->height) continue;
            
            int srcIndex = (_x + _y * src.width);
            int index = (_x + x + (_y + y) * width);
            
            if(alphaBlend && channels == 4) {
                float srcRed = src.pixels[srcIndex].r;
                float srcGreen = src.pixels[srcIndex].g;
                float srcBlue =  src.pixels[srcIndex].b;
                float srcAlpha = src.pixels[srcIndex].a;
                
                float targetRed = this->pixels[index].r;
                float targetGreen = this->pixels[index].g;
                float targetBlue = this->pixels[index].b;
                
                this->pixels[index].r = (srcRed * srcAlpha) + (targetRed * (1.0f - srcAlpha));
                this->pixels[index].g = (srcGreen * srcAlpha) + (targetGreen * (1.0f - srcAlpha));
                this->pixels[index].b = (srcBlue * srcAlpha) + (targetBlue * (1.0f - srcAlpha));
                this->pixels[index].a = 1.0f;
            }else {									
                if(index < 0 || srcIndex < 0 || index > this->pixels.size() || srcIndex > src.pixels.size()) {
                    continue;
                }

                this->pixels[index] = src.pixels[srcIndex];
            }
            
        }
    }
}

void DNK::Bitmap::fill(DNK::Color &c){
    for(int i = 0; i < this->pixels.size(); ++i){
        this->pixels[i] = c;
    }
}

std::vector<uint8>  DNK::Bitmap::getFlatArray() const {
    std::vector<uint8> pixels;
    for(unsigned i = 0; i < this->pixels.size(); ++i){
        auto p = this->pixels[i].rgb();
        switch(this->format){
            case ImageFormat::RGBA: {
                pixels.push_back(p[0]);
                pixels.push_back(p[1]);
                pixels.push_back(p[2]);
                pixels.push_back(p[3]);
            } break;
            case ImageFormat::RGB: {
                pixels.push_back(p[0]);
                pixels.push_back(p[1]);
                pixels.push_back(p[2]);
            } break;     
            case ImageFormat::RED: {
                pixels.push_back(p[0]);
            } break;   
            case ImageFormat::GREEN: {
                pixels.push_back(p[1]);
            } break; 
            case ImageFormat::BLUE: {
                pixels.push_back(p[2]);
            } break;      
            case ImageFormat::RG: {
                pixels.push_back(p[0]);
                pixels.push_back(p[1]);
            } break;                                                    
        } 
    }
    return pixels;
}

DNK::Bitmap DNK::Bitmap::copy(){
    DNK::Bitmap result;
    result.width = this->width;
    result.height = this->height;
    result.format = this->format;
    result.channels = this->channels;
    for(unsigned i = 0; i < this->width * this->height; ++i){
        result.pixels[i] = this->pixels[i];
    }
    return result;
}

DNK::Rect<unsigned> DNK::Bitmap::autocrop(){
    unsigned min_x = this->width;
    unsigned min_y = this->height;
    unsigned max_x = 0;
    unsigned max_y = 0;
    
    for(unsigned y = 0; y < this->height; ++y){
        for(unsigned x = 0; x < this->width; ++x){
            unsigned i = x + y * this->width;
            auto &p = this->pixels[i];
            if(p.a != 0){
                min_x = std::min(min_x, x);
                min_y = std::min(min_y, y);		
                max_x = std::max(max_x, x);
                max_y = std::max(max_y, y);						
            }
        }
    }
    
    unsigned w = (max_x - min_x) + 1;
    unsigned h = (max_y - min_y) + 1;
    return DNK::Rect<unsigned>(min_x, min_y, w, h);
}

void DNK::Bitmap::clear(){
    this->pixels.clear();
    this->width = 0;
    this->height = 0;   
}

DNK::Bitmap &DNK::Bitmap::build(const DNK::Color &p, unsigned format, unsigned width, unsigned height){
    this->width = width;
    this->height = height;
    this->format = format;
    this->channels = format == DNK::ImageFormat::RGBA ? 4 : 3;
    this->pixels.resize(width * height);
    for(unsigned i = 0; i < this->width * this->height; ++i){
        this->pixels[i] = p;
    }
    return *this;
}

std::vector<std::vector<DNK::Rect<unsigned>>> DNK::Bitmap::findBoxes(){

    auto get_box = [&](unsigned fx, unsigned fy, unsigned sw, unsigned sh){
        unsigned min_x = sw;
        unsigned min_y = sh;
        unsigned max_x = 0;
        unsigned max_y = 0;
        
        
        auto is_row_emty = [&](unsigned y){
            for(unsigned x = 0; x < sw; ++x){
                auto &p = this->pixels[(x+fx) + (y+fy) * this->width];
                if(p.a != 0){
                    return false;
                }
            }
            return true;
        };				
        
        for(unsigned y = 0; y < sh; ++y){
            for(unsigned x = 0; x < sw; ++x){
                if(is_row_emty(y)){
                    continue;
                }
                auto &p = this->pixels[(x+fx) + (y+fy) * this->width];
                if(p.a != 0){
                    min_x = std::min(min_x, x);
                    min_y = std::min(min_y, y);		
                    max_x = std::max(max_x, x);
                    max_y = std::max(max_y, y);		
                }
            }
        }
        
        
        min_x += fx;
        min_y += fy;
        max_x += fx;
        max_y += fy;			
        
        
        int w = (max_x - min_x) + 1;
        int h = (max_y - min_y) + 1;
        
        return DNK::Rect<unsigned>(min_x, min_y, w, h);			
    };
    
    auto is_row_emty = [&](unsigned y){
        for(unsigned x = 0; x < this->width; ++x){
            auto &p = this->pixels[x + y * this->width];
            if(p.a != 0){
                return false;
            }
        }
        return true;
    };		
    
    
    auto get_line = [&](unsigned stX, unsigned stY, unsigned height){
        std::vector<DNK::Rect<unsigned>> result;
        
        unsigned cursor_x = stX;
        unsigned cursor_dx = stX;
        
        auto is_column_emty = [&](unsigned x){
            for(unsigned y = stY; y < stY+height; ++y){
                auto &p = this->pixels[x + y * this->width];
                if(p.a != 0){
                    return false;
                }
            }
            return true;
        };			

        while(cursor_x+1 < this->width){
            bool is_empty = true;
            for(unsigned x = cursor_x; x < this->width; ++x){
                if(!is_column_emty(x)){
                    cursor_x = x;
                    is_empty = false;
                    break;
                }
            }
            if(is_empty){
                break;
            }
            cursor_dx = cursor_x+1;
            for(unsigned x = cursor_dx; x < this->width; ++x){
                if(is_column_emty(x)){
                    cursor_dx = x;
                    break;
                }
            }				
            
            //std::cout << "CURSOR " << cursor_x << " " << cursor_dx << " " << cursor_dx-cursor_x << std::endl;				
            auto b = get_box(cursor_x, stY, cursor_dx-cursor_x, height);
            //std::cout << "BOX " << b.x << " " << b.y << " " << b.w << " " << b.h << std::endl;			
            //std::cout << std::endl;
            result.push_back(b);
            cursor_x = cursor_dx+1;
            
                    
        }
        //std::cout << result.size() << std::endl;
        return result;
    };
    

    
    unsigned cursor_y = 0;
    unsigned cursor_dy = 0;
    std::vector<std::vector<DNK::Rect<unsigned>>> out;
    
    while(cursor_y+1 < this->height){
        bool is_empty = true;
        for(unsigned y = cursor_y; y < this->height; ++y){
            if(!is_row_emty(y)){
                cursor_y = y;
                is_empty = false;
                break;
            }
        }
        if(is_empty){
            break;
        }

        cursor_dy = cursor_y + 1;
        
        for(unsigned y = cursor_dy; y < this->height; ++y){
            if(is_row_emty(y)){
                cursor_dy = y;
                break;
            }
        }

        //std::cout << cursor_y << " " << cursor_dy << "\theight: " << cursor_dy-cursor_y << std::endl;
        auto line = get_line(0, cursor_y, cursor_dy-cursor_y);
        //std::cout << line.size() << std::endl;
        out.push_back(line);
        cursor_y = cursor_dy+1;			
    }
    
    return out;
}


bool DNK::Bitmap::load(int w, int h, int chan, unsigned char* src){
    this->width = w;
    this->height = h;
    
    this->format = chan == 4 ? ImageFormat::RGBA : ImageFormat::RGB;
    this->channels = chan;
    this->pixels.resize(width * height);
    
    for(unsigned i = 0; i < width * height; ++i){
        auto &pixel = this->pixels[i];
        pixel.r = src[i * 4 + 0] / 255.0f;
        pixel.g = src[i * 4 + 1] / 255.0f;
        pixel.b = src[i * 4 + 2] / 255.0f;
        pixel.a = src[i * 4 + 3] / 255.0f;
    }
    
    return true;
}

bool DNK::Bitmap::load(const std::string &path){
    int width, height, chan;
    
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &chan, 0); 
    
    if(!data){
        return false;
    }		
    
    this->width = width;
    this->height = height;
    
    this->format = chan == 4 ? ImageFormat::RGBA : ImageFormat::RGB;
    this->channels = chan;
    this->pixels.resize(width * height);
    
    unsigned total = width * height * chan;
    for(unsigned i = 0; i < width * height; ++i){
        auto &pixel = this->pixels[i];
        pixel.r = data[i * 4 + 0] / 255.0f;
        pixel.g = data[i * 4 + 1] / 255.0f;
        pixel.b = data[i * 4 + 2] / 255.0f;
        pixel.a = data[i * 4 + 3] / 255.0f;
    }
    
    stbi_image_free(data);
    
    return true;
}

bool DNK::Bitmap::write(const std::string &path){
    unsigned char *data = new unsigned char[width * height * this->channels];

    for(unsigned i = 0; i < this->width * this->height; ++i){
        auto &pixel = this->pixels[i];
        data[i * 4 + 0] = pixel.r * 255.0f;
        data[i * 4 + 1] = pixel.g * 255.0f;
        data[i * 4 + 2] = pixel.b * 255.0f;
        data[i * 4 + 3] = pixel.a * 255.0f;
    }	

    stbi_write_png(path.c_str(), this->width, this->height, 4, data, this->width * this->channels);
    
    delete data;
    
    return true;
}