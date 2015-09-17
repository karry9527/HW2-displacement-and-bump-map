#ifndef _VECTOR3
#define _VECTOR3
#include <math.h>

// ����������
struct TexCoord
{
	float u,v;
};
// ��ɫ��
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

//ʸ����
class CVector3 {
public:

  double x, y, z;   // ��άʸ��


  // ȱʡ���캯�� & �û����캯��
  inline CVector3(void) {};
  inline CVector3(double vx, double vy, double vz) {
    x = vx; 
    y = vy; 
    z = vz; 
  };

  // Setʸ��ֵ
  inline void set(double vx, double vy, double vz) {
    x = vx; 
    y = vy; 
    z = vz; 
  };

  // ��ģ(ʸ������)
  inline double abs() {
    return sqrt(x * x + y * y + z * z); 
  };

  // ��ģƽ��
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

  // ʸ��'+'��
  inline CVector3 operator+(CVector3& v) {
    CVector3 a; 
    a.x = v.x + x; 
    a.y = v.y + y; 
    a.z = v.z + z; 
    return a; 
  };

  // ʸ��'+='��
  inline CVector3& operator+=(CVector3& v) {
    x += v.x; 
    y += v.y; 
    z += v.z; 
    return *this; 
  };

  // ʸ��'-'��
  inline CVector3 operator-(CVector3& v) { 
    CVector3 a; 
    a.x = x - v.x; 
    a.y = y - v.y; 
    a.z = z - v.z; 
    return a; 
  };

  // ʸ��'-='��
  inline CVector3& operator-=(CVector3& v) {
    x -= v.x; 
    y -= v.y; 
    z -= v.z; 
    return *this; 
  };

  // ʸ��'*'��
  inline CVector3 operator*(double c) { 
    return CVector3(x * c, y * c, z * c); 
  };

  // ʸ��'*='��
  inline CVector3& operator*=(double c) {
    x *= c; 
    y *= c; 
    z *= c; 
    return *this; 
  };

  // ʸ��'/'��
  inline CVector3 operator/(double c) { 
    CVector3 a; 
    a.x = x / c; 
    a.y = y / c;
    a.z = z / c;
    return a; 
  };

  // ʸ��'/='��
  inline CVector3& operator/=(double c) { 
    x /= c; 
    y /= c; 
    z /= c; 
    return *this; 
  };

  // ��ʸ��vec2���
  inline double DotProd(CVector3& vec2) { 
    return x * vec2.x + y * vec2.y + z * vec2.z; 
  };


  // ��ʸ��vec2��ˣ�����������ʸ������ֱ��ʸ��
  inline CVector3 CrossProd(CVector3& vec2) {
    CVector3 rNormal;
    rNormal.x = y * vec2.z - z * vec2.y;
    rNormal.y = z * vec2.x - x * vec2.z;
    rNormal.z = x * vec2.y - y * vec2.x;
    return rNormal; 
  };

  // ��λ��
  inline CVector3 norm() {
	double v = abs();
	return CVector3(x / v,  y / v,  z / v);
  };

  // ��λ��
  inline CVector3& normalize() {
	double v = abs();
	x /= v;
	y /= v;
	z /= v;
	return *this;
  };
};

#endif

