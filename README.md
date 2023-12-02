※ 가상 숫자 입력기
---------------------------------------------------------------------------------------------------------------
1. 학습 데이터
0~9 각 1000장 -> 10000장
![image](https://user-images.githubusercontent.com/103232864/205635259-714df3c5-1003-47eb-9cf4-e4f491a75757.png)
(깃허브에 따로 올려 둠)
---------------------------------------------------------------------------------------------------------------
2. 동작 영상
* 숫자 그리기

https://user-images.githubusercontent.com/103232864/205667563-d4628fcb-66df-49bf-b069-57d336262e2f.mp4

* 숫자 지우기

https://user-images.githubusercontent.com/103232864/205667637-58f2771f-54c1-4c2f-89e8-bd45238f5dfc.mp4

----------------------------------------------------------------------------------------------------------------
3.전체 소스 코드
* 메인 함수
<pre>
<code>
#include<iostream> // 헤더 파일 포함
#include<opencv2/opencv.hpp> // 헤더파일 포함
#include<cmath> // 헤더 파일 포함
using namespace std; // 객체명 앞에 std:: 생략
using namespace cv; // 객체명 앞에 cv:: 생략
using namespace cv::dnn; // 객체명 앞에 cv::dnn 생략
using namespace cv::ml; // 객체명 앞에 cv::ml 생략

vector<Point> find_draw_contours(Mat input1, Mat input2); // 손 외곽선 검출 및 그리기 함수 선언
void dis_angle(vector<Point> hull, double& Ptrdis1 ,double& Ptrangle1,double& Ptrdis2, double& Ptrangle2);
// 외곽선의 볼록 다각형 끝 점 사이의 거리와 각도 구하는 함수 선언
String hand_shape(double dis1, double angle1, double dis2, double angle2); // 거리와 각도에 따른 손 모양 판별 함수 선언

Mat input, handimg, element; // Mat 전역 번수
vector<vector<Point>> contours; // 외곽선을 저장할 전역 변수
double max_con_index = 0; // 최대 크기 외곽선 인덱스 전역 변수
vector<Point>hull; // 볼록 다각형 점들을 저장할 전역 변수
vector<Point> save; // 현재 손가락의 좌표를 저장할 전역 변수
vector<Point>  his; //  과거 손가락의 좌표를 저장할 전역 변수
double avr_dis0_1 = 0, avr_dis1_2 = 0, avr_angle0_1 = 0, avr_angle1_2 = 0; // 거리와 각도를 저장할 전역 변수
String st_hand_shape; // 손 모양을 저장할 전역 변수

int main() // 메인 함수 
{

	VideoCapture cap(0); // 객체 생성
	if (!cap.isOpened()) { cerr << "Camera open failed!" << endl; return -1; } // 애러 처리


	while (1) // 반복문
	{
		cap >> input; // Mat 객체에 영상 저장
		if (input.empty()) { break; } // 애러 처리
		cvtColor(input, handimg, COLOR_BGR2YCrCb); // BGR -> YCrCb로 변환
		inRange(handimg, Scalar(0, 140, 85), Scalar(255, 170, 130), handimg); // 살색 부분 추출

		element = getStructuringElement(1, Size(7, 7)); // kernel 생성
		erode(handimg, handimg, element); // 침식
		dilate(handimg, handimg, element); // 팽창
		flip(input, input, 1); // 영상 좌우 반전
		flip(handimg, handimg, 1); // 영상 좌우 반전

		find_draw_contours(handimg, input); // 손 외곽선 검출 및 그리기 함수
		dis_angle(hull, avr_dis0_1, avr_angle0_1, avr_dis1_2, avr_angle1_2); // 외곽선의 볼록 다각형 끝 점 사이의 거리와 각도 구하는 함수 
		hand_shape(avr_dis0_1, avr_angle0_1, avr_dis1_2, avr_angle1_2); // 거리와 각도에 따른 손 모양 판별 함수
		if (st_hand_shape == "delete_hand") // 손 모양이 보자기 모양인 경우
		{
			his.clear(); // his의 모든 요소 제거
			save.clear(); // save의 모든 요소 재거
		}
		else if (st_hand_shape == "draw_hand") // 손 모양이 검지만 핀 경우
		{
			int i = 0; // 횟수를 카운트
			if (i == 0) // 처음 실행할 경우
			{
				his.push_back(hull[0]); // his에 hull[0]의 좌표 값을 넣음
			}
			save.push_back(hull[0]); // savce에 hull[0]의 좌표 값을 넣음
			for (int d = 0; d < his.size() - 1; d++) 
				line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
				// 검지 손가락 현재 좌표와 과거 좌표를 이어 흰색 선 그리기
			i++; // i값 1증가
		}
		else if (st_hand_shape == "save_hand") // 손 모양이 주먹 쥔 경우
		{
			if (his.size() <= 1) imshow("input", input); // 검지 손가락 과거 좌표가 없을 경우
			else
			{
				for (int d = 0; d < his.size() - 1; d++) 
					line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
					// 검지 손가락 현재 좌표와 과거 좌표를 이어 흰색 선 그리기
				imwrite("hand_number.jpg", input); // hand_number.jpg로 영상 저장
				break; // 반복문 종료
			}
		}
		else // 위의 상황들이 아닌 경우
		  {
			if (his.size() <= 1) imshow("input", input); // 검지 손가락 과거 좌표가 없을 경우
			else for (int d = 0; d < his.size() - 1; d++) 
				line(input, Point(save[d].x, save[d].y), Point(his[d].x, his[d].y), Scalar(255, 255, 255), 20);
				// 검지 손가락 현재 좌표와 과거 좌표를 이어 흰색 선 그리기
		  }

		max_con_index = 0; // 최대 크기의 외곽선 인덱스 초기화
		imshow("input", input); // input 영상 출력
		imshow("hand", handimg); // handimg 영상 출력
		if (waitKey(1) == 27) break; // esc 입력시 반복문 종료
	}
	TickMeter tm; // 시간 측정에 필요한 변수 
	tm.start(); // 시간 측정 시작
	vector<string> classNames = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" }; // String형 클레스 백터
	Net net = readNet("hand_number_model5.pb"); // 학습모델, 구성파일을 불러옴
	if (net.empty()) { cerr << "Network load failed!" << endl; return -1; } // 애러 처리

	double maxVal; // 실수를 선언할 변수
	Point maxLoc; // Point를 선언할 변수

	Mat hand_result = imread("hand_number.jpg"); // hand_number.jpg를 hand_result로 불러옴
	inRange(hand_result, Scalar(250, 250, 250), Scalar(255, 255, 255), hand_result); // 흰색 숫자 부분 추출
	
	element = getStructuringElement(1, Size(3, 3)); // kernel 생성
	erode(hand_result, hand_result, element); // 침식
	dilate(hand_result, hand_result, element); // 팽창
	
	cvtColor(hand_result, hand_result, COLOR_GRAY2BGR); // 흑백->컬러(BGR)로 변환
	imwrite("hand_number.jpg", hand_result); // 결과 영상 hand_number.jpg로 저장

	Mat inputBlob = blobFromImage(hand_result, 1.0 / 127.5, Size(224, 224), Scalar(-1, -1, -1)); // 입력 영상을 blob 행렬로 변환
	net.setInput(inputBlob); // 네트워크 테스트 입력 설정
	Mat prob = net.forward(); // 예측(추론) 실행
	
	minMaxLoc(prob, NULL, &maxVal, NULL, &maxLoc); // 최대값과 그 저장위치 찾는 함수
	String str = classNames[maxLoc.x] + format("(%4.2lf%%)", maxVal * 100); // 추론 결과와 퍼센트를 나타낼 문자열
	putText(hand_result, str, Point(10, 50), FONT_HERSHEY_SIMPLEX, 2.0, Scalar(0, 0, 255)); // hand_result영상에 결과 문자열 입력
	imshow("result", hand_result); // hand_result 영상 출력
	tm.stop(); // 시간 측정 종료
	
	cout << "FPS: " << 1 / tm.getAvgTimeSec() << endl; // fps 결과 출력
	waitKey(); // 키보드 입력 대기
	return 0; // 함수 종료
}
</code>
</pre>

* 손 외곽선 검출 및 그리기 함수
<pre>
<code>
vector<Point> find_draw_contours(Mat input1, Mat input2) // 손 외곽선 검출 및 그리기 함수 정의
{
	double max = 0; // 최대 객체의 면적을 저장할 실수형 변수

	findContours(input1, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); // 외곽선 검출
	for (int i = 0; i < contours.size(); i++) // 반복문
	{
		if (contourArea(contours[i]) > contourArea(contours[max_con_index])) // 현재 객체가 현재 최대값보다 면적이 큰 경우
		{
			max_con_index = i; // 최대 객체 인덱스 변경
			max = contourArea(contours[i]); // 최대 객체 면적 변경
		}
	}

	approxPolyDP(contours[max_con_index], contours[max_con_index], arcLength(contours[max_con_index], true) * 0.03, false);
	// 외곽선 근사화
	convexHull(contours[max_con_index], hull); // 외곽선의 모든 점을 포함하는 최소크기 볼록 다각형 검출 
	for (int j = 0; j < hull.size() - 1; j++) // 반복문
	{
		line(input2, hull[j], hull[j + 1], Scalar(255, 255, 0), 3); // 볼록 다각형 그리기
	}
	circle(input2, hull[0], 3, Scalar(255, 0, 0), 10); // 손가락 끝 점 그리기

	return hull; // vector<Point>형으로 리턴
}
</code>
</pre>

*외곽선의 볼록 다각형 끝 점 사이의 거리와 각도 구하는 함수
<pre>
<code>
void dis_angle(vector<Point> hull, double& Ptrdis1, double& Ptrangle1,double& Ptrdis2, double& Ptrangle2)
// 외곽선의 볼록 다각형 끝 점 사이의 거리와 각도 구하는 함수 정의
{
	double dis0_1 = sqrt(pow(hull[0].x - hull[1].x, 2) + pow(hull[0].y - hull[1].y, 2)); // hull[0]~hull[1] 사이 거리
	double dis1_2 = sqrt(pow(hull[1].x - hull[2].x, 2) + pow(hull[1].y - hull[2].y, 2)); // hull[1]~hull[2] 사이 거리
	double angle0_1 = atan2(hull[1].y - hull[0].y, hull[1].x - hull[0].x); // hull[0]~hull[1] 사이 각도
	double angle1_2 = atan2(hull[2].y - hull[1].y, hull[2].x - hull[1].x); // hull[1]~hull[2] 사이 각도
	double add_dis0_1 = 0, add_dis1_2 = 0; // 거리 값의 총합을 저장할 변수
	double add_angle0_1 = 0, add_angle1_2 = 0; // 각도 값의 총합을 저장할 변수
	
	for (int i = 0; i < 10000; i++) // 반복문
	{
		add_dis0_1 += dis0_1; // 1000개의 hull[0]~hull[1] 사이 거리 합
		add_dis1_2 += dis1_2; // 1000개의 hull[1]~hull[2] 사이 거리 합
		add_angle0_1 += angle0_1; // 1000개의 hull[0]~hull[1] 사이 각도 합
		add_angle1_2 += angle1_2; // 1000개의 hull[1]~hull[2] 사이 각도 합
	}
	
	double avr_dis0_1 = add_dis0_1 / 10000, avr_dis1_2 = add_dis1_2 / 10000; 
	// 1000개 hull[0]~hull[1] 거리 평균 값, 1000개 hull[1]~hull[2] 거리 평균 값
	double avr_angle0_1 = add_angle0_1 / 10000, avr_angle1_2 = add_angle1_2 / 10000;
	// 1000개 hull[0]~hull[1] 각도 평균 값, 1000개 hull[1]~hull[2] 각도 평균 값
	Ptrdis1 = avr_dis0_1, Ptrangle1 = avr_angle0_1, Ptrdis2 = avr_dis1_2, Ptrangle2 = avr_angle1_2; // 참조자를 사용한 다중 리턴
}
</code>
</pre>

* 거리와 각도에 따른 손 모양 판별 함수
<pre>
<code>
String hand_shape(double dis1, double angle1, double dis2, double angle2) // 거리와 각도에 따른 손 모양 판별 함수 정의
{
	if ((dis1 > 20 && dis1 < 70) && (dis2 > 60 && dis2 < 85) && (angle1 > 0.3 && angle1 < 0.6) && (angle1 > 0.5 && angle1 < 0.9))
	// hull[0]~hull[1] 사이 거리: 20~70, hull[0]~hull[1] 사이 각도: 0.3~0.6
	// hull[1]~hull[2] 사이 거리: 60~85, hull[1]~hull[2] 사이 긱도: 0.5~0.9
	{
		st_hand_shape = "delete_hand"; // 삭제하는 손 모양

	}

	else if ((dis1 >= 120 && dis1 <= 175) && (dis2 >= 50 && dis2 <= 140) && (angle1 > 0.9 && angle1 < 1.1) && (angle2 > 1.9))
	// hull[0]~hull[1] 사이 거리: 120~175, hull[0]~hull[1] 사이 각도: 0.9~1.1
	// hull[1]~hull[2] 사이 거리: 50~140, hull[1]~hull[2] 사이 긱도: 1.9이상
	{
		st_hand_shape = "draw_hand"; // 그리는 손 모양
	}
	else if ((dis1 > 85 && dis1 <= 100) && (angle1 > 0.2 && angle1 < 0.55) && (angle2 > 1.5))
	// hull[0]~hull[1] 사이 거리: 85~100, hull[0]~hull[1] 사이 각도: 0.2~0.55, hull[1]~hull[2] 사이 긱도: 1.5 이상
	{
		st_hand_shape = "save_hand"; // 저장하는 손 모양
	}
	else st_hand_shape = "none"; // 위의 경우가 아닌 경우

	return st_hand_shape; // string형으로 리턴
}
</code>
</pre>

<img width="479" alt="화면 캡처 2022-12-08 105854" src="https://user-images.githubusercontent.com/103232864/206338404-899d8dd7-5976-41f4-abbd-7dd679502689.png">
<img width="478" alt="화면 캡처 2022-12-08 105914" src="https://user-images.githubusercontent.com/103232864/206338413-85e8924e-d9b8-48b5-8bc4-066bc2f391fb.png">
<img width="479" alt="화면 캡처 2022-12-08 105958" src="https://user-images.githubusercontent.com/103232864/206338421-715d2255-7ed2-4895-aae3-d9b4d18bc8a7.png">
