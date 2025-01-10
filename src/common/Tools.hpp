#ifndef DNK_TOOLS_HPP
    #define DNK_TOOLS_HPP

    #include "Types.hpp"
    #include "thirdparty/Jzon.hpp"

    namespace DNK {

        uint64 ticks();
        uint64 epoch();
        void sleep(uint64 t);

        namespace Core {

            struct ParamPair {
                std::string key;
                std::string value;
                bool isValid;
                ParamPair(){
                    isValid = false;
                }
                ParamPair(const std::string &key){
                    isValid = true;
                    this->key = key;
                }
                ParamPair(const std::string &key, const std::string &value){
                    this->key = key;
                    this->value = value;
                    isValid = true;
                }
            };


            namespace SupportedPlatform {
                enum SupportedPlatform : int {
                    // Desktop
                    WINDOWS,
                    LINUX,
                    OSX,
                    // Consoles
                    STEAM_DECK,
                    N_SWITCH,
                    S_PS5,
                    XBOX_ONEX,
                    PS_VITA,
                    UNSUPPORTED = -1
                };
                static std::string name(int plat){
                    switch(plat){
                        case SupportedPlatform::WINDOWS: 
                            return "WINDOWS";
                        case SupportedPlatform::LINUX:
                            return "LINUX";
                        default:
                            return "UNKNOWN";
                    }
                }
            }


            namespace SupportedArchitecture {
                enum SupportedArchitecture : int {
                    X86,
                    X86_64,
                    ARM,
                    ARM64,
                    UNKNOWN
                };
                static std::string name(int plat){
                    switch(plat){
                        case SupportedArchitecture::X86: 
                            return "X86";
                        case SupportedArchitecture::X86_64: 
                            return "X86_64";
                        case SupportedArchitecture::ARM: 
                            return "ARM";
                        case SupportedArchitecture::ARM64: 
                            return "ARM64";                                                                                    
                        default:
                            return "UNKNOWN";
                    }
                }
            }

            #if defined(__i386__) || defined(__i686__)
                const static int ARCH = SupportedArchitecture::X86;
            #elif defined(__x86_64__) || defined(__amd64__)
                const static int ARCH = SupportedArchitecture::X86_64;
            #elif defined(__arm__)
                const static int ARCH = SupportedArchitecture::ARM;
            #elif defined(__aarch64__)
                const static int ARCH = SupportedArchitecture::ARM64;
            #else
                const static int ARCH = SupportedArchitecture::UNKNOWN;
            #endif

            #ifdef _WIN32
                const static int PLATFORM = SupportedPlatform::WINDOWS;
            #elif __linux__
                const static int PLATFORM = SupportedPlatform::LINUX;
            #else
                const static int PLATFORM = SupportedPlatform::UNSUPPORTED;
            #endif    


            void exit(int code);
            std::vector<std::string> loadParams(int argc, char* argv[]);
            DNK::Core::ParamPair getParam(const std::vector<std::string> &params, const std::string &key, bool single = false);
        }

        namespace Date {
            
        }


        namespace Hash {
            std::string md5(const std::string &path, bool partial = false);
            std::string md5(char *data, size_t size, bool partial = false);
        }
        
        namespace File {
            namespace ListType {
                enum ListType : int {
                    File,
                    Directory,
                    Any
                }; 
            }
            std::string getCwd();
            std::string readTextFile(const std::string &path);
            void setDirStep(int platform);
            std::string dirSep();
            std::string fixPath(const std::string &path);
            bool exists(const std::string &path);
            std::string format(const std::string &filename);
            std::string filename(const std::string &path);
            std::string filenameNoFormat(const std::string &path);
            std::string path(const std::string &p);
            size_t size(const std::string &path);
            uint64 lastModifiedTime(const std::string &path);
        }

        namespace String {
            std::string lower(const std::string &str);
            std::string upper(const std::string &str);
            std::vector<std::string> split(const std::string &str, const char sep);
            std::string format(const std::string &str, ...);
            std::string formatByes(int n);
            std::string str(float n);
            std::string str(double n);
            std::string str(uint8 n);
            std::string str(int8 n);
            std::string str(uint16 n);
            std::string str(int16 n);
            std::string str(uint32 n);
            std::string str(int32 n);
            std::string str(uint64 n);
            std::string str(int64 n);    
            float number(const std::string &n);   
            std::string replaceAll(std::string subject, const std::string &search, const std::string &replace);
            bool isNumber(const std::string &s);
            bool isString(const std::string &s);             

            template<typename T>
            std::string str(const DNK::Vec2<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ")";
            }     

            template<typename T>
            std::string str(const DNK::Vec3<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ", " + str(static_cast<T>(vec.z)) + ")";
            }      

            template<typename T>
            std::string str(const DNK::Vec4<T> &vec){
                return "(" + str(static_cast<T>(vec.x)) + ", " + str(static_cast<T>(vec.y)) + ", " + str(static_cast<T>(vec.z)) + ", " + str(static_cast<T>(vec.w)) + ")";
            }                                   

        }


        namespace Math {


            static const float PI = 3.14159265358979323846;
            int random(int min, int max);
            float sqrt(float n);
            float pow(float base, float exp);
            float sin(float n);
            float cos(float n);
            float tan(float n);
            float asin(float n);
            float acos(float n);
            float atan(float y, float x);
            float rads(float deg);
            float abs(float n);
            float degs(float rads);
            float clamp(float num, float min, float max);
            float min(const std::vector<float> &n);
            float max(const std::vector<float> &n);
            float min(float a, float b);
            float max(float a, float b);
            int odd(int n);
            int even(int n);
            float round(float n);
            double round(double n);
            float floor(float n);
            double floor(double n);
            float ceil(float n);
            double ceil(double n);
            float lerpNonDamped(float a, float b, float t);

            float lerp(float x1, float x2, float step, float delta, float limit = 0.01f);
            float lerpUnit(float x1, float x2, float step, float delta, float limit = 0.0f);
            float angleStepInterp(float s, float t, float step);

            DNK::Vec3<float> getPointPlaneIntersect(const DNK::Vec3<float> &p1, const DNK::Vec3<float> &p2, float z);

            bool isPointWithinRegion(const DNK::Vec2<int> &point, const DNK::Vec2<int> &pos, const DNK::Vec2<int> &size);


            template<typename T>
            DNK::Vec4<T> slerp(DNK::Vec4<T> pStart, DNK::Vec4<T> pEnd, T pFactor){
                T cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

                DNK::Vec4<T> end = pEnd;
                if(cosom < static_cast<T>(0.0)){
                    cosom = -cosom;
                    end.x = -end.x;   
                    end.y = -end.y;
                    end.z = -end.z;
                    end.w = -end.w;
                }

                T sclp, sclq;
                if((static_cast<T>(1.0) - cosom) > static_cast<T>(0.0001)){ 
                    T omega, sinom;
                    omega = acos(cosom);
                    sinom = sin( omega);
                    sclp  = sin( (static_cast<T>(1.0) - pFactor) * omega) / sinom;
                    sclq  = sin( pFactor * omega) / sinom;
                }else{
                    sclp = static_cast<T>(1.0) - pFactor;
                    sclq = pFactor;
                }

                DNK::Vec4<T> out;

                out.x = sclp * pStart.x + sclq * end.x;
                out.y = sclp * pStart.y + sclq * end.y;
                out.z = sclp * pStart.z + sclq * end.z;
                out.w = sclp * pStart.w + sclq * end.w; 

                return out;               
            }
        }        

    }


#endif
