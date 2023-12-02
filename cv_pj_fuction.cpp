#include<iostream> // ��� ���� ����
#include<opencv2/opencv.hpp> // ������� ����
#include<cmath> // ��� ���� ����
using namespace std; // ��ü�� �տ� std:: ����
using namespace cv; // ��ü�� �տ� cv:: ����
using namespace cv::dnn; // ��ü�� �տ� cv::dnn ����
using namespace cv::ml; // ��ü�� �տ� cv::ml ����

vector<Point> find_draw_contours(Mat input1, Mat input2); // �� �ܰ��� ���� �� �׸��� �Լ� ����
void dis_angle(vector<Point> hull, double& Ptrdis1 ,double& Ptrangle1,double& Ptrdis2, double& Ptrangle2);
// �ܰ����� ���� �ٰ��� �� �� ������ �Ÿ��� ���� ���ϴ� �Լ� ����
String hand_shape(double dis1, double angle1, double dis2, double angle2); // �Ÿ��� ������ ���� �� ��� �Ǻ� �Լ� ����

Mat input, handimg, element; // Mat ���� ����
vector<vector<Point>> contours; // �ܰ����� ������ ���� ����
double max_con_index = 0; // �ִ� ũ�� �ܰ��� �ε��� ���� ����
vector<Point>hull; // ���� �ٰ��� ������ ������ ���� ����
vector<Point> save; // ���� �հ����� ��ǥ�� ������ ���� ����
vector<Point>  his; //  ���� �հ����� ��ǥ�� ������ ���� ����
double avr_dis0_1 = 0, avr_dis1_2 = 0, avr_angle0_1 = 0, avr_angle1_2 = 0; // �Ÿ��� ������ ������ ���� ����
String st_hand_shape; // �� ����� ������ ���� ����

