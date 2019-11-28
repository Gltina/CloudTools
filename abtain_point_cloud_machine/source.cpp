/* 
* ��ά���ƻ�ȡ��ת̨����
* ʹ�ô���ͨ�� �� VST3D��ά���� SDK
*/
#include <boost/thread/thread.hpp>

#include "../../CloudTools/CloudTools.h"
#include "../../examples/work_piece/WorkPiece.h"
#include <CameraComm1.h>
#include <SerialComm.h>
#include <VST3D.h>

// �ȴ�ת̨�˶����
const float WAIT_MSECOND = 500;

// ������Ϣ���
const float SEND_GAP_MSECOND = 1000;

// �����ٴ�ɨ����(ϣ����Ҫ�õ�)
const float SCAN_GAP_MSECOND = 1000;

//! ���ڸı�����ĸ߶�
int line_count = 1;
int col_count = 1;

//! �����С
int font_size = 12;
int line_add = 12;

//! -1:��ͣ 0:2D���  1:��ʼ
/*!
	����a-> 0
	����b-> 1
*/
int obtain_point_cloud_signal = -1;

// ��ǰ����ĵ�������
int cloud_count = 0;

// ���/���ϼ������ͷ
CameraComm1 camera_comm1;

// ����ͨ��(����ת̨)
SerialComm serial_comm;

// 3d ɨ�����
VST3D vst3d;

// TODO:�Ѻۼ�����

// 3D ���ݴ���
WorkPiece work_piece;

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());

pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("ȱ�ϼ��"));

//! �����ʼ������ģ�飬����� 1. ����ͨ��ģ�� 2. vst3d���ģ��
bool init_env(vector<float> &);

//! ת��vst��ʽ����Ϊpcd��ʽ
void convert_vst2pcd(VST3D_PT ** pPointClouds, pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);

//! ��ȡ�����߳�
void obtain_point_cloud(std::vector<string> &);

//! ���ļ��ж�ȡ����ָ��
void read_command_from_file(const string& filename,vector<string> &command_vec);

//! ���ԭʼ��������
void add_original_cloud(pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointXYZ>::Ptr original_cloud);

//! ���ȱ����Ϣ
void add_defect_info(pcl::visualization::PCLVisualizer::Ptr viewer, vector<vector<int>> &bottom_defect, pcl::PointCloud<pcl::PointXYZ>::Ptr original_cloud, vector<Defect_info> &defect_info_vec);

//! ��������Ϣ
void add_max_dis_line(pcl::visualization::PCLVisualizer::Ptr viewer, vector<pointT>& point_vec, vector<double>& dis_vec);

//! ������ڵ�
void add_inner_point(pcl::visualization::PCLVisualizer::Ptr viewer, vector<pointT>& point_vec, vector<double>& inner_distance_vec);

//! ��Ӹ߶���Ϣ
void add_height(pcl::visualization::PCLVisualizer::Ptr viewer, double height);

//! ���2d��������Ϣ
void add_work_piece_info(pcl::visualization::PCLVisualizer::Ptr viewer, const string &str);

//! 2D����߳�
void obtain_2D_camera();

//! pcl�µļ��̼��
void keyboardEventOccurred(const pcl::visualization::KeyboardEvent& event, void* nothing);

int main()
{
	vector<float> top_plane_func;
	if (0 != init_env(top_plane_func))
	{
		cerr<<"��ʼ��ʧ��"<<endl;
		system("pause");
		return -1;
	}
	work_piece.set_top_plane_func(top_plane_func);

	vector<string> command_vec;
	read_command_from_file("command.txt", command_vec);
	if (command_vec.empty())
	{
		cerr<<"ָ���ȡʧ��"<<endl;
		return -1;
	}

	viewer->setBackgroundColor(0, 0, 1);

	viewer->addCoordinateSystem(1.0, "first");
	
	// �������̿��ƣ���ȡ�����ת̨����ָ��
	viewer->registerKeyboardCallback(&keyboardEventOccurred, (void*)NULL);

	// ��������ת̨+��ȡ���ƽ���
	boost::thread th_obtain_2D_camera(&obtain_2D_camera);
	//th_obtain_2D_camera.join();

	// ��������ת̨+��ȡ���ƽ���
	boost::thread th_obtain_point_cloud(&obtain_point_cloud, command_vec);
	//th_obtain_point_cloud.join();
	
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
	    //boost::this_thread::sleep(boost::posix_time::microseconds(1000));
	}

	//viewer->spin();
	return 0;
}

void init_table()
{
	string init[2] =
	{
		"YH 10 3",
		"XH 10 3"
	};
	int serial_status;

	cout << "[ת̨]" << "ת̨��ʼ��" << endl;

	serial_status = serial_comm.send(init[0] + "\r\n");

	Sleep(5000);

	serial_status = serial_comm.send(init[1] + "\r\n");

	Sleep(1000);

	cout << "[ת̨]" << "ת̨��ʼ�����" << endl;
}

