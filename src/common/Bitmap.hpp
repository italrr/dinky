#ifndef DNK_BITMAP_HPP
    #define DNK_BITMAP_HPP

    #include "./Types.hpp"

    namespace DNK {


        namespace ImageFormat {
            enum ImageFormat : unsigned {
                RED, // 8 bits
                GREEN, // 8 bits
                BLUE, // 8 bits
                RG, // 16 bit
                RGB, // 24 bits
                RGBA, // 32 bits
            };
            static unsigned getChannels(unsigned format){
                switch(format){
                    default:
                    case ImageFormat::RED:
                    case ImageFormat::GREEN:
                    case ImageFormat::BLUE: {
                        return 1;
                    }; 
                    case ImageFormat::RG: {
                        return 2;
                    };     
                    case ImageFormat::RGB: {
                        return 3;
                    };
                    case ImageFormat::RGBA: {
                        return 4;
                    };                                                                                 
                }                    
            }
        }     

        struct Bitmap {
            std::vector<DNK::Color> pixels;
            unsigned width;
            unsigned height;
            unsigned format;
            unsigned channels;
            
            Bitmap sub(const DNK::Rect<unsigned> &box);
            Bitmap sub(unsigned x, unsigned y, unsigned width, unsigned height);
            Bitmap rotate(float angle);

            void paste(Bitmap *src, unsigned x, unsigned y, bool alphaBlend);
            void pasteAndShade(Bitmap *src, unsigned x, unsigned y, const DNK::Color &c);

            void resize(unsigned nwidth, unsigned nheight);
            void shade(const DNK::Color &color);
            void clear();
            void fill(DNK::Color &c);
            std::vector<uint8> getFlatArray() const;
            Bitmap copy();
            DNK::Rect<unsigned> autocrop();
            Bitmap &build(const DNK::Color &p, unsigned format, unsigned width, unsigned height);
            std::vector<std::vector<DNK::Rect<unsigned>>> findBoxes();        
            bool load(const std::string &path);
            bool load(int w, int h, int chan, unsigned char* src);
            bool write(const std::string &path);
        };

    }

#endif