int main() // ���� �Լ� 
{

	VideoCapture cap(0); // ��ü ����
	if (!cap.isOpened()) { cerr << "Camera open failed!" << endl; return -1; } // �ַ� ó��


	while (1) // �ݺ���
	{
		cap >> input; // Mat ��ü�� ���� ����
		if (input.empty()) { break; } // �ַ� ó��
		cvtColor(input, handimg, COLOR_BGR2YCrCb); // BGR -> YCrCb�� ��ȯ
		inRange(handimg, Scalar(0, 140, 85), Scalar(255, 170, 130), handimg); // ��� �κ� ����

		element = getStructuringElement(1, Size(7, 7)); // kernel ����
		erode(handimg, handimg, element); // ħ��
		dilate(handimg, handimg, element); // ��â
		flip(input, input, 1); // ���� �¿� ����
		flip(handimg, handimg, 1); // ���� �¿� ����

		find_draw_contours(handimg, input); // �� �ܰ��� ���� �� �׸��� �Լ�
		dis_angle(hull, avr_dis0_1, avr_angle0_1, avr_dis1_2, avr_angle1_2); // �ܰ����� ���� �ٰ��� �� �� ������ �Ÿ��� ���� ���ϴ� �Լ� 
		hand_shape(avr_dis0_1, avr_angle0_1, avr_dis1_2, avr_angle1_2); // �Ÿ��� ������ ���� �� ��� �Ǻ� �Լ�
		if (st_hand_shape == "delete_hand") // �� ����� ���ڱ� ����� ���
		{
			his.clear(); // his�� ��� ��� ����
			save.clear(); // save�� ��� ��� ���
		}
		else if (st_hand_shape == "draw_hand") // �� ����� ������ �� ���
		{
			int i = 0; // Ƚ���� ī��Ʈ
			if (i == 0) // ó�� ������ ���
			{
				his.push_back(hull[0]); // his�� hull[0]�� ��ǥ ���� ����
			}
			save.push_back(hull[0]); // savce�� hull[0]�� ��ǥ ���� ����
			for (int d = 0; d < his.size() - 1; d++) line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
			// ���� �հ��� ���� ��ǥ�� ���� ��ǥ�� �̾� ��� �� �׸���
			i++; // i�� 1����
		}
		else if (st_hand_shape == "save_hand") // �� ����� �ָ� �� ���
		{
			if (his.size() <= 1) imshow("input", input); // ���� �հ��� ���� ��ǥ�� ���� ���
			else
			{
				for (int d = 0; d < his.size() - 1; d++) line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
				// ���� �հ��� ���� ��ǥ�� ���� ��ǥ�� �̾� ��� �� �׸���
				imwrite("hand_number.jpg", input); // hand_number.jpg�� ���� ����
				break; // �ݺ��� ����
			}
		}
		else // ���� ��Ȳ���� �ƴ� ���
		{
			if (his.size() <= 1) imshow("input", input); // ���� �հ��� ���� ��ǥ�� ���� ���
			else for (int d = 0; d < his.size() - 1; d++) line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
			// ���� �հ��� ���� ��ǥ�� ���� ��ǥ�� �̾� ��� �� �׸���
		}

		max_con_index = 0; // �ִ� ũ���� �ܰ��� �ε��� �ʱ�ȭ
		imshow("input", input); // input ���� ���
		imshow("hand", handimg); // handimg ���� ���
		if (waitKey(1) == 27) break; // esc �Է½� �ݺ��� ����
	}
	TickMeter tm; // �ð� ������ �ʿ��� ���� 
	tm.start(); // �ð� ���� ����
	vector<string> classNames = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" }; // String�� Ŭ���� ����
	Net net = readNet("hand_number_model5.pb"); // �н���, ���������� �ҷ���
	if (net.empty()) { cerr << "Network load failed!" << endl; return -1; } // �ַ� ó��

	double maxVal; // �Ǽ��� ������ ����
	Point maxLoc; // Point�� ������ ����

	Mat hand_result = imread("hand_number.jpg"); // hand_number.jpg�� hand_result�� �ҷ���
	inRange(hand_result, Scalar(250, 250, 250), Scalar(255, 255, 255), hand_result); // ��� ���� �κ� ����
	element = getStructuringElement(1, Size(3, 3)); // kernel ����
	erode(hand_result, hand_result, element); // ħ��
	dilate(hand_result, hand_result, element); // ��â
	cvtColor(hand_result, hand_result, COLOR_GRAY2BGR); // ���->�÷�(BGR)�� ��ȯ
	imwrite("hand_number.jpg", hand_result); // ��� ���� hand_number.jpg�� ����

	Mat inputBlob = blobFromImage(hand_result, 1.0 / 127.5, Size(224, 224), Scalar(-1, -1, -1)); // �Է� ������ blob ��ķ� ��ȯ
	net.setInput(inputBlob); // ��Ʈ��ũ �׽�Ʈ �Է� ����
	Mat prob = net.forward(); // ����(�߷�) ����
	minMaxLoc(prob, NULL, &maxVal, NULL, &maxLoc); // �ִ밪�� �� ������ġ ã�� �Լ�
	String str = classNames[maxLoc.x] + format("(%4.2lf%%)", maxVal * 100); // �߷� ����� �ۼ�Ʈ�� ��Ÿ�� ���ڿ�
	putText(hand_result, str, Point(10, 50), FONT_HERSHEY_SIMPLEX, 2.0, Scalar(0, 0, 255)); // ahnd_result���� ��� ���ڿ� �Է�
	imshow("result", hand_result); // hand_result ���� ���
	tm.stop(); // �ð� ���� ����
	cout << "FPS: " << 1 / tm.getAvgTimeSec() << endl; // fps ��� ���
	waitKey(); // Ű���� �Է� ���
	return 0; // �Լ� ����
}

vector<Point> find_draw_contours(Mat input1, Mat input2) // �� �ܰ��� ���� �� �׸��� �Լ� ����
{
	double max = 0; // �ִ� ��ü�� ������ ������ �Ǽ��� ����

	findContours(input1, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); // �ܰ��� ����
	for (int i = 0; i < contours.size(); i++) // �ݺ���
	{
		if (contourArea(contours[i]) > contourArea(contours[max_con_index])) // ���� ��ü�� ���� �ִ밪���� ������ ū ���
		{
			max_con_index = i; // �ִ� ��ü �ε��� ����
			max = contourArea(contours[i]); // �ִ� ��ü ���� ����
		}
	}

	approxPolyDP(contours[max_con_index], contours[max_con_index], arcLength(contours[max_con_index], true) * 0.03, false);
	// �ܰ��� �ٻ�ȭ
	convexHull(contours[max_con_index], hull); // �ܰ����� ��� ���� �����ϴ� �ּ�ũ�� ���� �ٰ��� ���� 
	for (int j = 0; j < hull.size() - 1; j++) // �ݺ���
	{
		line(input2, hull[j], hull[j + 1], Scalar(255, 255, 0), 3); // ���� �ٰ��� �׸���
	}
	circle(input2, hull[0], 3, Scalar(255, 0, 0), 10); // �հ��� �� �� �׸���

	return hull; // vector<Point>������ ����
}

