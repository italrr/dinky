#include <cstdlib>
#include <chrono>
#include <thread>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <limits.h>
#include <random>
#include <cmath>
#include <mutex>
#include <fstream>

#include "Tools.hpp"

/*
	TIME
*/

uint64 DNK::ticks(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64 DNK::epoch(){
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void DNK::sleep(uint64 t){
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}


/* 
    CORE
*/

namespace DNK {
    namespace Core {
            void exit(int code){
                std::exit(code);
            }
            std::vector<std::string> loadParams(int argc, char* argv[]){
                std::vector<std::string> params;
                for(int i = 0; i < argc; ++i){
                    params.push_back(std::string(argv[i]));
                }            
                return params;    
            }
            DNK::Core::ParamPair getParam(const std::vector<std::string> &params, const std::string &key, bool single){
                for(int i = 0; i < params.size(); ++i){
                    if(params[i] == key){
                        return single ? DNK::Core::ParamPair(key) : (i < params.size()-1 ? DNK::Core::ParamPair(key, params[i+1]) : DNK::Core::ParamPair());
                    }
                }
                return DNK::Core::ParamPair();
            }
    }
}

/* 
	FILE
*/
namespace DNK {
	namespace File {
		
		uint64 lastModifiedTime(const std::string &path){
			if(!DNK::File::exists(path)){
				return 0;
			}
			struct stat file_stat;
			int err = stat(path.c_str(), &file_stat);
			if (err != 0) {
				// error
				std::exit(errno);
			}
			return static_cast<uint64>(file_stat.st_mtime);			
		}

        std::string getCwd(){
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            return std::string(cwd);
        }
		
		static int useDirSep = Core::PLATFORM;

		std::string readTextFile(const std::string &path){
			std::ifstream file(path, std::ios::binary);
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			char *buffer = new char[size];
			memset(buffer, 0, size);
			file.seekg(0);
			file.read(buffer, size); 
			std::string text(buffer, size);
			delete buffer;
			return text;
		}

		void setDirStep(int platform){
			useDirSep = platform;
		}

		std::string dirSep(){
			return useDirSep == Core::SupportedPlatform::LINUX ? "/" : "\\";
		}

        std::string fixPath(const std::string &path){
            auto result = path;
            if(Core::PLATFORM == Core::SupportedPlatform::WINDOWS){
                return DNK::String::replaceAll(path, "/", "\\");                
            }
            return path;
        }

		bool exists(const std::string &path){
			struct stat tt;
			stat(path.c_str(), &tt);
			return S_ISREG(tt.st_mode);		
		}

		size_t size(const std::string &path){
			if (!exists(path)){
				return 0;
			}
			struct stat tt;
			stat(path.c_str(), &tt);
			return tt.st_size;			
		}

		std::string format(const std::string &filename){
			int k = filename.rfind(".");
			return k != -1 ? filename.substr(k+1, filename.length()-1) : "";
		}

		std::string filename(const std::string &path){
			int k = path.rfind(File::dirSep());
			return k != -1 ? path.substr(k+1, path.length()-1) : path;			
		}

		std::string filenameNoFormat(const std::string &path){
			int k = path.rfind(File::dirSep());
			auto fn =  k != -1 ? path.substr(k+1, path.length()-1) : path;				
			if(fn.rfind(".") != -1){
				fn = fn.substr(0, fn.find("."));
			}
			return fn;
		}

		std::string path(const std::string &p){
			int k = p.rfind(File::dirSep());
			return k != -1 ? p.substr(0, k) : p;			
		}		

	}
}

/*
	STRING
*/
std::string DNK::String::lower(const std::string &str){
	std::string out;
	for(int i = 0; i < str.size(); i++){
		out += tolower(str.at(i));
	}
	return out;
}

std::string DNK::String::upper(const std::string &str){
	std::string out;
	for(int i = 0; i < str.size(); i++){
		out += toupper(str.at(i));
	}
	return out;
}

std::vector<std::string> DNK::String::split(const std::string &str, const char sep){
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = str.find(sep, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(str.substr(start));
  return tokens;
}

std::string DNK::String::format(const std::string &_str, ...){
	va_list arglist;
	char buff[1024];
	va_start(arglist, _str);
	vsnprintf(buff, sizeof(buff), _str.c_str(), arglist);
	va_end( arglist );
	return std::string(buff);
}

std::string DNK::String::formatByes(int n){
	int round = 0;
	float _n = n;
	while(_n > 1024){
		_n /= 1024;
		++round;
	}
	std::string size = "B";
	switch(round){
		case 1: 
			size = "KB";
			break;
		case 2: 
			size = "MB";
			break;
		case 3:
			size = "GB";
			break;
		case 4:
			size = "TB";									
			break;
	}
	return String::format("%.2f %s", _n, size.c_str());
}

std::string DNK::String::str(float n){
	return std::to_string(n);
}

std::string DNK::String::str(double n){
	return std::to_string(n);
}

std::string DNK::String::str(uint8 n){
	return std::to_string(n);
}

std::string DNK::String::str(int8 n){
	return std::to_string(n);
}

std::string DNK::String::str(uint16 n){
	return std::to_string(n);
}

std::string DNK::String::str(int16 n){
	return std::to_string(n);
}

std::string DNK::String::str(uint32 n){
	return std::to_string(n);
}

std::string DNK::String::str(int32 n){
	return std::to_string(n);
}

std::string DNK::String::str(uint64 n){
	return std::to_string(n);
}

std::string DNK::String::str(int64 n){
	return std::to_string(n);
}

std::string DNK::String::replaceAll(std::string subject, const std::string& search, const std::string& replace){
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}


/* 
	MATH
*/
namespace DNK {
	namespace Math {

		float floor(float n){
			return std::floor(n);
		}
		
		double floor(double n){
			return std::floor(n);
		}
		
		float ceil(float n){
			return std::ceil(n);
		}

		double ceil(double n){
			return std::ceil(n);
		}

        float round(float n){
			return std::round(n);
		}
		double round(double n){
			return std::round(n);
		}
		int even(int n){
			return n % 2 != 0 ? n + 1 : n;
		}
		int odd(int n){
			return n % 2 == 0 ? n + 1 : n;
		}
		int random(int min, int max){
			// static std::random_device rd;
			static std::mt19937 rng(std::time(nullptr));
			std::uniform_int_distribution<int> uni(min,max);
			return uni(rng);			
		}
		float pow(float base, float exp){
			return std::pow(base, exp);
		}
		float sqrt(float n){
			return std::sqrt(n);
		}
		float sin(float n){
			return std::sin(n);
		}
		float cos(float n){
			return std::cos(n);
		}
		float tan(float n){
			return std::tan(n);
		}
		float asin(float n){
			return std::asin(n);
		}
		float acos(float n){
			return std::acos(n);
		}		
		float abs(float n){
			return std::abs(n);
		}
		float atan(float y, float x){
			return std::atan2(y, x);
		}
		float rads(float deg){
			return deg * 0.01745329251994329576923690768489;
		}
		float clamp(float num, float min, float max){
			return DNK::Math::min(DNK::Math::max(num, min), max);
		}
		float min(const std::vector<float> &n){
			if(n.size() == 0){
				return 0.0f;
			}
			float m = n[0];
			for(int i = 1; i < n.size(); ++i){
				m = DNK::Math::min(n[i], m);
			}
			return m;
		}
		float max(const std::vector<float> &n){
			if(n.size() == 0){
				return 0.0f;
			}
			float m = n[0];
			for(int i = 1; i < n.size(); ++i){
				m = DNK::Math::max(n[i], m);
			}
			return m;
		}		
		float min(float a, float b){
			return std::min(a, b);
		}
		float max(float a, float b){
			return std::max(a, b);
		}		
		float degs(float x){
			// return (rads/PI*180) + (rads > 0 ? 0 : 360);	
			return (x > 0 ? x : (2*DNK::Math::PI + x)) * 360 / (2*DNK::Math::PI);
		}
		DNK::Vec3<float> getPointPlaneIntersect(const DNK::Vec3<float> &p1, const DNK::Vec3<float> &p2, float z){
			auto diff = (p2.z - p1.z);
			if(diff == 0.0f){
				return DNK::Vec3<float>();
			}
			float t = (z - p1.z) / diff;
			float x = (1 - t) * p1.x + t * p2.x;
			float y = (1 - t) * p1.y + t * p2.y;
			return DNK::Vec3<float>(x, y, z);			
		}

		bool isPointWithinRegion(const DNK::Vec2<int> &point, const DNK::Vec2<int> &pos, const DNK::Vec2<int> &size){
			bool hor = point.x >= pos.x && point.x <= (pos.x + size.x);
			bool vert = point.y >= pos.y && point.y <= (pos.y + size.y);
			return hor && vert;
		}
		
		float lerpNonDamped(float a, float b, float t){
			return a + (b-a) * t;
		}

		float lerpUnit(float x1, float x2, float step, float delta, float limit){
			return DNK::Math::lerp(x1 * 100.0f, x2 * 100.0f, step, delta * 100.0f, limit) / 100.0f;                                			
		}

		float lerp(float x1, float x2, float step, float delta, float limit){
			if(x1 == x2 || DNK::Math::abs(x2-x1) < limit) return x2;	
			if(step > 1.0 || step <= 0.0){
				step = 1.0;
			}	
			const float f = (1.0 - std::pow(1.0 - step, delta));
			x1 = (x1 * (1.0 - f)) + (x2 * f);	
                return x1;       
		}

		// Stupid implementation made by me
        float angleStepInterp(float s, float t, float step){
			// address angles over pi*2
			if(t > DNK::Math::PI*2.0f){
				t = std::fmod(t, DNK::Math::PI*2.0f);
			}
			if(t < -DNK::Math::PI*2.0f){
				t = std::fmod(t, -DNK::Math::PI*2.0f);
			}	

			if(s-t > DNK::Math::PI){
				s += step;
				if(s > DNK::Math::PI*2.0f){
					s = std::fmod(s, DNK::Math::PI*2.0f);
				}
			}else
			if(s-t < -DNK::Math::PI){
				s -= step;
				if(s < 0){
					s = DNK::Math::PI*2.0f;
				}
			}else
			if(s < t){
				if(s+step > t){
					s = t;
				}else{
					s += step;
				}
			}else
			if(s > t){
				if(s-step < t){
					s = t;
				}else{
					s -= step;
				}
			}        
			return s;
			// Stolen from https://stackoverflow.com/a/67219519
			// auto CS = (1.0f-w)*DNK::Math::cos(A) + w*DNK::Math::cos(B);
			// auto SN = (1.0f-w)*DNK::Math::sin(A) + w*DNK::Math::sin(B);
			// return DNK::Math::atan(SN,CS);			
		}
	}
}