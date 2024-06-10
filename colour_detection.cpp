#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
vector<vector<int>>newPoints;

vector<vector<int>>myColour{ {80,68,69,164,255,255},//purple
							 {45,97,0,108,255,255} //green 
                              };

vector<Scalar> myColourValue{ {255,0,255},//purple
	                          {0,255,0} //green 
                               };

Point getContour(Mat imgDil ) {

	vector<vector<Point>>contours;
	vector<Vec4i>hierarcy;

	findContours(imgDil, contours, hierarcy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	/*drawContours(img, contours, -1, Scalar(255, 0, 255), 2);*/

	vector<vector<Point>>comPoly(contours.size());
	vector<Rect>boundRect(contours.size());
	
	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++) {
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 2000) {
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], comPoly[i], 0.02 * peri, true);

			cout << comPoly[i].size() << endl;
			boundRect[i] = boundingRect(comPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, contours, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2);
		}
	}
	return myPoint;
}


vector<vector<int>> findcolour(Mat img) {

	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV); 

	for (int i = 0; i < myColour.size(); i++) {

		Scalar lower(myColour[i][0], myColour[i][1], myColour[i][2]);
		Scalar upper(myColour[i][3], myColour[i][4], myColour[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);

		Point myPoint= getContour(mask);
		if (myPoint.x != 0 && myPoint.y != 0) {

			newPoints.push_back({ myPoint.x,myPoint.y,i });
		}

	}
	return newPoints;
}


void drawOnCanvas(vector<vector<int>>newPoints, vector<Scalar> myColourValue) {

	for (int i = 0; i < newPoints.size(); i++) {

		circle(img, (Point(newPoints[i][0], newPoints[i][1])), 10, myColourValue[newPoints[i][2]], FILLED);

	}

}

 
void main() {

	VideoCapture cap(0);


	while (true) {

		cap.read(img);
		findcolour(img);
		drawOnCanvas(newPoints,myColourValue);
		imshow("Image", img);

		waitKey(1);
	}
}