bool init_env(vector<float> & top_plane_func)
{
	// ����ͨ��ģ��
	try
	{
		int serial_status = 0;
		serial_status = serial_comm.open_port("COM5");

		if (!serial_status)
		{
			throw string("Open port failed" );
		}

		serial_status = serial_comm.setup_DCB(9600);
		if (!serial_status)
		{
			throw string("Setup DCB failed");
		}

		serial_status = serial_comm.setup_timeout(0, 0, 0, 0, 0);
		if (!serial_status)
		{
			throw string("Setup timeout failed");
		}
		serial_comm.flush_buffer();
	}
	catch (const string &e)
	{
		cout << "[����] ����ͨ�ų�ʼ��ʧ��" << endl;
		cerr << e << endl;
		return -1;
	}

	cout << "[����]" << "����ͨ�ų�ʼ�� OK!" << endl;

	// vst3d���ģ��
	try
	{
		int vst3d_status = 0;

		vst3d_status = vst3d.init("C:\\Program Files\\VST\\VisenTOP Studio\\VisenTOP Studio.exe");
		if (VST3D_RESULT_OK != vst3d_status)
		{
			throw string("Could not Start Scanner software");
		}
		Sleep(3000);

		vst3d_status = vst3d.connect_align();
		if (VST3D_RESULT_OK != vst3d_status)
		{
			throw string("Could not connect to Scanner(connect_align)");
		}

		Sleep(3000);

		vst3d_status = vst3d.del_background(top_plane_func);
		if (VST3D_RESULT_OK != vst3d_status)
		{
			throw string("Could not del_background");
		}
	}
	catch (const string& e)
	{
		cout << "[3d] 3D �����ʼ��ʧ��" << endl;
		cerr << e << endl;
		return -2;
	}

	cout << "[3D]" << "3D �����ʼ�� OK!" << endl;

	//! ��ʼ��ת̨
	{
		init_table();
	}

	//! TCP/IP ͨ��
	{
		char ip[32] = "192.168.1.111";
		int port = 8081;
		camera_comm1.init(ip, port);
	}
	return 0;
}

void convert_vst2pcd(
	VST3D_PT ** pPointClouds, 
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	int totalnum;
	vst3d.get_num_points(totalnum);

	// �������βɼ��������е��ƣ��õ������������Ϣ��ͨ��������ʽ[0 1 2 ....]
	VST3D_PT * pt = nullptr;

	pcl::PointXYZ normal;
	
	for (int i = 0; i < totalnum; i++)
	{
		vst3d.get_each_point_by_index(i, &pt);
		normal.x = pt->x;
		normal.y = pt->y;
		normal.z = pt->z;
		//normal.normal_x = pt->nx;
		//normal.normal_y = pt->ny;
		//normal.normal_z = pt->nz;
		//cr = (float)pt->cr / 255;
		//cg = (float)pt->cg / 255;
		//cb = (float)pt->cb / 255;
		cloud->points.push_back(normal);
	}

	cloud->height = 1;
	cloud->width = cloud->points.size();
}

