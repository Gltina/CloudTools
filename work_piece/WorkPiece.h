#pragma once
#pragma warning(disable:4996)

#include "../../CloudTools/CloudTools.h"

typedef pcl::PointXYZ pointT;

//! ȱ���������Ϣ
struct Defect_info {
	//! ͶӰ������ƽ������
	double area_size;

	double 
		max_height, //!< �������ƽ������߶�
		mean_height;//!< �������ƽ���ƽ���߶�

	//! �������ڻ���������(�߶�Ϊ0��ƽ��)
	/*!
	vex_vec���ĸ�ֵ���Թ���һ��ƽ�����\n
	���������ڻ����ڿռ��еľ����ĸ�����\n
	vex_vec[0]\n
	vex_vec[1]\n
	vex_vec[2]\n
	vex_vec[3]\n
	*/
	vector<pointT> vex_vec;
};

//! �����࣬���������������
/*!
��ҵ��� Ŀǰ��ɲ��Ƭ\n
һ����ҵ�����Ҫ���7���棬\n
����һ���ϵ����Լ��������棬������������İ�˳��洢
*/
class WorkPiece
{
public:
	WorkPiece();

	~WorkPiece();

	//! ���ö���ķ���
	/*!
	������Ҫ���뺬��������Ĳ���\n
	�˺����Ĳ���������3D���ݲɼ��豸\n
	������ֱ���:\n
	[0][1][2]:ƽ����������\n
	[3][4][5]:ƽ�淨��
	*/
	void set_top_plane_func(vector<float> &top_plane_func);

	//! ����ԭʼ��������
	/*!
	����ԭʼ���ƣ���ʼ���������Ϣ \n
	ע��ԭʼ���ƻᱻ���񻯴���

	\param[in] cloud ԭʼ��������

	*/
	void load_original_cloud(pcl::PointCloud<pointT>::Ptr cloud);

	//! ���㹤��ƽ�淽��
	/*!
	����ƽ�淽�̣���������幤����ƽ�棬���������򣬹���ȹ���
	*/
	void self_calc_plane_func();

	//! ��ԭʼ����ȥ��Բ����
	/*!
	\param[in] threshold_dis �����ж��Ƿ������淽����
	*/
	void self_remove_cylinder_cloud(double threshold_dis = 1.0);

	//! ������ȱ�ݲ���
	void detect_defect_part_on_bottom_plane();

	//! ������������
	void detect_max_dis_bottom_line();

	//! ��ȡ�������(�������ʾֱ�߶�)���Լ���Ӧ�ľ���
	/*!
	�ߵ���ʽ���߶Σ����߶���һ�Ե����ʽ����������һ����ߣ����ĸ����ʾ \n
	point_vec�е�Ԫ��ÿ�ĸ�һ��  \n
	point_vec.[0]-[1] ��һ����  \n
	point_vec.[2]-[3] ����һ���� \n

	\param[out] point_vec �㼯�ԣ�ÿ�ĸ�һ��
	\param[out] dis_vec ����ԣ�ÿ�ĸ�point_vec��Ԫ�ض�Ӧһ��
	*/
	void get_max_dis_bottom_line_segment(vector<pointT>& point_vec, vector<double> &dis_vec);

	//! �����������ĳһ�ξ�����ڵ�
	/*!
	\param[in] dis ��Ҫ�ƶ��ľ���
	*/
	void detect_bottom_inner_point_along_with_line(double dis);

	//! ��ȡ��������ĳһ�ξ�����ڵ�
	/*!
	\param[out] inner_points �ƶ�֮��ĵ㼯
	\param[out] dis_vec ��Ҫ�ƶ��ľ��뼯�ϣ����һһ��Ӧ
	*/
	void get_bottom_inner_point_along_with_line(vector<pointT>& inner_points, vector<double> &dis_vec);

	//! ��ȡ����ȱ�ݼ����
	/*!
	\param[out] bottom_defect ȱ�ݵ㼯����������ԭʼ����
	\param[out] defect_info_vec ȱ�㲿�ֵ���Ϣ
	*/
	void get_bottom_defect(vector<vector<int>>& bottom_defect, vector<Defect_info>&defect_info_vec);

	//! ��ȡ���浽����ĸ߶�
	void get_distance_between_top_and_bottom(double & dis);

	//! ��ȡ����Ķ���
	void get_bottom_vertex(vector<pointT>& bottom_vertex) const;

	//! ��ȡ�����ĵ����������
	void get_bottom_cloud(pcl::PointCloud<pointT>::Ptr bottom_cloud);

	//! ��ȡԭʼ��������
	void get_original_cloud(pcl::PointCloud<pointT>::Ptr original_cloud);

	//! ��ȡû��Բ�����ԭʼ��������
	/*!
	ע�������ȡ��������
	*/
	void get_no_cylinder_original_cloud(vector<int>& no_cylinder_original_cloud_index);

	//! ��ȡ�����ĵ���ƽ�淽��
	void get_bottom_plane_func(Eigen::Vector4f& bottom_plane_func);

	//! ��ȡ������Ƽ���
	void get_beside_cloud_vec(vector<pcl::PointCloud<pointT>::Ptr>& beside_cloud_vec);

