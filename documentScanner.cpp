#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

typedef cv::Mat mat;

mat imgOrig, imgBlur , imghsv, imgwrap , imgGray, imgthres,imgcany ,imgDil, imgcrop;
std::vector<cv::Point> initialPoint, docpoint;

float w=420, h = 596;

mat preprocess(mat img)
{
    cv::cvtColor(img,imgGray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(imgGray, imgBlur,cv::Size(3,3),3, 0);
    cv::Canny(imgBlur, imgcany, 25, 75);

    mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::dilate(imgcany, imgDil, kernel);

    return imgDil;
}


std::vector<cv::Point> getContours(cv::Mat img)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    // cv::findContours();
    cv::findContours(img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int area=0;
    int maxarea=0;
    std::vector<std::vector<cv::Point>> conpoly(contours.size());
    std::vector<cv::Rect> boundrect(contours.size());
    std::string objType{};
    std::vector<cv::Point> biggest;
    for(int i=0; i< contours.size(); ++i)
    {
        area = cv::contourArea(contours[i]);
        std::cout<<"Area = "<<area<<std::endl;
        if( area > 500)
        {
            float peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conpoly[i], 0.02*peri, true);
          
            if(area > maxarea && conpoly[i].size() == 4)
            {
                cv::drawContours(imgOrig, conpoly, i , cv::Scalar(0,0,255), 3);    

                biggest ={conpoly[i][0], conpoly[i][1], conpoly[i][2], conpoly[i][3]};
                maxarea = area;
            }
          //  cv::rectangle(frame, boundrect[i].tl(), boundrect[i].br(),cv::Scalar(255,0,0), 5);
            
        }
    }

    return biggest;
}


void drawPoints(std::vector<cv::Point> points, cv::Scalar color)
{
    for( int i=0; i < points.size(); ++i)
    {
        cv::circle(imgOrig, points[i],5 , color, cv::FILLED);
        cv::putText(imgOrig, std::to_string(i), points[i], cv::FONT_HERSHEY_PLAIN,2, color, 2 );

    }


}


std::vector<cv::Point> reorder(std::vector<cv::Point> points)
{
    std::vector<cv::Point> newPoints;
    std::vector<int>  sumPoints, subPoints;

    for(size_t i =0; i< points.size(); ++i)
    {
        sumPoints.push_back( points[i].x + points[i].y);
        subPoints.push_back( points[i].x - points[i].y);
    }

    newPoints.push_back(points[min_element(sumPoints.begin(),sumPoints.end()) - sumPoints.begin()]);
    newPoints.push_back(points[max_element(subPoints.begin(),subPoints.end()) - subPoints.begin()]);
    newPoints.push_back(points[min_element(subPoints.begin(),subPoints.end()) - subPoints.begin()]);
    newPoints.push_back(points[max_element(sumPoints.begin(),sumPoints.end()) - sumPoints.begin()]);

    return newPoints;
}

mat getwrap(mat img, std::vector<cv::Point> points, float w, float h)
{

    cv::Point2f src[4] = {points[0], points[1],points[2],points[3] };
    cv::Point2f dst[4] = { {0.0f, 0.0f}, {w, 0.0f},{0.0f, h}, {w,h}};

    mat matrix = cv::getPerspectiveTransform(src,dst);
    cv::warpPerspective(img, imgwrap, matrix, cv::Point(w, h) );

    // for(size_t i=0; i<4; ++i)
    // {
    //     cv::circle(img, src[i], 10, cv::Scalar(0,0,255), cv::FILLED);
    // }

    return imgwrap;
}

int main(int argc, char** argv)
{
    imgOrig = cv::imread(argv[1], -1);
    cv::resize(imgOrig, imgOrig, cv::Size(),0.5,0.5);
    // cv::VideoCapture cap;
    // mat frame;
    // cap.open(0);
    // if( !cap.isOpened() ) return 0;
    int crop =5;

    while (true)
    {
        // cap >> imgOrig;
        if(imgOrig.empty() ) break;
            /********** Preprocess *************/
        imgthres = preprocess(imgOrig);

        /***********Get Contours************/
        initialPoint = getContours(imgthres);
        std::cout<<"Initial Point [  "<<initialPoint<<"   ]"<<std::endl;
        
        drawPoints(initialPoint, cv::Scalar(0,0,255));
        docpoint = reorder(initialPoint);
        drawPoints(docpoint,cv::Scalar(0,255,0));
        
        /*********Wrap***********/
        imgwrap = getwrap(imgOrig, docpoint, w, h); //change frame to imOrig for single image

        cv::Rect roi(crop, crop, w-(2*crop),h -(2*crop) );
        imgcrop = imgwrap(roi);

        cv::imshow("ShoW", imgOrig);

        cv::imshow("ShoW wrap", imgwrap);

        cv::imshow("ShoW crop", imgcrop);


        char c = static_cast<char>(cv::waitKey(100000));
        if(c == 27)
            break;
    }
    

    
    cv::destroyAllWindows();
    return 0;
}