// 3DS.h: interface for the 3DS class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _3DS_H
#define _3DS_H

#include <fstream>
#include <vector>
#include <olectl.h>              
#include <math.h> 

#include "Vector3.h"              

using namespace std;



// ������(Primary Chunk)��λ���ļ��Ŀ�ʼ
#define PRIMARY            0x4D4D

// ����(Main Chunks)
#define OBJECTINFO         0x3D3D        // ���������Ϣ
#define VERSION            0x0002        // .3ds�ļ��İ汾
#define EDITKEYFRAME       0xB000        // ���йؼ�֡��Ϣ��ͷ��

// ����Ĵμ�����(��������Ĳ��ʺͶ���
#define MATERIAL           0xAFFF        // ����������Ϣ
#define OBJECT             0x4000        // ���������桢�������Ϣ

// ���ʵĴμ�����
#define MATNAME            0xA000        // �����������
#define MATDIFFUSE         0xA020        // ����/���ʵ���ɫ
#define MATMAP             0xA200        // �²��ʵ�ͷ��
#define MATMAPFILE         0xA300        // ����������ļ���

#define OBJECT_MESH        0x4100        // �µ��������

// OBJECT_MESH�Ĵμ�����
#define OBJECT_VERTICES    0x4110      // ���󶥵�
#define OBJECT_FACES       0x4120      // �������
#define OBJECT_MATERIAL    0x4130      // ����Ĳ���
#define OBJECT_UV          0x4140      // �����UV��������

#define MAX_TEXTURES 100                // ����������Ŀ

// ����3D����࣬���ڱ���ģ���еĶ���
class CVector3D 
{
public:
  float x, y, z;
};

// ����2D���࣬���ڱ���ģ�͵�UV��������
class CVector2D 
{
public:
  float x, y;
};

// ��Ľṹ����
struct tFace
{
public:
  int vertIndex[3];      // ��������
  int coordIndex[3];      // ������������
};



// ������Ϣ�ṹ��
struct tMaterialInfo
{
  char strName[255];      // ��������
  char strFile[255];      // �����������ӳ�䣬���ʾ�����ļ�����
  BYTE color[3];        // �����RGB��ɫ
  int texureId;        // ����ID
  float uTile;        // u �ظ�
  float vTile;        // v �ظ�
  float uOffset;       // u ����ƫ��
  float vOffset;        // v ����ƫ��
} ;

// ������Ϣ�ṹ��
struct t3DObject 
{
  int numOfVerts;      // ģ���ж������Ŀ
  int numOfFaces;      // ģ���������Ŀ
  int numTexVertex;      // ģ���������������Ŀ
  int materialID;      // ����ID
  bool bHasTexture;      // �Ƿ��������ӳ��
  char strName[255];      // ���������
  CVector3D *pVerts;      // ����Ķ���
  CVector3D *pNormals;    // ����ķ�����
  CVector2D *pTexVerts;    // ����UV����
  tFace *pFaces;        // ���������Ϣ
};

// ģ����Ϣ�ṹ��
struct t3DModel 
{
  UINT texture[MAX_TEXTURES];
  bool  Textured;           //�Ƿ�ʹ������
  int numOfObjects;          // ģ���ж������Ŀ
  int numOfMaterials;          // ģ���в��ʵ���Ŀ
  vector<tMaterialInfo> pMaterials;  // ����������Ϣ
  vector<t3DObject> pObject;      // ģ���ж���������Ϣ
};



// �������Ϣ�Ľṹ
struct tChunk
{
  unsigned short int ID;          // ���ID    
  unsigned int length;          // ��ĳ���
  unsigned int bytesRead;          // ��Ҫ���Ŀ����ݵ��ֽ���
};


// CLoad3DS�ദ�����е�װ�����
class CLoad3DS 
{
public:
  CLoad3DS();                // ��ʼ�����ݳ�Ա
  virtual ~CLoad3DS();
  //װ��ģ��ʵ��

  // װ��3ds�ļ���ģ�ͽṹ��   
  bool ImportModel(GLuint Model_id, char *strFileName);
  //��Ⱦ
  void RenderModel();

    void SetModelPosition(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z)
	{
		this->ModelPosition.x = pos_x;
		this->ModelPosition.y = pos_y;				
		this->ModelPosition.z = pos_z;
	}
	void SetModelRotation(GLfloat rot_x, GLfloat rot_y, GLfloat rot_z)
	{
		this->ModelRotation.x = rot_x;
		this->ModelRotation.y = rot_y;
		this->ModelRotation.z = rot_z;
	}
	void SetModelPosition(CVector3 pos3){ModelPosition = pos3;}
	void SetModelRotation(CVector3 rot3){ModelRotation = rot3;}
    void SetModelSize(GLfloat size){this->ModelSize = size;}



	CVector3 GetPos() const{ return ModelPosition; }
	CVector3 GetRot() const{ return ModelRotation; }
	GLfloat GetSize() const{ return ModelSize; }
    t3DModel& GetModel() {return Model3DS;}

private:
  //����ģ��
  void DrawModel();
  // ����һ������
  int BuildTexture(char *szPathName, GLuint &texid);
  // ��һ���ַ���
  int GetString(char *);
  // ����һ����
  void ReadChunk(tChunk *);
  // ����һ����
  void ProcessNextChunk(t3DModel *pModel, tChunk *);
  // ����һ�������
  void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);
  // ����һ�����ʿ�
  void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);
  // ��������ɫ��RGBֵ
  void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);
  // ������Ķ���
  void ReadVertices(t3DObject *pObject, tChunk *);
  // �����������Ϣ
  void ReadVertexIndices(t3DObject *pObject, tChunk *);
  // ���������������
  void ReadUVCoordinates(t3DObject *pObject, tChunk *);
  // ���������Ĳ�������
  void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);
  // ������󶥵�ķ�����
  void ComputeNormals(t3DModel *pModel);
  // �ر��ļ����ͷ��ڴ�ռ�
  void CleanUp();


  GLuint Model_ID;
  FILE *m_FilePointer;
  t3DModel Model3DS;

  CVector3 ModelPosition;
  CVector3 ModelRotation;
  GLfloat ModelSize;

  static GLuint CountModels;

  tChunk *m_CurrentChunk;
  tChunk *m_TempChunk;


};

#endif