	//! ��ȡ����ƽ�淽�̼���
	void get_beside_plane_func_vec(vector<Eigen::Vector4f>& beside_plane_func);

	//! ��ȡ������������
	void get_cloud_center_point(pointT & p);
private:

	//! ���ذ��������Լ������ȫ��ƽ����������Լ���Ӧ����
	/*!
	�㼯��������Ϊ���棬�����Ϊ����\n
	���в��������������������������\n
	ֻ����ȷ�������Լ��������˳���Ż����ƽ�淽�̡�\n
	��ˣ�ƽ�淽�̵�˳��͵㼯��˳����һ����

	\param[in] ȫ���������ݼ���

	*/
	void load_all_plane(vector<pcl::PointCloud<pointT>::Ptr> &cloud_vec);

	//! ���ز���ƽ�����
	/*!
	�������ĵ���\n
	�����ݾ������򣬼����ڵ�ƽ��һ�����໥�����\n
	���������ǲ�����Ƶ���������

	\param[in] ����������ݼ���

	*/
	void load_beside_plane(vector<pcl::PointCloud<pointT>::Ptr> &cloud_vec);

	//! ���ص���ƽ�����
	/*!
	�������ĵ���

	\param[in] �����������
	\param[in] ����ƽ�淽��

	*/
	void load_bottom_plane(pcl::PointCloud<pointT>::Ptr bottom_cloud);

	//! �������Ͳ����ཻ��ֱ�߷���
	/*!
	����ÿ�����淽�̺͵���Ľ���
	*/
	void self_calc_bottom_intersection_of_line_func();

	//! ������潻�ߵĽ���
	/*!
	��������ƽ�����ȷ��һ�����ԭ��\n
	����������������͵���������ƽ��Ľ���\n
	ͬʱ���������Ҳ�ǵ���Ķ���
	*/
	void self_calc_bottom_intersection_of_point();

	//! ����Բ������
	/*!
	Ϊ��Բ�������׼ȷ�ȣ��Ὣ������ƽ�������ȫ��ȥ�����������Ϊ��ʣ��Բ����ĵ���\n
	���Լ���ǰ��Ҫ�������ƽ��ķ��̣����÷��̽������㼯ȥ����\n
	���ʣ�����

	\param[in] threshold_dis �����жϵ㼯�Ƿ��ڲ��淽����

	*/
	void self_calc_cylinder_func(double threshold_dis = 0.8);

	//! ����ϴ����м�������
	void clear();

private:
	//! ���ݴ�����
	CloudTools m_cloud_tools;

	//! ԭʼ��������
	pcl::PointCloud<pointT>::Ptr m_original_cloud;

	//! ������Ƶ���������
	pointT m_bottom_center_point;
	
	//! ������������
	pointT m_cloud_center_point;

	//! ��ԭʼ�������ݵĻ�����ȥ����Բ����
	vector<int> m_original_cloud_without_cylinder_index;

	//! �����������
	pcl::PointCloud<pointT>::Ptr m_bottom_cloud;

	//! ���淽��
	Eigen::Vector4f m_bottom_plane_func;

	//! �����������
	//pcl::PointCloud<pointT>::Ptr m_top_cloud;

	//! ���淽��\n
	/*!
	[0] [1] [2] ��ʾ����\n
	[3] [4] [5] ƽ�淨�߷���
	*/
	vector<float> m_top_plane_func;

	//!ʹ��ABCD����ʾƽmain����
	Eigen::Vector4f m_top_plane_func_ABCD;

	//! ������Ƽ���
	vector<pcl::PointCloud<pointT>::Ptr> m_beside_cloud_vec;
	//! ���淽�̼���
	vector<Eigen::Vector4f> m_beside_plane_func_vec;

	//! ����͸�������Ľ���
	vector<Line_func> m_bottom_intersection_of_line_vec;

	//! ���潻�ߵĽ���(����Ķ���)
	/*!
	[0]-[1] ��line[0]�����˵�\n
	[1]-[2] ��line[1]�����˵�
	*/
	vector<pointT> m_bottom_intersection_of_point_vec;

	//! (�м������ȸ�)Բ���ķ���
	/*!
	Ϊ���ڴ����ʹ��vector�洢
	*/
	vector<Cylinder_func> m_cylinder_func_vec;

	//! ����ȱ�϶������֣����������ȥ��������ԭʼ����
	vector<vector<int>> m_bottom_defect_index;
	
	//! �ײ�ȱ����Ϣ���Ծ��α�ʾ
	vector<Defect_info> m_bottom_defect_info_vec;

	//! ����������
	/*! 
	������ ����͸�������Ľ���\n
	Couple_distance.i1 ����1\n
	Couple_distance.i2 ����2\n
	Couple_distance.distance ��ֱ�߼�ľ���
	*/
	vector<Couple_distance> m_max_dis_bottom_line_vec;

	//! �����ƶ�һ������ĵ�
	vector<pointT> m_bottom_innner_points;

	//! �����ƶ�һ������ĵ㵽����ľ���
	vector<double> m_bottom_inner_distance;
};

