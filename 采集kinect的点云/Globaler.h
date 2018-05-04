#pragma once
#include "Header.h"
#include "MotionRecognition.h"

// ����Ԥ����
void preProcess();

// �����߳�
int createKinectThread();
int createShowThread();
int createGLUTThread(int _argc,char **argv);

// �������߳�
void * myKinect(void * argc);
void * myGlut(void *argc);
void * showAgain(void * argc);

// �����߳�����
int destroyPthread();

// ����ص�����
void display(void);
void reshape(int w, int h);
void mousebutton(int button, int state, int x, int y);
void mousemove(int x, int y);
void keyboard(unsigned char key, int /*x*/, int /*y*/);

// ��Ӧ�����¼�
// ��������
void startUpSaveAsFile();