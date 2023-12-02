// 제 목 : ch15 - 2 실습 3번
// 날 짜 : 2022년 9월25일
// 작성자 : 1801253 신승일
// **********************************************
#include<iostream> // 헤더파일 포함
#include<opencv2/opencv.hpp> // 헤더파일 포함
using namespace std; // 객체명 앞에 std:: 생략
using namespace cv; // 객체명 앞에 cv:: 생략
using namespace cv::ml; // 객체명 앞에 cv::ml 생략
void on_mouse(int event, int x, int y, int flags, void* userdata); // 마우스 이벤트 핸들러 선언

int main() // 메인 함수
{
	Mat img = Mat::zeros(400, 400, CV_8UC1); //모든 원소가 0인행렬생성
	imshow("img", img); // img 영상 출력
	setMouseCallback("img", on_mouse, (void*)&img); // 마우스 콜백 함수

	for (int n = 0; n < 10; n++) // 반복문
	{
		for (int i = 800; i < 1000; i++) // 반복문
		{
			int c = waitKey(0);  // 키보드 입력
			if (c == 27) return 0; // ESC입력시 종료
			else if (c == ' ') // 스페이스바 입력시
			{
				string file = format("C:\\Users\\Shin\\source\\repos\\HelloCV\\HelloCV\\숫자 학습 데이터\\학습_%d_%d.jpg", n, i);
				// 파일 저장 위치 및 이름 설정
				cout << file << endl; // 결과 메세지 출력
				imwrite(file, img); // 영상 file명으로 저장
				img.setTo(0); // img 영상 초기화
				imshow("img", img); // 입력 윈도우 지움
			}
		}
	}

	return 0; // 함수 종료
}
Point ptPrev(-1, -1); // 포인트 객체 선언
void on_mouse(int event, int x, int y, int flags, void* userdata) // 마우스 이벤트 핸들러 정의
{
	Mat img = *(Mat*)userdata; // 위치 값 이미지에 설정
	if (event == EVENT_LBUTTONDOWN) // 마우스 왼쪽 버튼 누를 경우
	{
		ptPrev = Point(x, y); // 현재 x,y좌표를 ptPrev에 저장
	}
	else if (event == EVENT_LBUTTONUP) // 마우스 왼쪽 버튼 떼는 경우
	{
		ptPrev = Point(-1, -1); // -1,-1 로 ptPrev 초기화
	}
	else if ((event == EVENT_MOUSEMOVE) && (flags & EVENT_FLAG_LBUTTON))
		// 마우스 움직이거나 마우스 왼쪽 버튼이 눌려있는 경우
	{
		line(img, ptPrev, Point(x, y), Scalar::all(255), 20, LINE_AA, 0); // 그리기 함수
		ptPrev = Point(x, y); // 
		imshow("img", img); // img 영상 출력
	}
}