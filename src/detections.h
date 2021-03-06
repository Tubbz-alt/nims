/*
 *  Nekton Interaction Monitoring System (NIMS)
 *
 *  detections.h
 *  
 *  Created by Shari Matzner on 08/20/2015.
 *  Copyright 2015 Pacific Northwest National Laboratory. All rights reserved.
 *
 */

#ifndef __NIMS_DETECTIONS_H__
#define __NIMS_DETECTIONS_H__

#include <cstdint>  // fixed width integer types
#include <cstring>  // memset
 #include <vector>
 #include <algorithm> // std::copy, min, max, sum
 #include <ios> // io formatting

//#include "log.h"      // NIMS logging

//#include "pixelgroup.h"

#define MAX_DETECTIONS_PER_FRAME 100

 enum dimension_t {RANGE=0, BEARING, ELEVATION};

// Data structure for describing the objects detected
// in one ping image.  Detections are possibly fish
// or marine mammals or some other object moving
// through the sonar field of view.
struct __attribute__ ((__packed__)) Detection
//struct  Detection
{
    double timestamp; // seconds since midnight 1-Jan-1970
    // spatial shape information
    float center[3]; // x,y,z
    float size[3];   // width, length, height
    // rotation in degrees, clockwise
    // rot_deg[0] is rotation in x-y plane
    // rot_deg[1] is elevation angle with respect to x-y plane
    float rot_deg[2];  
    // target strength
    float intensity_min;
    float intensity_max;
    float intensity_sum;
   
    Detection()
    {
        timestamp = 0.0;
        center[RANGE] = 0.0; center[BEARING] = 0.0; center[ELEVATION] = 0.0;
        size[RANGE] = 0.0; size[BEARING] = 0.0; size[ELEVATION] = 0.0;
        rot_deg[0] = 0.0; rot_deg[1] = 0.0;
        intensity_min =0.0;
        intensity_max = 0.0;
        intensity_sum = 0.0;
       
    };
        
};

 // TODO:  Resolve multiple definitions link error.
/*
std::ostream& operator<<(std::ostream& strm, const Detection& d)
{
    std::ios_base::fmtflags fflags = strm.setf(std::ios::fixed,std::ios::floatfield);
    int prec = strm.precision();
    strm.precision(3);

    strm << d.timestamp 
    << "," << d.center[BEARING] << "," << d.center[RANGE] << "," << d.center[ELEVATION]
    << "," << d.size[BEARING] << "," << d.size[RANGE] << "," << d.size[ELEVATION]
    << "," << d.rot_deg[0] << "," << d.rot_deg[1]
    << std::endl;

    // restore formatting
    strm.precision(prec);
    strm.setf(fflags);
    return strm;
};
*/

//struct __attribute__ ((__packed__)) DetectionMessage
struct  DetectionMessage
{
    uint32_t  frame_num; // frame number from FrameBuffer
    uint32_t  ping_num; // sonar ping number 
    double ping_time; // seconds since midnight 1-Jan-1970
    uint32_t  num_detections; // number of detections
    Detection detections[MAX_DETECTIONS_PER_FRAME];

    DetectionMessage()
    {
        frame_num = 0;
        ping_num = 0;
        ping_time = 0.0;

        num_detections = 0;
        memset(detections, 0, sizeof(Detection)*MAX_DETECTIONS_PER_FRAME);
    };
    
     DetectionMessage(uint32_t fnum, uint32_t pnum, double ptime, std::vector<Detection> vec_detections )
    {
        frame_num = fnum;
        ping_num = pnum;
        ping_time = ptime;
        num_detections = std::min((int)vec_detections.size(),MAX_DETECTIONS_PER_FRAME);
        std::copy(vec_detections.begin(), 
                vec_detections.begin()+num_detections, detections);
    };
   
}; // Detections

/*
std::ostream& operator<<(std::ostream& strm, const DetectionMessage& dm)
{
    strm << "    ping_num = " << dm.ping_num << "\n"
         << "    num_detections = " << dm.num_detections << "\n"
    << std::endl;
    
    
    return strm;
};
*/

#endif // __NIMS_DETECTIONS_H__
