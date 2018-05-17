#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>
#include <pcl/visualization/pcl_visualizer.h>
int
main(int argc, char** argv)
{
	//���ص���ģ��
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	// pcl::PCLPointCloud2 cloud_blob;
	if (pcl::io::loadPCDFile<pcl::PointXYZ>("pcd_cloud1_grid.pcd", *cloud) == -1){

		PCL_ERROR("Could not read file \n");
	}
	//* the data should be available in cloud

	// Normal estimation*
	//�������
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//����kdtree�����н��ڵ㼯����
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	//Ϊkdtree��ӵ�������
	tree->setInputCloud(cloud);

	n.setInputCloud(cloud);
	n.setSearchMethod(tree);
	//���Ʒ������ʱ����Ҫ�����Ľ��ڵ��С
	n.setKSearch(20);
	//��ʼ���з������
	n.compute(*normals);
	//* normals should not contain the point normals + surface curvatures

	// Concatenate the XYZ and normal fields*
	//�����������뷨����Ϣƴ��
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
	pcl::concatenateFields(*cloud, *normals, *cloud_with_normals);

	/*ͼ����ʾģ��*/
	//��ʾ����
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));

	//���ñ���ɫ
	viewer->setBackgroundColor(0, 0, 0.7);

	//���õ�����ɫ���ô�Ϊ��һ��ɫ����
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0);

	//�����Ҫ��ʾ�ĵ�������
	viewer->addPointCloud<pcl::PointXYZ>(cloud, single_color, "sample cloud");

	//���õ���ʾ��С
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");

	//�����Ҫ��ʾ�ĵ��Ʒ���cloudΪԭʼ����ģ�ͣ�normalΪ������Ϣ��10��ʾ��Ҫ��ʾ����ĵ��Ƽ������ÿ10������ʾһ�η��򣬣���ʾ���򳤶ȡ�
	viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(cloud, normals, 10, 5, "normals");

	//--------------------
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}

	// Finish
	return (0);
}