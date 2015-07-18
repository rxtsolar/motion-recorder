#ifndef _MOTION_RECORDER_H_
#define _MOTION_RECORDER_H_

#include <opencv2/opencv.hpp>
#include <ctime>

namespace gs {

using namespace std;
using namespace cv;

class Recorder {
public:
	Recorder(void) : videoCapture(0)
	{
		assert(videoCapture.isOpened());

	}

	void startRecording(void)
	{
		time_t t;
		tm* timeinfo;
		char buffer[80];
		string fileName;

		time(&t);
		timeinfo = localtime(&t);

		strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
		fileName = string(buffer);

		fileName += ".avi";
		videoWriter.open(fileName, CV_FOURCC('M', 'J', 'P', 'G'), 25, 
				cvSize(width, height));
		if (!videoWriter.isOpened()) {
		}
		recording = true;
	}

	void stopRecording(void)
	{
		recording = false;
		videoWriter.release();
	}

	Mat getFrame(void)
	{
		Mat frame;
		videoCapture >> frame;
		return frame;
	}

	void writeFrame(const Mat& frame)
	{
		videoWriter << frame;
	}

	bool isRecording(void)
	{
		return recording;
	}

	int getWidth(void)
	{
		return width;
	}

	int getHeight(void)
	{
		return height;
	}

	void setWidth(int w)
	{
		width = w;
	}

	void setHeight(int h)
	{
		height = h;
	}

private:
	VideoCapture videoCapture;
	VideoWriter videoWriter;
	int width;
	int height;
	bool recording;
};

class MotionRecorder : public Recorder {
public:
	MotionRecorder (void) : margin(100), thresh(10)
	{
		watch();
	}

private:
	Mat current;
	Mat previous;
	int thresh;
	int area;
	int margin;
	int count;

	void watch(void)
	{
		bool running = true;

		previous = getFrame();
		setWidth(previous.cols);
		setHeight(previous.rows);
		area = previous.cols * previous.rows / 10;

		while (running) {
			current = getFrame();

			if (isMotion()) {
				resetCount();
				if (!isRecording()) {
					startRecording();
				}
			}
			
			if (isRecording()) {
				writeFrame(current);

				if (timeUp()) {
					stopRecording();
				} else {
					countDown();
				}
			}

			previous = current;

			if (waitKey(40) == 27)
				running = false;
		}
	}

	bool isMotion(void)
	{
		Mat diff;
		int count;
		Mat pGray;
		Mat cGray;

		cvtColor(previous, pGray, CV_BGR2GRAY);
		cvtColor(current, cGray, CV_BGR2GRAY);
		absdiff(pGray, cGray, diff);
		threshold(diff, diff, thresh, 255, THRESH_BINARY);
		count = countNonZero(diff);

		imshow("current", current);
		imshow("diff", diff);

		if (count > area)
			return true;
		else
			return false;
	}

	void resetCount(void)
	{
		count = margin;
	}

	bool timeUp(void)
	{
		return count == 0;
	}

	void countDown(void)
	{
		count--;
	}
};

}
#endif
