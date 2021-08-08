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
//    path:保存landmarks.txt的文件路径
//    landmarks:保存最终读取的结果
//    scale:cinema4d里面的模型尺度
//    */
//    ifstream infile;
//    infile.open(path);
//    if (!infile)
//    {
//        cout << "open file error!" << endl;
//    }
//    //保存的landmarks.txt前三行是说明，
//    //第四行是保存的landmark个数，从第5行开始是landmark
//    string s;
//    getline(infile, s);
//    getline(infile, s);
//    getline(infile, s);
//    int num;
//    infile >> num;
//    double x, y, z;
//    while (!infile.eof())
//    {
//        //读取的点坐标是m，记得换算尺度
//        infile >> x >> y >> z;
//        infile.get(); // 读取最后的回车符
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
////以序号为165的相机参数开始
//int main()
//{
//    vector<Vector3d> sun_pos_list;
//    vector<Vector3d> cam_pos_list;
//    vector<Vector3d> cam_hpb_list;
//    vector<Vector3d> dem3d;
//    vector<int> indexList;//保存图片数量
//    string camPath = "E:\\迅雷下载\\SPC总结20210120(1)\\SPC总结20210120\\camPos.txt";
//    string sunPath = "E:\\迅雷下载\\SPC总结20210120(1)\\SPC总结20210120\\sunPos.txt";
//    int size = 240;
//
//    //读取所有相机参数并转换到右手系
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
//        infile.get(); // 读取最后的回车符
//        if (infile.peek() == '\n')
//            break;
//        Vector3d camPos(x, y, z);
//        SFSutils::convertPoint2Right(camPos);
//        cam_pos_list.push_back(camPos);
//        cam_hpb_list.push_back(Vector3d(h, p, b));
//    }
//    infile.close();
//
//    //读取所有太阳参数并转换到右手系
//    ifstream infile2;
//    infile2.open(sunPath.c_str());
//    if (!infile2)
//    {
//        cout << "open file error!" << endl;
//    }
//    while (!infile2.eof())
//    {
//        infile2 >> cF >> x >> y >> z;
//        infile2.get(); // 读取最后的回车符
//        if (infile2.peek() == '\n')
//            break;
//        Vector3d sunPos(x, y, z);
//        SFSutils::convertPoint2Right(sunPos);
//        sun_pos_list.push_back(sunPos);
//    }
//    infile2.close();
//
//    //读取所有landmark点坐标
//    string landmarkPath = "E:\\迅雷下载\\SPC总结20210120(1)\\SPC总结20210120\\landmarks.txt";
//    vector<Vector3d> landmarks;
//    int scale = 1;
//    readLandmarks(landmarkPath.c_str(), landmarks, scale);//将landmark点坐标保存到landmarks容器里面
//    //读取landmark三维坐标
//    Vector3d V = landmarks[8];//需要修改
//    CDEM Cdem = CDEM(V);
//    cout << "V = " << Cdem.V << endl;
//
//    //读取对应的DEM，选择优化结束时的DEM
//    string demPath = "E:\\迅雷下载\\SPC总结20210120(1)\\SPC总结20210120\\orthoDEM\\v9\\final.txt";//读取第v9个DEM的值
//    //double **DEM = SFSutils::readDEMAndLandmark(demPath, Cdem);
//    double **DEM = Cdem.ReadDEM(demPath.data(), DEMsize);
//
//    int imageSize = 512;
//    double sun_dist = sqrt(
//            sun_pos_list[165][0] * sun_pos_list[165][0] + sun_pos_list[165][1] * sun_pos_list[165][1] +
//            sun_pos_list[165][2] * sun_pos_list[165][2]);//需要修改
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
//    Vector3d P0, P1, P2, W, C1, UV1, UV;//分别对应局部坐标、世界坐标、相机坐标、齐次坐标、图像坐标、像素坐标
//    Vector2d S0, S1, S2;
//
//    //将物方三角面上的所有DEM点转化为本体坐标系下的点，并保存至容器中
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
//    //将物方三角面的三个边界点转换为像素坐标系下的点
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
//        //求出本体坐标系下坐标
//        W = Cdem.V + x * Cdem.u1 + y * Cdem.u2 + H * Cdem.u3;
//        //cout << "W=:" << W << endl;
//        C1 = (RotMatrix.transpose()) * (W - sun_pos_list[165]);//转换到相机坐标系
//        UV1 = K * C1;//转换到图像坐标系
//        UV = UV1 / UV1(2);
//        X = floor(UV(0) + imageSize / 2);
//        Y = floor(UV(1) + imageSize / 2);//转换到像素坐标系
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
//    map<double, Vector3d> angles;//保存dem的每一点与np的夹角:(dem索引，夹角)
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
//                result << (j - imageSize / 2 + dx), (i - imageSize / 2 + dy), f;//将像平面上的点转换为相机坐标系下的点
//                WN = sun_pos_list[165] + result;//本体坐标系下的像点
//                Vector3d sun_pi = (WN - sun_pos_list[165]).normalized();//由本体坐标系下的太阳坐标指向像点的向量
//                for (int k = 0; k < dem3d.size(); k++)
//                {
//                    Vector3d tmp = dem3d[k] - sun_pos_list[165];//太阳指向dem上3维点的向量
//                    //求二者的夹角
//                    double tt = tmp.dot(sun_pi) / (sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]));
//                    double theta = acos(tt) * 180 / PI;//acos返回的是弧度！！！！
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
//    imwrite("C:\\Users\\gtf\\Desktop\\result.jpg", temp_image);//需要修改
//    return 0;
//}