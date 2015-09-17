#ifndef _VECTOR3
#define _VECTOR3
#include <math.h>

// 纹理坐标类
struct TexCoord
{
	float u,v;
};
// 颜色类
struct Color
{
	float r,g,b,alpha;
	void set(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	void set(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->alpha = a;
	}
};

//矢量类
class CVector3 {
public:

  double x, y, z;   // 三维矢量


  // 缺省构造函数 & 用户构造函数
  inline CVector3(void) {};
  inline CVector3(double vx, double vy, double vz) {
    x = vx; 
    y = vy; 
    z = vz; 
  };

  // Set矢量值
  inline void set(double vx, double vy, double vz) {
    x = vx; 
    y = vy; 
    z = vz; 
  };

  // 求模(矢量长度)
  inline double abs() {
    return sqrt(x * x + y * y + z * z); 
  };

  // 求模平方
  inline double abs2() {
    return x * x + y * y + z * z; 
  };

 inline bool operator ==(CVector3& v)
{
  if(x == v.x && y == v.y && z == v.z)    
      return true;
  else 
	  return false;          
}

  // 矢量'+'法
  inline CVector3 operator+(CVector3& v) {
    CVector3 a; 
    a.x = v.x + x; 
    a.y = v.y + y; 
    a.z = v.z + z; 
    return a; 
  };

  // 矢量'+='法
  inline CVector3& operator+=(CVector3& v) {
    x += v.x; 
    y += v.y; 
    z += v.z; 
    return *this; 
  };

  // 矢量'-'法
  inline CVector3 operator-(CVector3& v) { 
    CVector3 a; 
    a.x = x - v.x; 
    a.y = y - v.y; 
    a.z = z - v.z; 
    return a; 
  };

  // 矢量'-='法
  inline CVector3& operator-=(CVector3& v) {
    x -= v.x; 
    y -= v.y; 
    z -= v.z; 
    return *this; 
  };

  // 矢量'*'法
  inline CVector3 operator*(double c) { 
    return CVector3(x * c, y * c, z * c); 
  };

  // 矢量'*='法
  inline CVector3& operator*=(double c) {
    x *= c; 
    y *= c; 
    z *= c; 
    return *this; 
  };

  // 矢量'/'法
  inline CVector3 operator/(double c) { 
    CVector3 a; 
    a.x = x / c; 
    a.y = y / c;
    a.z = z / c;
    return a; 
  };

  // 矢量'/='法
  inline CVector3& operator/=(double c) { 
    x /= c; 
    y /= c; 
    z /= c; 
    return *this; 
  };

  // 与矢量vec2点乘
  inline double DotProd(CVector3& vec2) { 
    return x * vec2.x + y * vec2.y + z * vec2.z; 
  };


  // 与矢量vec2叉乘，即求与两个矢量都垂直的矢量
  inline CVector3 CrossProd(CVector3& vec2) {
    CVector3 rNormal;
    rNormal.x = y * vec2.z - z * vec2.y;
    rNormal.y = z * vec2.x - x * vec2.z;
    rNormal.z = x * vec2.y - y * vec2.x;
    return rNormal; 
  };

  // 单位化
  inline CVector3 norm() {
	double v = abs();
	return CVector3(x / v,  y / v,  z / v);
  };

  // 单位化
  inline CVector3& normalize() {
	double v = abs();
	x /= v;
	y /= v;
	z /= v;
	return *this;
  };
};

#endif

