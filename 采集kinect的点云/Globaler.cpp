#pragma once
#include "Globaler.h"

// ��ʾ����,�����Ƿ��л�������
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

// ��ʼ��ʾ��ǰ֡�ź���
pthread_cond_t startCond = PTHREAD_COND_INITIALIZER;   

// ˢ��֡�ź���
pthread_cond_t displayCond = PTHREAD_COND_INITIALIZER;

// ����ڵ�
osg::ref_ptr<osg::Group> root = new osg::Group();

// ����kinect������
osg::ref_ptr<osg::Node> kinectModel = new osg::Node();

// ʵʱ��ʾ
osg::ref_ptr<osg::Node> leftFrame = new osg::Node(); 

// �ں���ʾ
osg::ref_ptr<osg::Node> rightFrame = new osg::Node(); 

// ����������λ�þ���
osg::Matrix x = osg::Matrix::rotate(-180, 1, 0, 0)*osg::Matrix::rotate(-180, 0, 0, 1);

osg::ref_ptr<osg::MatrixTransform> leftMT = new osg::MatrixTransform();
osg::Matrix leftM = x*osg::Matrix::rotate(osg::inDegrees(90.0f), 1, 0, 0)*osg::Matrix::translate(-8, 0, 2);

osg::ref_ptr<osg::MatrixTransform> rightMT = new osg::MatrixTransform();
osg::Matrix rightM = x*osg::Matrix::rotate(osg::inDegrees(90.0f), 1, 0, 0)*osg::Matrix::translate(8, 0, 0);

// ������ʾ����
osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
osg::observer_ptr<osgViewer::GraphicsWindow> window = new osgViewer::GraphicsWindow;
osg::ref_ptr<osg::Node> getfirstFrame(void);

bool flag_getCloudPoints = false;

size_t saveCount = 0;
// ���������������
struct argc_s{
	int argc;
	char ** argv;
};
static argc_s argcS;

pthread_t show_t;
pthread_t glut_t;

// ��������Ԥ����
void preProcess(){

	leftMT->setMatrix(leftM);
	rightMT->setMatrix(rightM);

	leftMT->addChild(leftFrame);
	rightMT->addChild(rightFrame);

	root->addChild(leftMT);
	root->addChild(rightMT);
}

// ���� mykinect�߳�
int createKinectThread(){
	pthread_t kinect_t;

	int kinect_t_flag;

	kinect_t_flag = pthread_create(&kinect_t, NULL, myKinect, NULL);

	if (kinect_t_flag != 0)
	{
		std::cout << "Kinect�̴߳���ʧ��" << std::endl;
		return -1;
	}
	return 0;
}
 
// ����showAgain�߳�
int createShowThread(){
	int show_t_flag;

	show_t_flag = pthread_create(&show_t, NULL, showAgain, NULL);

	if (show_t_flag != 0)
	{
		std::cout << "show�̴߳���ʧ��" << std::endl;
		return -1;
	}
	return 0;
}

// ���������߳�
int createGLUTThread(int _argc,char **_argv){
	int glut_t_flag;
	
	argcS.argc = _argc;
	argcS.argv = _argv;

	glut_t_flag = pthread_create(&glut_t, NULL, myGlut,(void *)&argcS);

	if (glut_t_flag != 0)
	{
		std::cout << "glut�̴߳���ʧ��" << std::endl;
		return -1;
	}
	return 0;
}

