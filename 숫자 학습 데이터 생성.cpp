// �� �� : ch15 - 2 �ǽ� 3��
// �� ¥ : 2022�� 9��25��
// �ۼ��� : 1801253 �Ž���
// **********************************************
#include<iostream> // ������� ����
#include<opencv2/opencv.hpp> // ������� ����
using namespace std; // ��ü�� �տ� std:: ����
using namespace cv; // ��ü�� �տ� cv:: ����
using namespace cv::ml; // ��ü�� �տ� cv::ml ����
void on_mouse(int event, int x, int y, int flags, void* userdata); // ���콺 �̺�Ʈ �ڵ鷯 ����

int main() // ���� �Լ�
{
	Mat img = Mat::zeros(400, 400, CV_8UC1); //��� ���Ұ� 0����Ļ���
	imshow("img", img); // img ���� ���
	setMouseCallback("img", on_mouse, (void*)&img); // ���콺 �ݹ� �Լ�

	for (int n = 0; n < 10; n++) // �ݺ���
	{
		for (int i = 800; i < 1000; i++) // �ݺ���
		{
			int c = waitKey(0);  // Ű���� �Է�
			if (c == 27) return 0; // ESC�Է½� ����
			else if (c == ' ') // �����̽��� �Է½�
			{
				string file = format("C:\\Users\\Shin\\source\\repos\\HelloCV\\HelloCV\\���� �н� ������\\�н�_%d_%d.jpg", n, i);
				// ���� ���� ��ġ �� �̸� ����
				cout << file << endl; // ��� �޼��� ���
				imwrite(file, img); // ���� file������ ����
				img.setTo(0); // img ���� �ʱ�ȭ
				imshow("img", img); // �Է� ������ ����
			}
		}
	}

	return 0; // �Լ� ����
}
Point ptPrev(-1, -1); // ����Ʈ ��ü ����
void on_mouse(int event, int x, int y, int flags, void* userdata) // ���콺 �̺�Ʈ �ڵ鷯 ����
{
	Mat img = *(Mat*)userdata; // ��ġ �� �̹����� ����
	if (event == EVENT_LBUTTONDOWN) // ���콺 ���� ��ư ���� ���
	{
		ptPrev = Point(x, y); // ���� x,y��ǥ�� ptPrev�� ����
	}
	else if (event == EVENT_LBUTTONUP) // ���콺 ���� ��ư ���� ���
	{
		ptPrev = Point(-1, -1); // -1,-1 �� ptPrev �ʱ�ȭ
	}
	else if ((event == EVENT_MOUSEMOVE) && (flags & EVENT_FLAG_LBUTTON))
		// ���콺 �����̰ų� ���콺 ���� ��ư�� �����ִ� ���
	{
		line(img, ptPrev, Point(x, y), Scalar::all(255), 20, LINE_AA, 0); // �׸��� �Լ�
		ptPrev = Point(x, y); // 
		imshow("img", img); // img ���� ���
	}
}