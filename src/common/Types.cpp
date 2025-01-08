#include "Types.hpp"
#include "Tools.hpp"

/*
================
color
================
*/
DNK::Color::Color(){
	r = 0;
	g	= 0;
	b	= 0;
	a	= 1.0f;

}

DNK::Color::Color(const DNK::Color &c){
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}


DNK::Color::Color(float r, float g, float b){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a	= 1.0f;

}

DNK::Color::Color(float r, float g, float b, float a){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

DNK::Color::Color(const std::string &hex){
	set(hex);
}

DNK::Color DNK::Color::shade(float factor){
	auto c = toHSL();
	c[2] *= factor;
	return DNK::Color().fromHSL(c);
}

DNK::Color &DNK::Color::fromHSL(const std::vector<float> &hsl){
	float r, g, b;

	if (hsl[1] == 0) {
		r = g = b = hsl[2]; // achromatic
	} else {
		static auto hue2rgb = [](float p, float q, float t) {
			if (t < 0) t += 1.0f;
			if (t > 1) t -= 1.0f;
			if (t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
			if (t < 1.0f/2.0f) return q;
			if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
			return p;
		};

		float q = hsl[2] < 0.5f ? hsl[2] * (1.0f + hsl[1]) : hsl[2] + hsl[1] - hsl[2] * hsl[1];
		float p = 2.0f * hsl[2] - q;

		r = hue2rgb(p, q, hsl[0] + 1.0f/3.0f);
		g = hue2rgb(p, q, hsl[0]);
		b = hue2rgb(p, q, hsl[0] - 1.0f/3.0f);
	}

	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0f;

	return *this;
}

std::vector<float> DNK::Color::toHSL(){

	auto orig = this->rgb();
	auto color = orig;

	color[0] /= 255.0f;
	color[1] /= 255.0f;
	color[2] /= 255.0f;

	auto max = DNK::Math::max(color);
	auto min = DNK::Math::min(color);

	float h, s;
	auto l = (max + min) / 2.0f;

	if (max == min) {
		h = s = 0;
	} else {
		auto d = max - min;
		s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
		if(max == orig[0]){
			h = (g - b) / d + (g < b ? 6.0f : 0);
		}else
		if(max == orig[1]){
			h = (b - r) / d + 2.0f;
		}
		if(max == orig[2]){
			h = (r - g) / d + 4.0f;
		}
		h /= 6.0f;
	}

	return std::vector<float>({ h, s, l });
}

std::vector<float> DNK::Color::rgb() const {
	return {
		DNK::Math::round(this->r * 255.0f),
		DNK::Math::round(this->g * 255.0f),
		DNK::Math::round(this->b * 255.0f),
		DNK::Math::round(this->a * 255.0f)
	};
}

void DNK::Color::set(const std::string &hex){
	char *p;
	int hexValue = strtol(hex.substr(1).c_str(), &p, 16);
    r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
    a = 1.0f;
}

std::string DNK::Color::hex(){
	int ri = r * 255.0f; int gi = g * 255.0f; int bi = b * 255.0f;
	int hex = ((ri & 0xff) << 16) + ((gi & 0xff) << 8) + (bi & 0xff); // loses alpha though
	char hexString[4*sizeof(int)+1];
	return std::string(hexString);
}

void DNK::Color::set(float r, float g, float b, float a){
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void DNK::Color::set(const DNK::Color &color){
	this->r = color.r;
	this->g	= color.g;
	this->b	= color.b;
	this->a	= color.a;
}

DNK::Color::Color(float c){
	set(c);
}

DNK::Color::Color(float c, float a){
	set(c, a);
}

void DNK::Color::set(float c, float a){
	this->r = c;
	this->g	= c;
	this->b	= c;
	this->a	= a;
}

void DNK::Color::set(float c){
	this->r = c;
	this->g	= c;
	this->b	= c;
	this->a	= c;	
}

float& DNK::Color::operator[](std::size_t idx){
	switch(idx){
		case 1:{ return g; };
		case 2:{ return b; };
		case 3:{ return a; };
		default:
		case 0:{ return r; };					
	}					
}	

DNK::Color::operator std::string(){
	return str();
}

std::string DNK::Color::str(){
    return DNK::String::format("(%f, %f, %f, %f)", r, g, b, a);
}

void DNK::Color::operator=(float c){
	r = c;
	g = c;
	b = c;
	a = c;
}

bool DNK::Color::operator<(const DNK::Color &v){
	return r < v.r && g < v.g && b < v.b && a < v.a;
}

bool DNK::Color::operator>(const DNK::Color &v){
	return r > v.r && g > v.g && b > v.b && a > v.a;
}

bool DNK::Color::operator<=(const DNK::Color &v){
	return r <= v.r && g <= v.g && b <= v.b && a <= v.a;
}

bool DNK::Color::operator>=(const DNK::Color &v){
	return r >= v.r && g >= v.g && b >= v.b && a >= v.a;
}

bool DNK::Color::operator<(float c){
	return r < c && g < c && b < c && a < c;
}

bool DNK::Color::operator>(float c){
	return r > c && g > c && b > c && a > c;
}

bool DNK::Color::operator<=(float c){
	return r <= c && g <= c && b <= c && a <= c;
}

bool DNK::Color::operator>=(float c){
	return r >= c && g >= c && b >= c && a >= c;
}

bool DNK::Color::operator==(float c){
	return r == c && g == c && b == c && a == c;
}

bool DNK::Color::operator==(const DNK::Color &v){
	return r == v.r && g == v.g && b == v.b && a == v.a;
}

bool DNK::Color::operator!=(float c){
	return r != c && g != c && b != c && a != c;
}

bool DNK::Color::operator!=(const Color &v){
	return r != v.r && g != v.g && b != v.b && a != v.a;
}

DNK::Color DNK::Color::operator/(float c){
	auto b = *this;
	b.r /= c;
	b.g /= c;
	b.b /= c;
	return b;
}

DNK::Color DNK::Color::operator/(const DNK::Color &v){
	auto b = *this;
	b.r /= v.r;
	b.g /= v.g;
	b.b /= v.b;
	return b;
}

DNK::Color DNK::Color::operator*(float c){
	auto b = *this;
	b.r *= c;
	b.g *= c;
	b.b *= c;
	return b;
}

DNK::Color DNK::Color::operator*(const DNK::Color &v){
	auto b = *this;
	b.r *= v.r;
	b.g *= v.g;
	b.b *= v.b;
	return b;
}

DNK::Color DNK::Color::operator+(float c){
	auto b = *this;
	b.r += c;
	b.g += c;
	b.b += c;
	return b;
}

DNK::Color DNK::Color::operator+(const DNK::Color &v){
	auto b = *this;
	b.r += v.r;
	b.g += v.g;
	b.b += v.b;
	return b;
}

DNK::Color DNK::Color::operator-(float c){
	auto b = *this;
	b.r -= c;
	b.g -= c;
	b.b -= c;
	return b;
}

DNK::Color DNK::Color::operator-(const DNK::Color &v){
	auto b = *this;
	b.r -= v.r;
	b.g -= v.g;
	b.b -= v.b;
	return b;
}

DNK::Color DNK::Color::invert(){
	auto col = DNK::Color();
	col.r = 1.0 - r;
	col.g = 1.0 - g;
	col.b = 1.0 - b;
	return col;
}