void dis_angle(vector<Point> hull, double& Ptrdis1, double& Ptrangle1,double& Ptrdis2, double& Ptrangle2)
// �ܰ����� ���� �ٰ��� �� �� ������ �Ÿ��� ���� ���ϴ� �Լ� ����
{
	double dis0_1 = sqrt(pow(hull[0].x - hull[1].x, 2) + pow(hull[0].y - hull[1].y, 2)); // hull[0]~hull[1] ���� �Ÿ�
	double dis1_2 = sqrt(pow(hull[1].x - hull[2].x, 2) + pow(hull[1].y - hull[2].y, 2)); // hull[1]~hull[2] ���� �Ÿ�
	double angle0_1 = atan2(hull[1].y - hull[0].y, hull[1].x - hull[0].x); // hull[0]~hull[1] ���� ����
	double angle1_2 = atan2(hull[2].y - hull[1].y, hull[2].x - hull[1].x); // hull[1]~hull[2] ���� ����
	double add_dis0_1 = 0, add_dis1_2 = 0; // �Ÿ� ���� ������ ������ ����
	double add_angle0_1 = 0, add_angle1_2 = 0; // ���� ���� ������ ������ ����
	for (int i = 0; i < 10000; i++) // �ݺ���
	{
		add_dis0_1 += dis0_1; // 1000���� hull[0]~hull[1] ���� �Ÿ� ��
		add_dis1_2 += dis1_2; // 1000���� hull[1]~hull[2] ���� �Ÿ� ��
		add_angle0_1 += angle0_1; // 1000���� hull[0]~hull[1] ���� ���� ��
		add_angle1_2 += angle1_2; // 1000���� hull[1]~hull[2] ���� ���� ��
	}
	double avr_dis0_1 = add_dis0_1 / 10000, avr_dis1_2 = add_dis1_2 / 10000; 
	// 1000�� hull[0]~hull[1] �Ÿ� ��� ��, 1000�� hull[1]~hull[2] �Ÿ� ��� ��
	double avr_angle0_1 = add_angle0_1 / 10000, avr_angle1_2 = add_angle1_2 / 10000;
	// 1000�� hull[0]~hull[1] ���� ��� ��, 1000�� hull[1]~hull[2] ���� ��� ��
	Ptrdis1 = avr_dis0_1, Ptrangle1 = avr_angle0_1, Ptrdis2 = avr_dis1_2, Ptrangle2 = avr_angle1_2; // �����ڸ� ����� ���� ����
}

String hand_shape(double dis1, double angle1, double dis2, double angle2) // �Ÿ��� ������ ���� �� ��� �Ǻ� �Լ� ����
{
	if ((dis1 > 20 && dis1 < 70) && (dis2 > 60 && dis2 < 85) && (angle1 > 0.3 && angle1 < 0.6) && (angle1 > 0.5 && angle1 < 0.9))
		// hull[0]~hull[1] ���� �Ÿ�: 20~70, hull[0]~hull[1] ���� ����: 0.3~0.6, hull[1]~hull[2] ���� �Ÿ�: 60~85, hull[1]~hull[2] ���� �㵵: 0.5~0.9
	{
		st_hand_shape = "delete_hand"; // �����ϴ� �� ���

	}

	else if ((dis1 >= 120 && dis1 <= 175) && (dis2 >= 50 && dis2 <= 140) && (angle1 > 0.9 && angle1 < 1.1) && (angle2 > 1.9))
		// hull[0]~hull[1] ���� �Ÿ�: 120~175, hull[0]~hull[1] ���� ����: 0.9~1.1, hull[1]~hull[2] ���� �Ÿ�: 50~140, hull[1]~hull[2] ���� �㵵: 1.9�̻�
	{
		st_hand_shape = "draw_hand"; // �׸��� �� ���
	}
	else if ((dis1 > 85 && dis1 <= 100) && (angle1 > 0.2 && angle1 < 0.55) && (angle2 > 1.5))
		// hull[0]~hull[1] ���� �Ÿ�: 85~100, hull[0]~hull[1] ���� ����: 0.2~0.55, hull[1]~hull[2] ���� �㵵: 1.5 �̻�
	{
		st_hand_shape = "save_hand"; // �����ϴ� �� ���
	}
	else st_hand_shape = "none"; // ���� ��찡 �ƴ� ���

	return st_hand_shape; // string������ ����
}