// ���������ź�
// 1. ����getfirstFrame ��֪kinect�Ѿ��߱���ȡͼ�������
// 2. ����showAgain ��֪�ѻ�ȡͼ������,����Ҫ�ػ�
void * myKinect(void * argc){
	// ����Ϊ�ɷ���
	pthread_detach(pthread_self());

	std::cout <<"kinect�̴߳�"<< std::endl;

	CMotionRecognition motionRecognition;

	size_t frameCount = 0;

	// ��ʼ��kinect
	HRESULT hr = motionRecognition.InitializeDefaultSensor();
	HRESULT hrUpdate = E_FAIL;
	while (!SUCCEEDED(hrUpdate))
	{
		hrUpdate = motionRecognition.update();
	}

	// ֪ͨgetfirstFrame() �Ѿ���ȡ������
	// ��һ���ź�
	pthread_mutex_lock(&mutex);

	kinectModel = motionRecognition.CalPointCloud(flag_getCloudPoints);
	
	pthread_mutex_unlock(&mutex);
	
	pthread_cond_signal(&startCond);

	// ����ѭ����ȡ
	while (true)
	{
		hr = motionRecognition.update();
		
		if (SUCCEEDED(hr))
		{
			pthread_mutex_lock(&mutex);
			
			kinectModel = motionRecognition.CalPointCloud(flag_getCloudPoints);

			if (flag_getCloudPoints)
			{
				rightMT->setChild(0, kinectModel);
				cout <<"���ڱ����������..."<< endl;
				motionRecognition.SaveCloudPoints(true, false, std::to_string(++saveCount), "resultUpdate");
				cout << "�������" << endl;
				flag_getCloudPoints = false;
			}

			// ������Ϣ��showAgain ͨ��������glutPostRedisplay()��ͼ����
			// �ڶ����ź�
			pthread_cond_signal(&displayCond);
			
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}

// ���Ͻ����ź��Ե����ػ��ź�
void * showAgain(void * argc){
	// ���Ͻ����ź��Ե����ػ��ź�
	while (true)
	{
		//std::cout << "�ȴ������ػ��ź�..." << std::endl;

		pthread_mutex_lock(&mutex);

		pthread_cond_wait(&displayCond, &mutex);

		// ��������ز���
		//std::cout << "�ѽ����ػ��ź�" << std::endl;

		glutPostRedisplay();

		// ��������
		pthread_mutex_unlock(&mutex);
	}
}

// Ϊ�˴�����������źŶ�������ʾ�ĵ�һ֡ͼ��
osg::ref_ptr<osg::Node> getfirstFrame(void)
{
	std::cout << "���ڵȴ�kinect��ȡͼ��..." << std::endl;

	pthread_mutex_lock(&mutex);

	pthread_cond_wait(&startCond,&mutex);

	// ��������ز���
	std::cout << "��ȡ�ɹ�..." << std::endl;

	// ��������
	pthread_mutex_unlock(&mutex);

	return kinectModel;
}

// �����������
void * myGlut(void *argc){
	argc_s argcS = *(argc_s * )(argc);
	glutInit(&(argcS.argc), argcS.argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glRotatef(-180.0, 0.0, 0.0, 1.0);
	glutCreateWindow("GLUT");

	glutDisplayFunc	(display);	//ע���ػ��Ļص�����
	glutReshapeFunc	(reshape);	//ע�ᴰ�ڴ�С�ı�Ļص�����
	glutMouseFunc	(mousebutton);	//ע����갴���ص�����
	glutMotionFunc	(mousemove);	//ע������ƶ��ص�����
	glutKeyboardFunc(keyboard);

	leftMT->setChild(0, getfirstFrame());
	rightMT->setChild(0,kinectModel);

	// create the view of the scene.
	window = viewer->setUpViewerAsEmbeddedInWindow(100, 100, 800, 600);
	viewer->setSceneData(root.get());
	viewer->setCameraManipulator(new osgGA::TrackballManipulator);
	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->realize();

	glutMainLoop();
	return NULL;
}

// ������Ӧ�ص�����
void display(void)
{
	leftMT->setChild(0, kinectModel);

	// update and render the scene graph
	if (viewer.valid())
	{
		viewer->frame();
	}

	// Swap Buffers
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	// update the window dimensions, in case the window has been resized.
	if (window.valid())
	{
		window->resized(window->getTraits()->x, window->getTraits()->y, w, h);
		window->getEventQueue()->windowResize(window->getTraits()->x, window->getTraits()->y, w, h);
	}
}

void mousebutton(int button, int state, int x, int y)
{
	if (window.valid())
	{
		if (state == 0) window->getEventQueue()->mouseButtonPress(x, y, button + 1);
		else window->getEventQueue()->mouseButtonRelease(x, y, button + 1);
	}
}

void mousemove(int x, int y)
{
	if (window.valid())
	{
		window->getEventQueue()->mouseMotion(x, y);
	}
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
	switch (key)
	{
	case 27:
		// clean up the viewer 
		if (viewer.valid()) viewer = 0;
		glutDestroyWindow(glutGetWindow());
		break;
	// VΪ��׽��ǰ֡
	case 'V':
		startUpSaveAsFile();
		break;
	default:
		if (window.valid())
		{
			window->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) key);
			window->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol) key);
		}
		break;
	}
}

int destroyPthread(){
	// �߳�������ٲ���
	// ������������Ҫһ�����ٺ���

	pthread_join(glut_t,NULL);
	pthread_join(show_t,NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&displayCond);
	pthread_cond_destroy(&startCond);
	return 0;
}

void startUpSaveAsFile(){
	pthread_mutex_lock(&mutex);
	flag_getCloudPoints = true;
	pthread_mutex_unlock(&mutex);
}

int pcbConvertintoOsg(pcl::PointCloud<pcl::PointXYZRGB>::Ptr &_pcbResult, osg::ref_ptr<osg::Node> &_frame){
	// һ֡�ռ�����
	osg::ref_ptr<osg::Vec3Array> coodrVec = new osg::Vec3Array();
	// һ֡��ɫֵ
	osg::ref_ptr<osg::Vec4Array> colorVec = new osg::Vec4Array();
	for (size_t i = 0; i < _pcbResult->size(); i++)
	{
		coodrVec->push_back(osg::Vec3f(_pcbResult->at(i).x, _pcbResult->at(i).y, _pcbResult->at(i).z));
		colorVec->push_back(osg::Vec4f((float)_pcbResult->at(i).r / 255, (float)_pcbResult->at(i).g / 255, (float)_pcbResult->at(i).b / 255, 1));
	}
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	// �����洢����������Ϣ ����ͼ�� �����˶����������ݵ���Ⱦָ��
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

	geom->setVertexArray(coodrVec.get());

	geom->setColorArray(colorVec.get());
	// ÿһ����ɫ��Ӧ��һ������
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	// ָ�����ݻ��Ƶķ�ʽ
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, coodrVec->size()));
	// ���ص�Geode��
	geode->addDrawable(geom.get());
	// �رչ���
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	// ע���Լӿ��ٶ�
	//��������״̬���������߿�Ϊ2������,��Ϊ4�����ء�
	osg::ref_ptr<osg::StateSet> stateGeode = geode->getOrCreateStateSet();
	osg::ref_ptr<osg::Point> ptr = new osg::Point(4);
	stateGeode->setAttribute(ptr);

	_frame = geode;
	return 0;
}

