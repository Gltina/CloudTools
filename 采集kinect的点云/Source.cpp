#pragma once
#include "Globaler.h"

//2016-12-07 ������־:
//	���洢�ľ����޸�Ϊ�˴洢��ǰһ֡�ı任����,���Ƹ�ʽ����

// 2016-12-29 ������־
// ȡ��ʵʱ�任�Ĺ���,������תΪ��һ��ͼ���ȡ������.
// �������Ϊ:
// 1).����V��������
// 2).�Զ���ı���pcd��txt��ʽ������

int main(int argc, char **argv)
{
	// ��ʼ������������λ��
	preProcess();

	// �������ڳ���
	createGLUTThread(argc,argv);

	// ����kinect���߳�,�����ǿɷ����.
	createKinectThread();

	// ��������display���ػ���߳�
	createShowThread();

	// �ϲ��̲߳������̲߳���
	destroyPthread();
	
	return 0;
}

