//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <opencv2/xfeatures2d.hpp>
//#include <vector>
//#include <ceres/ceres.h>
//#include <Eigen/Dense>
//#include <Eigen/Core>
//#include "DEMReadProcess.h"
//#include "orthorectification.h"
//#include "optimation.h"
//#include "SFSutils.h"
//#include "glog/logging.h"
//#include "SFSutils.h"
//#include "DEMReadProcess.h"
//#include "orthorectification.h"
//#include <opencv2/core/eigen.hpp>
//#include <fstream>
//
//using namespace std;
//using namespace cv;
//using namespace Eigen;
//using namespace cv::xfeatures2d;
//
//void readLandmarks(const char *path, vector<Vector3d> &landmarks, int scale)
//{
//    /*
//    path:����landmarks.txt���ļ�·��
//    landmarks:�������ն�ȡ�Ľ��
//    scale:cinema4d�����ģ�ͳ߶�
//    */
//    ifstream infile;
//    infile.open(path);
//    if (!infile)
//    {
//        cout << "open file error!" << endl;
//    }
//    //�����landmarks.txtǰ������˵����
//    //�������Ǳ����landmark�������ӵ�5�п�ʼ��landmark
//    string s;
//    getline(infile, s);
//    getline(infile, s);
//    getline(infile, s);
//    int num;
//    infile >> num;
//    double x, y, z;
//    while (!infile.eof())
//    {
//        //��ȡ�ĵ�������m���ǵû���߶�
//        infile >> x >> y >> z;
//        infile.get(); // ��ȡ���Ļس���
//        if (infile.peek() == '\n')
//            break;
//        x = x * scale;
//        y = y * scale;
//        z = z * scale;
//        Vector3d V(x, y, z);
//        SFSutils::convertPoint2Right(V);
//        landmarks.push_back(V);
//    }
//    infile.close();
//}
//
////�����Ϊ165�����������ʼ
//int main()
//{
//    vector<Vector3d> sun_pos_list;
//    vector<Vector3d> cam_pos_list;
//    vector<Vector3d> cam_hpb_list;
//    vector<Vector3d> dem3d;
//    vector<int> indexList;//����ͼƬ����
//    string camPath = "E:\\Ѹ������\\SPC�ܽ�20210120(1)\\SPC�ܽ�20210120\\camPos.txt";
//    string sunPath = "E:\\Ѹ������\\SPC�ܽ�20210120(1)\\SPC�ܽ�20210120\\sunPos.txt";
//    int size = 240;
//
//    //��ȡ�������������ת��������ϵ
//    ifstream infile;
//    infile.open(camPath.c_str());
//    if (!infile)
//    {
//        cout << "open file error!" << endl;
//    }
//    double x, y, z, h, p, b;
//    int cF;
//    while (!infile.eof())
//    {
//        infile >> cF >> x >> y >> z >> h >> p >> b;
//        infile.get(); // ��ȡ���Ļس���
//        if (infile.peek() == '\n')
//            break;
//        Vector3d camPos(x, y, z);
//        SFSutils::convertPoint2Right(camPos);
//        cam_pos_list.push_back(camPos);
//        cam_hpb_list.push_back(Vector3d(h, p, b));
//    }
//    infile.close();
//
//    //��ȡ����̫��������ת��������ϵ
//    ifstream infile2;
//    infile2.open(sunPath.c_str());
//    if (!infile2)
//    {
//        cout << "open file error!" << endl;
//    }
//    while (!infile2.eof())
//    {
//        infile2 >> cF >> x >> y >> z;
//        infile2.get(); // ��ȡ���Ļس���
//        if (infile2.peek() == '\n')
//            break;
//        Vector3d sunPos(x, y, z);
//        SFSutils::convertPoint2Right(sunPos);
//        sun_pos_list.push_back(sunPos);
//    }
//    infile2.close();
//
//    //��ȡ����landmark������
//    string landmarkPath = "E:\\Ѹ������\\SPC�ܽ�20210120(1)\\SPC�ܽ�20210120\\landmarks.txt";
//    vector<Vector3d> landmarks;
//    int scale = 1;
//    readLandmarks(landmarkPath.c_str(), landmarks, scale);//��landmark�����걣�浽landmarks��������
//    //��ȡlandmark��ά����
//    Vector3d V = landmarks[8];//��Ҫ�޸�
//    CDEM Cdem = CDEM(V);
//    cout << "V = " << Cdem.V << endl;
//
//    //��ȡ��Ӧ��DEM��ѡ���Ż�����ʱ��DEM
//    string demPath = "E:\\Ѹ������\\SPC�ܽ�20210120(1)\\SPC�ܽ�20210120\\orthoDEM\\v9\\final.txt";//��ȡ��v9��DEM��ֵ
//    //double **DEM = SFSutils::readDEMAndLandmark(demPath, Cdem);
//    double **DEM = Cdem.ReadDEM(demPath.data(), DEMsize);
//
//    int imageSize = 512;
//    double sun_dist = sqrt(
//            sun_pos_list[165][0] * sun_pos_list[165][0] + sun_pos_list[165][1] * sun_pos_list[165][1] +
//            sun_pos_list[165][2] * sun_pos_list[165][2]);//��Ҫ�޸�
//    double Dstep = CCDsize * sun_dist / (imageSize * f);
//    double L = size * Dstep;
//
//    double dx = CCDsize / imageSize;
//    double dy = CCDsize / imageSize;
//
//    Matrix3d RotMatrix = SFSutils::HPB2Rotation(Vector3d(0, 0, 0));
//    cout << "RotMatrix = " << RotMatrix << endl;
//    Matrix3d K;
//    K << f / dx, 0, 0, 0, f / dy, 0, 0, 0, 1;
//
//    double H, X, Y;
//    Vector3d P0, P1, P2, W, C1, UV1, UV;//�ֱ��Ӧ�ֲ����ꡢ�������ꡢ������ꡢ������ꡢͼ�����ꡢ��������
//    Vector2d S0, S1, S2;
//
//    //���﷽�������ϵ�����DEM��ת��Ϊ��������ϵ�µĵ㣬��������������
//    for (int i = 2; i < size - 2; i++)
//    {
//        for (int j = 2; j < size - 2 - i; j++)
//        {
//            x = j * Dstep - L / 2;
//            y = i * Dstep - L / 2;
//            H = DEM[i][j];
//            Vector3d w = Cdem.V + x * Cdem.u1 + y * Cdem.u2 + H * Cdem.u3;
//            dem3d.push_back(w);
//        }
//    }
//    //���﷽������������߽��ת��Ϊ��������ϵ�µĵ�
//    for (int i = 0; i < 3; i++)
//    {
//        if (i == 0)
//        {
//            x = 2 * Dstep - L / 2;
//            y = 2 * Dstep - L / 2;
//            H = DEM[2][2];
//            P0 << x, y, H;
//        } else if (i == 1)
//        {
//            x = 2 * Dstep - L / 2;
//            y = 238 * Dstep - L / 2;
//            H = DEM[238][2];
//            P1 << x, y, H;
//        } else if (i == 2)
//        {
//            x = 238 * Dstep - L / 2;
//            y = 2 * Dstep - L / 2;
//            H = DEM[2][238];
//            P2 << x, y, H;
//        }
//        //�����������ϵ������
//        W = Cdem.V + x * Cdem.u1 + y * Cdem.u2 + H * Cdem.u3;
//        //cout << "W=:" << W << endl;
//        C1 = (RotMatrix.transpose()) * (W - sun_pos_list[165]);//ת�����������ϵ
//        UV1 = K * C1;//ת����ͼ������ϵ
//        UV = UV1 / UV1(2);
//        X = floor(UV(0) + imageSize / 2);
//        Y = floor(UV(1) + imageSize / 2);//ת������������ϵ
//        if (i == 0)
//            S0 << X, Y;
//        if (i == 1)
//            S1 << X, Y;
//        if (i == 2)
//            S2 << X, Y;
//    }
//    /*cout << "P0: " << P0 << endl;
//    cout << "P1: " << P1 << endl;
//    cout << "P2: " << P2 << endl;
//    cout << "S0: " << S0 << endl;
//    cout << "S1: " << S1 << endl;
//    cout << "S2: " << S2 << endl;*/
//
//    double minleft_x = min(S0(0), S1(0));
//    minleft_x = min(minleft_x, S2(0));
//    double minleft_y = min(S0(1), S1(1));
//    minleft_y = min(minleft_y, S2(1));
//    double maxright_x = max(S0(0), S1(0));
//    maxright_x = max(maxright_x, S2(0));
//    double maxright_y = max(S0(1), S1(1));
//    maxright_y = max(maxright_y, S2(1));
//    cout << minleft_x << " " << minleft_y << " " << maxright_x << " " << maxright_y << endl;
//
//    Vector2d index;
//    int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
//    int yesnum = 0, nonum = 0;
//    Vector3d result;
//    Vector3d WN;
//    double min_theta = 180;
//    double thres_angle = 5;
//    Vector3d min_point;
//    int final = 0;
//    double **resultDEM;
//    resultDEM = new double *[size];
//    for (int i = 0; i < size; i++)
//    {
//        resultDEM[i] = new double[size];
//    }
//    map<double, Vector3d> angles;//����dem��ÿһ����np�ļн�:(dem�������н�)
//
//    for (int i = minleft_y; i <= maxright_y; i++)
//    {
//        for (int j = minleft_x; j <= maxright_x; j++)
//        {
//            index << j, i;
//            tmp1 = (S2 - S1)(0) * (S0 - S1)(1) - (S2 - S1)(1) * (S0 - S1)(0);
//            tmp2 = (S2 - S1)(0) * (index - S1)(1) - (S2 - S1)(1) * (index - S1)(0);
//            tmp3 = (S2 - S0)(0) * (S1 - S0)(1) - (S2 - S0)(1) * (S1 - S0)(0);
//            tmp4 = (S2 - S0)(0) * (index - S0)(1) - (S2 - S0)(1) * (index - S0)(0);
//            tmp5 = (S1 - S0)(0) * (S2 - S0)(1) - (S1 - S0)(1) * (S2 - S0)(0);
//            tmp6 = (S1 - S0)(0) * (index - S0)(1) - (S1 - S0)(1) * (index - S0)(0);
//            if ((tmp1 * tmp2 >= 0) && (tmp3 * tmp4 >= 0) && (tmp5 * tmp6 >= 0))
//            {
//                yesnum++;
//                result << (j - imageSize / 2 + dx), (i - imageSize / 2 + dy), f;//����ƽ���ϵĵ�ת��Ϊ�������ϵ�µĵ�
//                WN = sun_pos_list[165] + result;//��������ϵ�µ����
//                Vector3d sun_pi = (WN - sun_pos_list[165]).normalized();//�ɱ�������ϵ�µ�̫������ָ����������
//                for (int k = 0; k < dem3d.size(); k++)
//                {
//                    Vector3d tmp = dem3d[k] - sun_pos_list[165];//̫��ָ��dem��3ά�������
//                    //����ߵļн�
//                    double tt = tmp.dot(sun_pi) / (sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]));
//                    double theta = acos(tt) * 180 / PI;//acos���ص��ǻ��ȣ�������
//                    if (theta < min_theta)
//                    {
//                        min_theta = theta;
//                        min_point = dem3d[k];
//                        final = k;
//                    }
//                }
//                int res_i = 0, res_j = 0;
//                if (final == 0)
//                    res_i = 0, res_j = 0;
//                if (final == 27966)
//                    res_i = 238, res_j = 0;
//                for (int t = 236; t >= 1; i--)
//                {
//                    if (final > i)
//                    {
//                        res_i++;
//                        final -= t;
//                    } else
//                    {
//                        res_j = final - 1;
//                        break;
//                    }
//                }
//                resultDEM[res_i][res_j] = 255;
//            } else
//            {
//                nonum++;
//            }
//        }
//    }
//    cout << "yesnum = " << yesnum << "nonum=" << nonum << endl;
//    Mat temp_image = Mat(size, size, CV_8U, Scalar::all(0));
//    for (int i = 0; i < size; i++)
//    {
//        for (int j = 0; j < size; j++)
//        {
//            temp_image.at<uchar>(i, j) = resultDEM[i][j];
//        }
//    }
//    imwrite("C:\\Users\\gtf\\Desktop\\result.jpg", temp_image);//��Ҫ�޸�
//    return 0;
//}