//! ��ȡ�������ݲ��Ҵ����߳�
void obtain_point_cloud(std::vector<string> &command_vec)
{
	cout << "[3D] ��������ȡ�����߳�" << endl;

	while (true)
	{
		// ��ʼ��ȡ����
		if (obtain_point_cloud_signal == 1)
		{
			cout << "[3D]" << "�յ�ָ���ʼ��ȡ��������" << endl;
			cout << "[3D]" << "�ȴ���е�ֱ��˶�" << endl;

			//Sleep(10000);
			Sleep(1000);

			string curr_command;

			int cloud_size = 0;

			int serial_status, vst3d_status;

			for (int i = 0; i < command_vec.size(); ++i)
			{
				//cout << "[" << i << "]" << "----------begin----------" << endl;

				curr_command = command_vec[i];

				cout << "[3D]��ǰָ��:" << curr_command << endl;

				// ����ָ��
				serial_status = serial_comm.send(curr_command+ "\r\n");

				while (!serial_status)
				{
					cerr << "���ݷ���ʧ�ܣ��ȴ� " << SEND_GAP_MSECOND / 1000 << " s" << endl;

					Sleep(SEND_GAP_MSECOND);

					serial_status = serial_comm.send(curr_command);
				}

				cout << "[3D]" << curr_command << "���ݷ��ͳɹ�" << endl;

				// �ȴ�ת̨ת��
				cout << "[3D]" << "�ȴ�ת̨ת����" << "�ȴ� " << WAIT_MSECOND/1000 << "s" << endl;

				Sleep(WAIT_MSECOND);

				// ��ȡ��������
				cout << "[3D]" << "��ʼɨ��" << endl;

				vst3d_status = VST3D_Scan(); // ��ʼ����ɨ��

				while (vst3d_status != VST3D_RESULT_OK)
				{
					cerr << "ɨ�����" << "�ȴ�"
						<< SCAN_GAP_MSECOND / 1000 << "s " << "���¿�ʼɨ��" << endl;
					// TODO
					// ɨ�������????
					Sleep(SCAN_GAP_MSECOND);

					vst3d_status = VST3D_Scan(); // ��ʼ����ɨ��
				}

				cout << "[3D]" << "��ǰɨ��ɹ�����ʼ��ȡ��������" << endl;

				VST3D_PT * pPointClouds = nullptr;

				vst3d_status = vst3d.get_point_cloud(cloud_size, &pPointClouds);

				if (vst3d_status == VST3D_RESULT_OK)
				{
					// TODO:
					// ת��Ϊpcd��ʽ???
					//cout << "���ƴ�С: " << cloud_size << endl;

					convert_vst2pcd(&pPointClouds, cloud);

					*cloud += *cloud;

					//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud_in_color_h(cloud, 255, 255, 255);
					//viewer->updatePointCloud(cloud, cloud_in_color_h, "cloud");
					//viewer->updatePointCloud(cloud, "cloud");
				}
				// TODO:
				// ��ȡ�����������
				else
				{
					cerr << "��ȡ���ƴ��� error code:" << vst3d_status << endl;
					// do nothing
				}
				//cout << "[" << i << "]" << "----------over----------" << endl;
			}

			cout << "[3D]" << "����ָ���Ѿ�����" << endl;
			init_table();

			cout << "[3D]" << "���Ʊ������" << endl;
			pcl::io::savePCDFileBinary(
				"completed_cloud" + std::to_string(cloud_count) + ".pcd", 
				*cloud);
			
			cout << "[3D]" << "���Ʊ������" << endl;

			work_piece.load_original_cloud(cloud);
			
			// ������浽����ĸ߶�
			double distance;
			work_piece.get_distance_between_top_and_bottom(distance);
			add_height(viewer, distance);
			cout << "���浽����ĸ߶�:" << distance << endl;

			// ������ȱ�����
			work_piece.detect_defect_part_on_bottom_plane();
			vector<vector<int>> bottom_defect;
			vector<Defect_info> defect_info_vec;
			work_piece.get_bottom_defect(bottom_defect, defect_info_vec);
			cout << "ȱ��/�ǲ�������:" << bottom_defect.size() << endl;
			add_defect_info(viewer, bottom_defect, cloud, defect_info_vec);

			// ��ȡ���߳�
			work_piece.detect_max_dis_bottom_line();
			vector<pointT> point_vec;
			vector<double> dis_vec;
			work_piece.get_max_dis_bottom_line_segment(point_vec, dis_vec);
			add_max_dis_line(viewer, point_vec, dis_vec);
			cout << "���߳�����:" << dis_vec.size() << endl;

			// ��ȡ�����ƶ�3mm�ĵ�
			work_piece.detect_bottom_inner_point_along_with_line(3.0);
			vector<pcl::PointXYZ> inner_point_vec;
			vector<double> inner_distance_vec;
			work_piece.get_bottom_inner_point_along_with_line(inner_point_vec, inner_distance_vec);
			for (auto D : inner_distance_vec) cout << "inner D:" << D << endl;
			add_inner_point(viewer, inner_point_vec, inner_distance_vec);

			// ���ԭʼ����
			add_original_cloud(viewer, cloud);

			// ��ȡ�������ĵ㣬��������ϵ
			pointT p;
			work_piece.get_cloud_center_point(p);
			viewer->addCoordinateSystem(5, p.x, p.y, p.z);

			vst3d.clear();

			//work_piece.clear();
			// system("cls");
			obtain_point_cloud_signal = -1;
		}
		
		// ��ͣ
		//if (obtain_point_cloud_signal == -1)
		//{
			// TODO
		//}
	}
}

//! 2D���ͨ��
void obtain_2D_camera();

void read_command_from_file(const string & filename, vector<string> &command_vec)
{
	ifstream ifile(filename);

	if (!ifile.is_open())
	{
		cerr <<"Open file failed"<< endl;
		return;
	}

	string str;

	while (getline(ifile, str))
	{
		command_vec.push_back(str);
	}

	ifile.close();
}

void add_original_cloud(pcl::visualization::PCLVisualizer::Ptr viewer, pcl::PointCloud<pcl::PointXYZ>::Ptr original_cloud)
{
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> rgb(original_cloud, 0, 255, 0);
	string str = "original_cloud";
	viewer->addPointCloud(original_cloud, rgb, str);
}

