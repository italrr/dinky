#ifndef DNK_TYPES_HPP
    #define DNK_TYPES_HPP

    #include <string>
    #include <vector>
	#include <set>
    #include <memory>
	#include <mutex>
	#include <sys/types.h>
    
	#define int8 int8_t
	#define uint8 uint8_t
	#define int16 int16_t
	#define uint16 uint16_t
	#define int32 int32_t
	#define uint32 uint32_t
	#define int64 int64_t
	#define uint64 uint64_t   

	// #define PRIu8 "hu"
	// #define PRId8 "hd"
	// #define PRIx8 "hx"
	// #define PRIu16 "hu"
	// #define PRId16 "hd"	

    namespace DNK {

		namespace Math {
			float sqrt(float n);
			float sin(float n);
			float cos(float n);
		}

        template<typename T>
        struct Vec2 {
            T x, y;
            
            Vec2(T x, T y){
                set(x, y);
            }
            
            Vec2(T c){
                set(c);
            }

			Vec2(const DNK::Vec2<T> &vec){
				set(vec);
			}

            Vec2(){
                set(0);
            }
            
			T operator[](std::size_t idx) const {
				switch(idx){
					case 1:{ return y; };
					default:
					case 0:{ return x; };					
				}					
			}

			T& operator[](std::size_t idx) {
				switch(idx){
					case 1:{ return y; };
					default:
					case 0:{ return x; };					
				}					
			}

            void set(T x, T y){
                this->x = x; this->y = y;
            }

            void set(T c){
                this->x = c;
                this->y = c;
            }

            void set(const DNK::Vec2<T> &vec){
				this->x = vec.x; this->y = vec.y;
            }

			DNK::Vec2<T> operator*(const DNK::Vec2<T> &vec) const{
				return DNK::Vec2<T>(this->x * vec.x, this->y * vec.y);
			}

			DNK::Vec2<T> operator/(const DNK::Vec2<T> &vec) const{
				return DNK::Vec2<T>(this->x / vec.x, this->y / vec.y);
			}

			DNK::Vec2<T> operator+(const DNK::Vec2<T> &vec) const{
				return DNK::Vec2<T>(this->x + vec.x, this->y + vec.y);
			}

			DNK::Vec2<T> operator-(const DNK::Vec2<T> &vec) const{
				return DNK::Vec2<T>(this->x - vec.x, this->y - vec.y);
			}			

			DNK::Vec2<T> normalize() const{
				DNK::Vec2<T> normalized = *this;
				T sqr = normalized.x * normalized.x + normalized.y * normalized.y;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				T invrt = 1.0f/DNK::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;		
				return normalized;
			}	

			T dot(const DNK::Vec2<T> &vec) const{
				T product = 0; 
				product += x * vec.x; 
				product += y * vec.y; 
				return product; 
			}

			DNK::Vec2<T> cross(const DNK::Vec2<T> &vec) const{
				return this->x * vec.y - this->y * vec.x;
			}

			DNK::Vec2<T> cross(T sc) {
				return DNK::Vec2<T>(sc * this->y, -sc * this->x);
			}	

			std::string str() const{
				return "["+std::to_string(this->x)+", "+std::to_string(this->y)+"]";
			}									
        };
        
        template<typename T>
        struct Vec3 {
            T x, y, z;
            
            Vec3(T x, T y, T z){
                set(x, y, z);
            }
            
            Vec3(T c){
                set(c);
            }
            
			Vec3(const DNK::Vec3<T> &vec){
				set(vec);
			}	

			Vec3(const DNK::Vec2<T> &vec){
				set(vec);
			}	

            Vec3(){
                set(0);
            }


			T& operator[](std::size_t idx) {
				switch(idx){
					case 1:{ return y; };
					case 2:{ return z; };
					default:
					case 0:{ return x; };					
				}					
			}

			T operator[](std::size_t idx) const {
				switch(idx){
					case 1:{ return y; };
					case 2:{ return z; };
					default:
					case 0:{ return x; };					
				}					
			}			
            

			void set(const DNK::Vec3<T> &vec){
				this->x = vec.x; this->y = vec.y; this->z = vec.z;
			}	

			void set(const DNK::Vec2<T> &vec){
				this->x = vec.x; this->y = vec.y; this->z = static_cast<T>(0);
			}				

            void set(T x, T y, T z){
                this->x = x; this->y = y; this->z = z;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->z = c;
            }

			DNK::Vec3<T> normalize() const {
				DNK::Vec3<T> normalized = *this;
				float sqr = normalized.dot(normalized);
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f / DNK::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}	

			DNK::Vec3<T> operator*(const DNK::Vec3<T> &vec) const{
				return DNK::Vec3<T>(this->x * vec.x, this->y * vec.y, this->z * vec.z);
			}

			DNK::Vec3<T> operator/(const DNK::Vec3<T> &vec) const{
				return DNK::Vec3<T>(this->x / vec.x, this->y / vec.y, this->z / vec.z);
			}

			DNK::Vec3<T> operator+(const DNK::Vec3<T> &vec) const{
				return DNK::Vec3<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z);
			}

			DNK::Vec3<T> operator-(const DNK::Vec3<T> &vec) const{
				return DNK::Vec3<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z);
			}						

			T dot(const DNK::Vec3<T> &vec) const{
				return this->x * vec.x + this->y * vec.y + this->z * vec.z;
			}

			DNK::Vec3<T> cross(const DNK::Vec3<T> &vec) const{
				DNK::Vec3<T> out;
				out.x = this->y * vec.z - vec.y * this->z;
				out.y = -(this->x * vec.z - vec.x * this->z);
				out.z = this->x * vec.y - vec.x * this->y;
				return out;
			}

			std::string str() const{
				return "["+std::to_string(this->x)+", "+std::to_string(this->y)+", "+std::to_string(this->z)+"]";
			}	

        };     


        template<typename T>
        struct Vec4 {
            T x, y, z, w;
            
            Vec4(T x, T y, T z, T w){
                set(x, y, z, w);
            }
            
            Vec4(T c){
                set(c);
            }
            
            Vec4(){
                set(0);
            }

			Vec4(const DNK::Vec4<T> &vec){
				set(vec);
			}

			Vec4(const DNK::Vec3<T> &vec){
				set(vec);
			}	


			Vec4(const DNK::Vec2<T> &vec){
				set(vec);
			}					


			T& operator[](std::size_t idx) {
				switch(idx){
					case 1:{ return y; };
					case 2:{ return z; };
					case 3:{ return w; };
					default:
					case 0:{ return x; };					
				}					
			}		

			T operator[](std::size_t idx) const {
				switch(idx){
					case 1:{ return y; };
					case 2:{ return z; };
					case 3:{ return w; };
					default:
					case 0:{ return x; };					
				}					
			}				

			void set(const DNK::Vec4<T> &vec){
				this->x = vec.x;
				this->y = vec.y;
				this->z = vec.z;
				this->w = vec.w;
			}

			void set(const DNK::Vec2<T> &vec){
				this->x = vec.x;
				this->y = vec.y;
				this->z = static_cast<T>(0);
				this->w = static_cast<T>(0);
			}

			void set(const DNK::Vec3<T> &vec){
				this->x = vec.x;
				this->y = vec.y;
				this->z = vec.z;
				this->w = static_cast<T>(0);
			}						
					
            
            void set(T x, T y, T z, T w){
                this->x = x; this->y = y; this->z = z; this->w = w;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->z = c;
				this->w = c;
            }

			DNK::Vec4<T> operator*(const DNK::Vec4<T> &vec) const{
				return DNK::Vec4<T>(this->x * vec.x, this->y * vec.y, this->z * vec.z, this->w * vec.w);
			}

			DNK::Vec4<T> operator/(const DNK::Vec4<T> &vec) const{
				return DNK::Vec4<T>(this->x / vec.x, this->y / vec.y, this->z / vec.z, this->w / vec.w);
			}

			DNK::Vec4<T> operator+(const DNK::Vec4<T> &vec) const{
				return DNK::Vec4<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z, this->w + vec.w);
			}

			DNK::Vec4<T> operator-(const DNK::Vec4<T> &vec) const{
				return DNK::Vec4<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z, this->w - vec.w);
			}									

			DNK::Vec4<T> normalize() const{
				DNK::Vec4<T> normalized = *this;
				float sqr = normalized.x * normalized.x + normalized.y * normalized.y + normalized.z * normalized.z;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f/DNK::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.z *= invrt;				
				return normalized;
			}

			float dot(const DNK::Vec4<T> &vec) const{
				return this->x * vec.x + this->y * vec.y + this->z * vec.z + this->w * vec.w;
			}

			DNK::Vec4<T> cross(const DNK::Vec4<T> &vec) const{
				DNK::Vec4<T> out;
				out.x = this->y*vec.z - this->z*vec.y;
				out.y = this->z*vec.x - this->x*vec.z;
				out.z = this->x*vec.y - this->y*vec.x;
				return out;
			}

			std::string str() const{
				return "["+std::to_string(this->x)+", "+std::to_string(this->y)+", "+std::to_string(this->z)+", "+std::to_string(this->w)+"]";
			}			

        };  

        // copy pasted vec4
        template<typename T>
        struct Rect {
            T x, y, w, h;
            
            Rect(T x, T y, T w, T h){
                set(x, y, w, h);
            }
            
            Rect(T c){
                set(c);
            }
            
            Rect(){
                set(0);
            }

			Rect(const DNK::Rect<T> &vec){
				set(vec);
			}	

			T& operator[](std::size_t idx){
				switch(idx){
					case 1:{ return y; };
					case 2:{ return w; };
					case 3:{ return h; };
					default:
					case 0:{ return x; };					
				}					
			}				

			T operator[](std::size_t idx) const {
				switch(idx){
					case 1:{ return y; };
					case 2:{ return w; };
					case 3:{ return h; };
					default:
					case 0:{ return x; };					
				}					
			}				

			void set(const DNK::Rect<T> &vec){
				this->x = vec.x;
				this->y = vec.y;
				this->w = vec.w;
				this->h = vec.h;
			}					
            
            void set(T x, T y, T w, T h){
                this->x = x; this->y = y; this->w = w; this->h = h;
            }

            void set(T c){
                this->x = c;
                this->y = c;
                this->w = c;
				this->h = c;
            }

			DNK::Rect<T> operator*(const DNK::Rect<T> &vec) const{
				return DNK::Rect<T>(this->x * vec.x, this->y * vec.y, this->w * vec.w, this->h * vec.h);
			}

			DNK::Rect<T> operator/(const DNK::Rect<T> &vec) const{
				return DNK::Rect<T>(this->x / vec.x, this->y / vec.y, this->w / vec.w, this->h / vec.h);
			}

			DNK::Rect<T> operator+(const DNK::Rect<T> &vec) const{
				return DNK::Rect<T>(this->x + vec.x, this->y + vec.y, this->w + vec.w, this->h + vec.h);
			}

			DNK::Rect<T> operator-(const DNK::Rect<T> &vec) const{
				return DNK::Rect<T>(this->x - vec.x, this->y - vec.y, this->w - vec.w, this->h - vec.h);
			}									

			DNK::Rect<T> normalize() const{
				DNK::Rect<T> normalized = *this;
				float sqr = normalized.x * normalized.x + normalized.y * normalized.y + normalized.w * normalized.w;
				if(sqr == 1 || sqr == 0){
					return normalized;
				}
				float invrt = 1.0f/DNK::Math::sqrt(sqr);
				normalized.x *= invrt;
				normalized.y *= invrt;
				normalized.w *= invrt;				
				return normalized;
			}

			float dot(const DNK::Rect<T> &vec) const{
				return this->x * vec.x + this->y * vec.y + this->w * vec.w + this->h * vec.h;
			}

			DNK::Rect<T> cross(const DNK::Rect<T> &vec) const{
				DNK::Rect<T> out;
				out.x = this->y*vec.w - this->z*vec.y;
				out.y = this->z*vec.x - this->x*vec.w;
				out.z = this->x*vec.y - this->y*vec.x;
				return out;
			}

			std::string str() const{
				return "["+std::to_string(this->x)+", "+std::to_string(this->y)+", "+std::to_string(this->w)+", "+std::to_string(this->h)+"]";
			}				

        };         

		// In unit
		struct Color {
			void operator=(float c);
			bool operator<(const DNK::Color &v);
			bool operator>(const DNK::Color &v);
			bool operator<=(const DNK::Color &v);
			bool operator>=(const DNK::Color &v);
			bool operator<(float c);
			bool operator>(float c);
			bool operator<=(float c);
			bool operator>=(float c);
			bool operator==(float c);
			bool operator==(const DNK::Color &v);
			bool operator!=(float c);
			bool operator!=(const Color &v);
			DNK::Color operator/(float c);
			DNK::Color operator/(const DNK::Color &v);
			DNK::Color operator*(float c);
			DNK::Color operator*(const DNK::Color &v);
			DNK::Color operator+(float c);
			DNK::Color operator+(const DNK::Color &v);
			DNK::Color operator-(float c);
			DNK::Color operator-(const DNK::Color &v);
			DNK::Color invert();
			Color(const std::string &hex);
			Color();
			Color(const DNK::Color &color);
			Color(float r, float g, float b);
			Color(float r, float g, float b, float a);
			Color(float c);
			Color(float c, float a);
			void set(float c, float a);			
			void set(float c);
			void set(const std::string &hex);
			void set(float r, float g, float b, float a);
			void set(const DNK::Color &color);
			float r;
			float g;
			float b;
			float a;
			float& operator[](std::size_t idx);	
			DNK::Color shade(float factor);
			std::vector<float> toHSL();
			DNK::Color &fromHSL(const std::vector<float> &c);
			std::vector<float> rgb() const;
			operator std::string();
			std::string str();
			std::string hex();
		};

        namespace Colors {

            static DNK::Color Black(0.0f, 0.0f, 0.0f, 1.0f);
            static DNK::Color White(1.0f, 1.0f, 1.0f, 1.0f);
            
            static DNK::Color Red(1.0f, 0.0f, 0.0f, 1.0f);
            static DNK::Color Green(0.0f, 1.0f, 0.0f, 1.0f);
            static DNK::Color Blue(0.0f, 0.0f, 1.0f, 1.0f);

            static DNK::Color Purple(1.0f, 0.0f, 1.0f, 1.0f);
            static DNK::Color Cyan(0.0f, 1.0f, 1.0f, 1.0f);
            static DNK::Color Yellow(1.0f, 1.0f, 0.0f, 1.0f);

        }

    }


#endif    
