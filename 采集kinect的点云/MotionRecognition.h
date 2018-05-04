#pragma once
#include "Header.h"

#include <Kinect.h>
#include <opencv2\opencv.hpp>
using namespace cv;


// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


class CMotionRecognition
{

public:
	CMotionRecognition();

	~CMotionRecognition();

	// ��ʼ��Kinect
	HRESULT InitializeDefaultSensor();

	HRESULT update();

	// ��ȡosg��ʽ�ĵ���
	osg::ref_ptr<osg::Node> CalPointCloud(bool _GetPclPointCloud);

	// ��������
	int SaveCloudPoints(bool _saveAsPCD, bool _saveAsTXT, std::string _filename, std::string _dir);

	osg::ref_ptr<osg::Node> GetPointCloud() const;
private:

	int nDepthWidth;

	int nDepthHeight;

	int nColorWidth;

	int nColorHeight;

	cv::Mat depthImg;

	cv::Mat colorImg;

	//CC3DPoint	* pC3dPoint;

	UINT16		* pDepthBuffer;

	RGBQUAD		* m_pColorRGBX;

	osg::ref_ptr<osg::Node>   model;

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr modelPointcloud;

	IKinectSensor		* m_pKinectSensor;

	IColorFrameReader	* m_pColorFrameReader;

	IDepthFrameReader	* m_pDepthFrameReader;

private:

	// ��Kinect����ת��ΪMatͼ�� & ���浽 m_pColorRGBX
	void ConvertMat_color(const RGBQUAD* pBuffer, int nWidth, int nHeight);
	
	// ��Kinect����ת��ΪMatͼ�� & ���浽 pDepthBuffer
	void ConvertMat_depth(const UINT16* pBuffer, USHORT nMinDepth, USHORT nMaxDepth);

	// ��ȡͼ������
	HRESULT GetColorImage();

	// ��ȡ�������
	HRESULT GetDepthImage();

	HRESULT updateIsOk;

	// ����ó�����
	osg::ref_ptr<osg::Node> AssembleAsPointCloud(bool _GetPclPointCloud);

	
};

