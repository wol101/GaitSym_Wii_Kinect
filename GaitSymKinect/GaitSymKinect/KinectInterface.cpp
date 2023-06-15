/*
 *  KinectInterface.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "KinectInterface.h"

#if defined(_WIN32) || defined(WIN32)
#include <Kinect.h>
#endif

#include <QMessageBox>
#include <iostream>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

KinectInterface::KinectInterface()
{
    InitializeDefaultSensor();

    m_characterList = new KinectCharacter[GetMaxCharacters()];
    m_nChactacters = 0;
    for (int iCharacter = 0; iCharacter < GetMaxCharacters(); iCharacter++)
    {
        for (int iJointID = 0; iJointID < KinectJointIDCount; iJointID++)
        {
            m_characterList[iCharacter].joints[iJointID].state = NotTracked;
        }
        m_characterList[iCharacter].tracked = false;
    }

    InitialiseBodies();

#if defined(_WIN32) || defined(WIN32)
    // create heap storage for depth pixel data in RGBX format
    m_pDepthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];
    m_cameraSpacePoints = new CameraSpacePoint[cDepthWidth * cDepthHeight];
#else
    m_pDepthRGBX = 0;
    m_cameraSpacePoints = 0;
#endif
}

KinectInterface::~KinectInterface()
{
#if defined(_WIN32) || defined(WIN32)
    // done with depth frame reader
    SafeRelease(m_pDepthFrameReader);

    // done with body frame reader
    SafeRelease(m_pBodyFrameReader);

    // done with coordinate mapper
    SafeRelease(m_pCoordinateMapper);

    // close the Kinect Sensor
    if (m_pKinectSensor)
    {
        m_pKinectSensor->Close();
    }

    SafeRelease(m_pKinectSensor);
    if (m_pDepthRGBX) delete [] m_pDepthRGBX;
    if (m_cameraSpacePoints) delete [] m_cameraSpacePoints;
#endif
    if (m_characterList) delete [] m_characterList;
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
long KinectInterface::InitializeDefaultSensor()
{
#if defined(_WIN32) || defined(WIN32)
    HRESULT hr;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        return hr;
    }

    if (m_pKinectSensor)
    {
        // Initialize the Kinect and get coordinate mapper and the body reader
        IBodyFrameSource* pBodyFrameSource = NULL;
        IDepthFrameSource* pDepthFrameSource = NULL;

        hr = m_pKinectSensor->Open();

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
        }

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
        }

        SafeRelease(pBodyFrameSource);

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
        }

        SafeRelease(pDepthFrameSource);
    }

    if (!m_pKinectSensor || FAILED(hr))
    {
        QMessageBox::warning(0, "Kinect Hardware Problem", "No ready Kinect found");
        return E_FAIL;
    }

    return hr;
#else
    return 0;
#endif
}

/// <summary>
/// Main processing function
/// </summary>
void KinectInterface::UpdateKinect()
{
#if defined(_WIN32) || defined(WIN32)
    for (int iCharacter = 0; iCharacter < BODY_COUNT; iCharacter++) m_characterList[iCharacter].tracked = false;

    // handle the skeleton
    if (!m_pBodyFrameReader)
    {
        return;
    }

    IBodyFrame* pBodyFrame = NULL;

    HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

    if (SUCCEEDED(hr))
    {
        INT64 nTime = 0;

        hr = pBodyFrame->get_RelativeTime(&nTime);

        IBody* ppBodies[BODY_COUNT] = {0};

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
        }

        if (SUCCEEDED(hr))
        {
            ProcessBody(nTime, BODY_COUNT, ppBodies);
        }

        for (int i = 0; i < _countof(ppBodies); ++i)
        {
            SafeRelease(ppBodies[i]);
        }
    }

    SafeRelease(pBodyFrame);

    // handle the depth map
    if (!m_pDepthFrameReader)
    {
        return;
    }

    IDepthFrame* pDepthFrame = NULL;

    hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);

    if (SUCCEEDED(hr))
    {
        INT64 nTime = 0;
        IFrameDescription* pFrameDescription = NULL;
        int nWidth = 0;
        int nHeight = 0;
        USHORT nDepthMinReliableDistance = 0;
        USHORT nDepthMaxDistance = 0;
        UINT nBufferSize = 0;
        UINT16 *pBuffer = NULL;

        hr = pDepthFrame->get_RelativeTime(&nTime);

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Width(&nWidth);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Height(&nHeight);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
        }

        if (SUCCEEDED(hr))
        {
            // In order to see the full range of depth (including the less reliable far field depth)
            // we are setting nDepthMaxDistance to the extreme potential depth threshold
            nDepthMaxDistance = USHRT_MAX;

            // Note:  If you wish to filter by reliable depth distance, uncomment the following line.
            //// hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxDistance);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
        }

        if (SUCCEEDED(hr))
        {
            ProcessDepth(nTime, pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxDistance);
        }

        SafeRelease(pFrameDescription);
    }

    SafeRelease(pDepthFrame);

#endif
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void KinectInterface::ProcessBody(int64_t nTime, int nBodyCount, IBody** ppBodies)
{
#if defined(_WIN32) || defined(WIN32)
    HRESULT hr;
    //std::cerr << "nBodyCount = " << nBodyCount << "\n";
    for (int i = 0; i < nBodyCount; ++i)
    {
        IBody* pBody = ppBodies[i];
        if (pBody)
        {
            BOOLEAN bTracked = false;
            hr = pBody->get_IsTracked(&bTracked);
            m_characterList[i].tracked = bTracked;

            if (SUCCEEDED(hr) && bTracked)
            {
                //std::cerr << "i = " << i << "\n";
                Joint joints[JointType_Count];
                HandState leftHandState = HandState_Unknown;
                HandState rightHandState = HandState_Unknown;

                pBody->get_HandLeftState(&leftHandState);
                pBody->get_HandRightState(&rightHandState);

                hr = pBody->GetJoints(_countof(joints), joints);
                if (SUCCEEDED(hr))
                {
                    KinectJoint *pJoint = m_characterList[i].joints;
                    for (int iJointID = 0; iJointID < KinectJointIDCount; iJointID++)
                        pJoint[iJointID].state = NotTracked;
                    DepthSpacePoint depthPoint;
                    for (int j = 0; j < _countof(joints); ++j)
                    {
                        pJoint[joints[j].JointType].state = static_cast<KinectTrackingState>(joints[j].TrackingState);
                        pJoint[joints[j].JointType].cameraSpaceLocation =
                                pgd::Vector(joints[j].Position.X, joints[j].Position.Y, joints[j].Position.Z);
                        m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthPoint);
                        pJoint[joints[j].JointType].depthSpaceLocation =
                                pgd::Vector(depthPoint.X, depthPoint.Y, 0);
                    }
                }

                JointOrientation jointOrientations[JointType_Count];
                hr = pBody->GetJointOrientations(_countof(jointOrientations), jointOrientations);
                if (SUCCEEDED(hr))
                {
                    KinectJoint *pJoint = m_characterList[i].joints;
#define USE_ABSOLUTE_JOINT_ORIENTATIONS // actually the currently produced joint orientations seem fairly useless for biomechanics
#ifdef USE_ABSOLUTE_JOINT_ORIENTATIONS
                    for (int j = 0; j < _countof(joints); ++j)
                    {
                        pJoint[joints[j].JointType].jointOrinetation =
                                pgd::Quaternion(jointOrientations[j].Orientation.w, jointOrientations[j].Orientation.x, jointOrientations[j].Orientation.y, jointOrientations[j].Orientation.z);
                    }
#else
                    KinectJointID parent = KinectJointIDCount;
                    pgd::Quaternion q, p;
                    for (int j = 0; j < _countof(joints); ++j)
                    {
                        switch (j)
                        {
                        case SpineBase: parent = SpineBase; break; // this is the reference segment
                        case SpineMid: parent = SpineBase; break;
                        case Neck: parent = SpineShoulder; break;
                        case Head: parent = Neck; break;
                        case ShoulderLeft: parent = SpineShoulder; break;
                        case ElbowLeft: parent = ShoulderLeft; break;
                        case WristLeft: parent = ElbowLeft; break;
                        case HandLeft: parent = WristLeft; break;
                        case ShoulderRight: parent = SpineShoulder; break;
                        case ElbowRight: parent = ShoulderRight; break;
                        case WristRight: parent = ElbowRight; break;
                        case HandRight: parent = WristRight; break;
                        case HipLeft: parent = SpineBase; break;
                        case KneeLeft: parent = HipLeft; break;
                        case AnkleLeft: parent = KneeLeft; break;
                        case FootLeft: parent = AnkleLeft; break;
                        case HipRight: parent = SpineBase; break;
                        case KneeRight: parent = HipRight; break;
                        case AnkleRight: parent = KneeRight; break;
                        case FootRight: parent = AnkleRight; break;
                        case SpineShoulder: parent = SpineMid; break;
                        case HandTipLeft: parent = HandLeft; break;
                        case ThumbLeft: parent = HandLeft; break;
                        case HandTipRight: parent = HandRight; break;
                        case ThumbRight: parent = HandRight; break;
                        }
                        q.Set(jointOrientations[j].Orientation.w, jointOrientations[j].Orientation.x, jointOrientations[j].Orientation.y, jointOrientations[j].Orientation.z);
                        p.Set(jointOrientations[parent].Orientation.w, jointOrientations[parent].Orientation.x, jointOrientations[parent].Orientation.y, jointOrientations[parent].Orientation.z);
                        pJoint[joints[j].JointType].jointOrinetation = q * (~p);
                    }
#endif

                    // debugging code to work out what these orientations are
//#define DEBUG_ARM
#ifdef DEBUG_ARM
                    // this value is actually the absolute angle of the arm segment
                    pgd::Quaternion o(jointOrientations[ElbowLeft].Orientation.w, jointOrientations[ElbowLeft].Orientation.x, jointOrientations[ElbowLeft].Orientation.y, jointOrientations[ElbowLeft].Orientation.z);
                    pgd::Vector a = pgd::QGetAxis(o);
                    double angle =  pgd::QGetAngle(o) * 180 / M_PI;
                    std::cerr << "w,x,y,z = " << o.n << " " << o.v.x << " " << o.v.y << " " << o.v.z << "\n";
                    std::cerr << "angle = " << angle << " x,y,z = " << a.x << " " << a.y << " " << a.z << "\n";
                    // so this should calculate the relative shoulder angle
                    pgd::Quaternion s(jointOrientations[ShoulderLeft].Orientation.w, jointOrientations[ShoulderLeft].Orientation.x, jointOrientations[ShoulderLeft].Orientation.y, jointOrientations[ShoulderLeft].Orientation.z);
                    pgd::Quaternion q = o * (~s);
                    pgd::Vector ar = pgd::QGetAxis(q);
                    double angler =  pgd::QGetAngle(q) * 180 / M_PI;
                    std::cerr << "angler = " << angler << " x,y,z = " << ar.x << " " << ar.y << " " << ar.z << "\n";
#endif
//#define DEBUG_FOREARM
#ifdef DEBUG_FOREARM
                    // this value is actually the absolute angle of the forearm segment
                    pgd::Quaternion o(jointOrientations[WristLeft].Orientation.w, jointOrientations[WristLeft].Orientation.x, jointOrientations[WristLeft].Orientation.y, jointOrientations[WristLeft].Orientation.z);
                    pgd::Vector a = pgd::QGetAxis(o);
                    double angle =  pgd::QGetAngle(o) * 180 / M_PI;
                    //std::cerr << "o w,x,y,z = " << o.n << " " << o.v.x << " " << o.v.y << " " << o.v.z << "\n";
                    std::cerr << "angle = " << angle << " x,y,z = " << a.x << " " << a.y << " " << a.z << "\n";
                    // so this should calculate the relative elbow angle
                    pgd::Quaternion s(jointOrientations[ElbowLeft].Orientation.w, jointOrientations[ElbowLeft].Orientation.x, jointOrientations[ElbowLeft].Orientation.y, jointOrientations[ElbowLeft].Orientation.z);
                    pgd::Quaternion q = o * (~s);
                    pgd::Vector ar = pgd::QGetAxis(q);
                    double angler =  pgd::QGetAngle(q) * 180 / M_PI;
                    //std::cerr << "s w,x,y,z = " << s.n << " " << s.v.x << " " << s.v.y << " " << s.v.z << "\n";
                    std::cerr << "angler = " << angler << " x,y,z = " << ar.x << " " << ar.y << " " << ar.z << "\n";
#endif


                }


            }
        }
    }
    m_nChactacters = nBodyCount;

    if (!m_nStartTime)
    {
        m_nStartTime = nTime;
    }

#endif
}

/// <summary>
/// Set up the initial body mappings
/// </summary>
void KinectInterface::InitialiseBodies()
{
    KinectBody *pBody;
    for (int iCharacter = 0; iCharacter < GetMaxCharacters(); iCharacter++)
    {
        pBody = m_characterList[iCharacter].bodies;

        // Torso
        pBody[Skull].jointID1 = Head;
        pBody[Skull].jointID2 = Neck;
        pBody[Cervical].jointID1 = Neck;
        pBody[Cervical].jointID2 = SpineShoulder;
        pBody[Thoracic].jointID1 = SpineShoulder;
        pBody[Thoracic].jointID2 = SpineMid;
        pBody[Lumbar].jointID1 = SpineMid;
        pBody[Lumbar].jointID2 = SpineBase;
        pBody[RightScapula].jointID1 = SpineShoulder;
        pBody[RightScapula].jointID2 = ShoulderRight;
        pBody[LeftScapula].jointID1 = SpineShoulder;
        pBody[LeftScapula].jointID2 = ShoulderLeft;
        pBody[RightPelvis].jointID1 = SpineBase;
        pBody[RightPelvis].jointID2 = HipRight;
        pBody[LeftPelvis].jointID1 = SpineBase;
        pBody[LeftPelvis].jointID2 = HipLeft;

        // Right Arm
        pBody[RightArm].jointID1 = ShoulderRight;
        pBody[RightArm].jointID2 = ElbowRight;
        pBody[RightForearm].jointID1 = ElbowRight;
        pBody[RightForearm].jointID2 = WristRight;
        pBody[RightPalm].jointID1 = WristRight;
        pBody[RightPalm].jointID2 = HandRight;
        pBody[RightIndex].jointID1 = HandRight;
        pBody[RightIndex].jointID2 = HandTipRight;
        pBody[RightThumb].jointID1 = HandRight;
        pBody[RightThumb].jointID2 = ThumbRight;

        // Left Arm
        pBody[LeftArm].jointID1 = ShoulderLeft;
        pBody[LeftArm].jointID2 = ElbowLeft;
        pBody[LeftForearm].jointID1 = ElbowLeft;
        pBody[LeftForearm].jointID2 = WristLeft;
        pBody[LeftPalm].jointID1 = WristLeft;
        pBody[LeftPalm].jointID2 = HandLeft;
        pBody[LeftIndex].jointID1 = HandLeft;
        pBody[LeftIndex].jointID2 = HandTipLeft;
        pBody[LeftThumb].jointID1 = HandLeft;
        pBody[LeftThumb].jointID2 = ThumbLeft;

        // Right Leg
        pBody[RightThigh].jointID1 = HipRight;
        pBody[RightThigh].jointID2 = KneeRight;
        pBody[RightShank].jointID1 = KneeRight;
        pBody[RightShank].jointID2 = AnkleRight;
        pBody[RightFoot].jointID1 = AnkleRight;
        pBody[RightFoot].jointID2 = FootRight;

        // Left Leg
        pBody[LeftThigh].jointID1 = HipLeft;
        pBody[LeftThigh].jointID2 = KneeLeft;
        pBody[LeftShank].jointID1 = KneeLeft;
        pBody[LeftShank].jointID2 = AnkleLeft;
        pBody[LeftFoot].jointID1 = AnkleLeft;
        pBody[LeftFoot].jointID2 = FootLeft;
    }
}

/// <summary>
/// Returns the maximum number of characters the sensor can detect
/// </summary>
int KinectInterface::GetMaxCharacters()
{
#if defined(_WIN32) || defined(WIN32)
    return BODY_COUNT;
#else
    return 6;
#endif
}

/// <summary>
/// Handle new depth data
/// <param name="nTime">timestamp of frame</param>
/// <param name="pBuffer">pointer to frame data</param>
/// <param name="nWidth">width (in pixels) of input image data</param>
/// <param name="nHeight">height (in pixels) of input image data</param>
/// <param name="nMinDepth">minimum reliable depth</param>
/// <param name="nMaxDepth">maximum reliable depth</param>
/// </summary>
void KinectInterface::ProcessDepth(int64_t nTime, const uint16_t *buffer, int nWidth, int nHeight, uint16_t nMinDepth, uint16_t nMaxDepth)
{
#if defined(_WIN32) || defined(WIN32)
//    if (m_hWnd)
//    {
//        if (!m_nStartTime)
//        {
//            m_nStartTime = nTime;
//        }

//        double fps = 0.0;

//        LARGE_INTEGER qpcNow = {0};
//        if (m_fFreq)
//        {
//            if (QueryPerformanceCounter(&qpcNow))
//            {
//                if (m_nLastCounter)
//                {
//                    m_nFramesSinceUpdate++;
//                    fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
//                }
//            }
//        }

//        WCHAR szStatusMessage[64];
//        StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));

//        if (SetStatusMessage(szStatusMessage, 1000, false))
//        {
//            m_nLastCounter = qpcNow.QuadPart;
//            m_nFramesSinceUpdate = 0;
//        }
//    }

    // Make sure we've received valid data
    if (m_pDepthRGBX && buffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
    {
        RGBQUAD* pRGBX = m_pDepthRGBX;

        const UINT16* pBuffer = buffer;
        // end pixel is start + width*height - 1
        const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;

            // To convert to a byte, we're discarding the most-significant
            // rather than least-significant bits.
            // We're preserving detail, although the intensity will "wrap."
            // Values outside the reliable depth range are mapped to 0 (black).

            // Note: Using conditionals in this loop could degrade performance.
            // Consider using a lookup table instead when writing production code.
            BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

            pRGBX->rgbRed   = intensity;
            pRGBX->rgbGreen = intensity;
            pRGBX->rgbBlue  = intensity;
            pRGBX->rgbReserved  = 255;

            ++pRGBX;
            ++pBuffer;
        }

        // convert the depth map to a 3D point cloud
        UINT depthPointCount = cDepthWidth * cDepthHeight;
        UINT cameraPointCount = cDepthWidth * cDepthHeight;
        HRESULT hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(depthPointCount, buffer, cameraPointCount, m_cameraSpacePoints);

        // Draw the data with Direct2D
        //m_pDrawDepth->Draw(reinterpret_cast<BYTE*>(m_pDepthRGBX), cDepthWidth * cDepthHeight * sizeof(RGBQUAD));

//        if (m_bSaveScreenshot)
//        {
//            WCHAR szScreenshotPath[MAX_PATH];

//            // Retrieve the path to My Photos
//            GetScreenshotFileName(szScreenshotPath, _countof(szScreenshotPath));

//            // Write out the bitmap to disk
//            HRESULT hr = SaveBitmapToFile(reinterpret_cast<BYTE*>(m_pDepthRGBX), nWidth, nHeight, sizeof(RGBQUAD) * 8, szScreenshotPath);

//            WCHAR szStatusMessage[64 + MAX_PATH];
//            if (SUCCEEDED(hr))
//            {
//                // Set the status bar to show where the screenshot was saved
//                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Screenshot saved to %s", szScreenshotPath);
//            }
//            else
//            {
//                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Failed to write screenshot to %s", szScreenshotPath);
//            }

//            SetStatusMessage(szStatusMessage, 5000, true);

//            // toggle off so we don't save a screenshot again next frame
//            m_bSaveScreenshot = false;
//        }
    }
#endif
}

