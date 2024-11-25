#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

// Convert to string for FPS
#define SSTR( x ) static_cast < std::ostringstream >( \
( std::ostringstream() << std::dec << x ) ).str()

/* Launch the video processing */
int videoProcessing(Ptr<Tracker> needTracker, String needTrackerName);

int main(int argc, char const *argv[])
{
    // List of tracker types in OpenCV 4.10
    string trackerTypes[7] = {"MIL","KCF","CSRT","GOTURN","DaSiamRPN","Nano","Vit"};

    // Create a tracker
    string trackerType = trackerTypes[2];

    Ptr<Tracker> tracker;

    if(trackerType=="MIL")
    tracker = TrackerMIL::create();
    if(trackerType=="KCF")
    tracker = TrackerKCF::create();
    if(trackerType=="CSRT")
    tracker = TrackerCSRT::create();
    if(trackerType=="GOTURN")
    tracker = TrackerGOTURN::create();
    if(trackerType=="DaSiamRPN")
    tracker = TrackerDaSiamRPN::create();
    if(trackerType=="Nano")
    tracker = TrackerNano::create();
    if(trackerType=="Vit")
    tracker = TrackerVit::create();

    cout << "Tracker "+trackerType+" are selected." << "\n";

    // Launch video processing
    videoProcessing(tracker, trackerType);
   
    return 0;
}

int videoProcessing(Ptr<Tracker> needTracker, String needTrackerName)
{
    // Read video from file
    //VideoCapture video("pixel.mp4",CAP_FFMPEG);
    
    // Read video from camera
    VideoCapture video(0);

    // Set maximum camera resolution (set FullHD, camera resolution <= HD)
    video.set(CAP_PROP_FRAME_WIDTH,1920);
    video.set(CAP_PROP_FRAME_HEIGHT,1080);

    // Exit if video is not opened
    if(!video.isOpened())
    {
        cout << "Could not read video file" << "\n";
        return 1;
    }

    // Read first frame
    Mat frame;
    bool ok = video.read(frame);

    // Mirrow frame
    flip(frame,frame,1);

    // Uncomment the line below to select a different bounding box 
    Rect bbox = selectROI(frame, false); 

    // Display bounding box. 
    rectangle(frame,bbox,Scalar(255,0,0),2,1);

    String window_name = "Tracking with " + needTrackerName + " tracker | esc to quit";
    namedWindow(window_name, WINDOW_NORMAL); //resizable window;

    imshow(window_name,frame);
    needTracker->init(frame,bbox);

    while(video.read(frame))
    {
        // Mirror video
        flip(frame,frame,1);
        
        // Start timer
        double timer = (double)getTickCount();

        // Update the tracking result
        bool ok = needTracker->update(frame,bbox);

        // Calculate frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);
        
        if(ok)
        {
            // Tracking success : Draw the tracked object
            rectangle(frame,bbox,Scalar(255,0,0),2,1);
            putText(frame,"Enemy target",Point(bbox.x,bbox.y-5),FONT_HERSHEY_COMPLEX,1,Scalar(0,0,255),2);
        }
        else
        {
            // Tracking failure detected.
            putText(frame,"Tracking failure detected",Point(10,80),FONT_HERSHEY_PLAIN,1,Scalar(0,0,255),2);
        }

        // Display tracker type on frame.
        putText(frame, needTrackerName + " Tracker", Point(10,20), FONT_HERSHEY_PLAIN, 1, Scalar(50,170,50),2);

        // Display FPS on frame.
        putText(frame, "FPS : " + SSTR(int(fps)), Point(10,50), FONT_HERSHEY_PLAIN, 1, Scalar(0,128,255), 2);
        
        // Display frame.
        
        imshow(window_name, frame);

        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            break;
        }
    }
    return 0;
}