void add_defect_info(pcl::visualization::PCLVisualizer::Ptr viewer, vector<vector<int>>& bottom_defect, pcl::PointCloud<pcl::PointXYZ>::Ptr original_cloud, vector<Defect_info>& defect_info_vec)
{
	string t = "defect size:" + to_string(bottom_defect.size()) + "(red zone)";
	line_count += line_add;
	viewer->addText(t, col_count, line_count, font_size, 1, 1, 1);

	for (size_t i = 0; i < defect_info_vec.size(); ++i)
	{
		t = "defect:" + to_string(i) + " size:" + to_string(defect_info_vec[i].area_size) + " max/mean height:" + to_string(defect_info_vec[i].max_height) + "/" + to_string(defect_info_vec[i].mean_height);
		line_count += line_add;
		viewer->addText(t, col_count, line_count, font_size, 1, 1, 1);
	}
	CloudTools cloud_tools;
	// ����ȱ��ȱ�ǲ��ֵ�������
	for (int i = 0; i < bottom_defect.size(); ++i)
	{
		string str = "defect" + to_string(i);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
		cloud_tools.index_vector_to_cloud < pcl::PointXYZ, pcl::PointXYZ>
			(bottom_defect[i], *original_cloud, *cloud);
		pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> rgb(cloud, 255, 0, 0);
		viewer->addPointCloud(cloud, rgb, str);
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION_POINTS, 5, str);
	}
}

void add_max_dis_line(pcl::visualization::PCLVisualizer::Ptr viewer, vector<pointT>& point_vec, vector<double>& dis_vec)
{
	size_t j = 0;
	for (size_t i = 0; i < point_vec.size(); i += 4)
	{
		viewer->addLine<pcl::PointXYZ>(point_vec[i], point_vec[i + 1], 255, 0, 0, "line" + to_string(i));
		viewer->addLine<pcl::PointXYZ>(point_vec[i + 2], point_vec[i + 3], 255, 255, 255, "line" + to_string(i + 1));

		string show_dis = "edge:" + to_string(j) + " distance:" + std::to_string(dis_vec[j]);
		line_count += line_add;
		viewer->addText(show_dis, col_count, line_count, font_size, 1, 1, 1);
		j++;
	}
}

void add_inner_point(pcl::visualization::PCLVisualizer::Ptr viewer, vector<pcl::PointXYZ>& point_vec, vector<double>& inner_distance_vec)
{
	string t = "inner point:" + to_string(point_vec.size());
	line_count += line_add;
	viewer->addText(t, col_count, line_count, font_size, 1, 1, 1);

	for (size_t i = 0; i < inner_distance_vec.size(); ++i)
	{
		t = to_string(i) + " inner height(to top plane):" + to_string(inner_distance_vec[i]);
		line_count += line_add;
		viewer->addText(t, col_count, line_count, font_size, 1, 1, 1);
	}
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	for (size_t i = 0; i < point_vec.size(); ++i)
	{
		cloud->points.push_back(point_vec[i]);
	}
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> rgb(cloud, 255, 0, 0);
	string str = "inner_point";
	viewer->addPointCloud(cloud, rgb, str);
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION_POINTS, 8, str);
}

void add_height(pcl::visualization::PCLVisualizer::Ptr viewer, double height)
{
	string t = "height:" + to_string(height);
	line_count += line_add;
	viewer->addText(t, col_count, line_count, font_size, 1, 1, 1);
}

void add_work_piece_info(pcl::visualization::PCLVisualizer::Ptr viewer, const string & str)
{
	line_count += line_add;
	viewer->addText(str, col_count, line_count, font_size, 1, 1, 1);
}

void obtain_2D_camera()
{
	
	string id, result;
	while (true)
	{
		if (obtain_point_cloud_signal == 0)
		{
			// ����111ָ��
			camera_comm1.send("111");
			Sleep(500);
			// ����111���
			camera_comm1.receive(id);

			Sleep(5000);

			// ����222ָ��
			camera_comm1.send("222");
			Sleep(500);
			// ����222���
			camera_comm1.receive(result);

			const string t = id + ":" + result;
			add_work_piece_info(viewer, t);

			// ��ͣ�ȴ�
			obtain_point_cloud_signal = -1;
		}
	}
}

void keyboardEventOccurred(const pcl::visualization::KeyboardEvent & event, void * nothing)
{
	string input = event.getKeySym();
	if (input == "a")
	{
		cout << "[2D] 2D��⿪ʼ" << endl;
		obtain_point_cloud_signal = 0;
	}
	else if (input == "b")
	{
		cout << "[3D] 3D��⿪ʼ" << endl;
		obtain_point_cloud_signal = 1;
	}
}
