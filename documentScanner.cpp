#include<opencv2/opencv.hpp>
#include<iostream>

typedef cv::Mat mat;

mat imgOrig, imgBlur , imghsv, imgGray, imgthres,imgcany ,imgDil, imgErrode;

mat preprocess(mat img)
{
    cv::cvtColor(img,imgGray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(imgGray, imgBlur,cv::Size(3,3),3, 0);
    cv::Canny(imgBlur, imgcany, 25, 75);

    mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::dilate(imgcany, imgDil, kernel);

    return imgDil;
}

int main(int argc, char** argv)
{
    imgOrig = cv::imread(argv[1], -1);
    cv::resize(imgOrig, imgOrig, cv::Size(),0.5,0.5);
    cv::imshow("ShoW", imgOrig);

    mat imgthres = preprocess(imgOrig);
    cv::imshow("ShoW Dilate", imgthres);


    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}