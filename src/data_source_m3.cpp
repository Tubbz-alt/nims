/*
 *  Nekton Interaction Monitoring System (NIMS)
 *
 *  data_source_m3.cpp
 *  
 *  Created by Shari Matzner on 06/23/2015.
 *  Copyright 2015 Pacific Northwest National Laboratory. All rights reserved.
 *
 */

#include "data_source_m3.h"

#include <iostream> // cout, cerr, clog
#include <cstring>
#include <cstdlib>  // malloc, free
#include <stdint.h> // fixed width integer types
#include <math.h>   // sqrt, pow

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr
#include <unistd.h> // close

#include "log.h"

using namespace std;

//-----------------------------------------------------------------------------
// from M3 IMB Beamformed Data Format, document 922-20007002
//-----------------------------------------------------------------------------
    
#define ERROR_MSG_EXIT(err_msg)     \
{                                   \
    if (bfData != nullptr) free(bfData); \
    NIMS_LOG_ERROR << err_msg;          \
    return -1;                         \
}                                   \

#define INT32U  uint32_t
#define INT32S  int32_t
#define INT16U  uint16_t
#define INT16S  int16_t
#define INT8U   uint8_t

#define HDR_SYNC_INT16U_1     (INT16U)0x8000
#define HDR_SYNC_INT16U_2     (INT16U)0x8000
#define HDR_SYNC_INT16U_3     (INT16U)0x8000
#define HDR_SYNC_INT16U_4     (INT16U)0x8000

#define PKT_DATA_TYPE_BEAMFORMED (INT16U)0x1002

#define MAX_NUM_BEAMS           (INT32U)1024


typedef struct
{
    float   I;
    float   Q;
} Ipp32fc_Type;

#define ABS_IQ(x) sqrt(pow(x.I,2)+pow(x.Q,2));

// Time Varying Gain (TVG)
// see section 9.9.3 Changing the TVG, p. 94 in User's Manual
// Chu, D. and Hufnagle, Jr., L., "Time varying gain (TVG) measurements
//    of a multibeam echo sounder for applications to quantitative acoustics.", 2006
// www.dtic.mil/cgi-bin/GetTRDoc?AD=ADA498689
// http://www.hydro-international.com/issues/articles/id890-Digital_Sidescan_is_this_the_end_of_TVG_and_AGC.html
//
// TL = max(A log10 r + B r + C, L)
// where TL is transmission loss, r is range
typedef struct
{
    INT16U  A; // the spreading coefficient
    INT16U  B; // the absorption coefficient in dB/km
    float   C; // the TVG curve offset in dB
    float   L; // the maximum gain limit in dB
} TVG_Params_Type;

typedef struct
{
	float fOffsetA;	// rotator offset A in meters
	float fOffsetB;	// rotator offset B in meters
	float fOffsetR;	// rotator offset R in degrees
	float fAngle;	// rotator angle in degrees

}M3_ROTATOR_OFFSETS;

typedef struct
{
    INT16U sync_word_1;
    INT16U sync_word_2;
    INT16U sync_word_3;
    INT16U sync_word_4;
    INT16U data_type;       // always 0x1002
    INT16U reserved_field;  // NOTE:  this is in spec but not in load_image_data.c
    INT32U reserved[10];
    INT32U packet_body_size;
} Packet_Header_Struct;

typedef struct
{
    INT32U packet_body_size; // this should match value in header
    INT32U reserved[10];
} Packet_Footer_Struct;

/****************************
 * Data header               *
 ****************************/
typedef struct
{
    INT32U  dwVersion;
    INT32U  dwSonarID;
    INT32U  dwSonarInfo[8];
    INT32U  dwTimeSec;
    INT32U  dwTimeMillisec;
    float   fVelocitySound;
    INT32U  nNumSamples;
    float   fNearRange;
    float   fFarRange;
    float   fSWST;
    float   fSWL;
    INT16U  nNumBeams;
    INT16U  wReserved1;
    float   fBeamList[MAX_NUM_BEAMS];
    float   fImageSampleInterval;
    INT16U  wImageDestination;
    INT16U  wReserved2;
    INT32U  dwModeID;
    INT32S  nNumHybridPRI;
    INT32S  nHybridIndex;
    INT16U  nPhaseSeqLength;
    INT16U  iPhaseSeqIndex;
    INT16U  nNumImages;
    INT16U  iSubImageIndex;
    
    INT32U  dwSonarFreq;
    INT32U  dwPulseLength;
    INT32U  dwPingNumber;
    
    float   fRXFilterBW;
    float   fRXNominalResolution;
    float   fPulseRepFreq;
    char    strAppName[128];
    char    strTXPulseName[64];
    TVG_Params_Type sTVGParameters;
    float   fCompassHeading;
    float   fMagneticVariation;
    float   fPitch;
    float   fRoll;
    float   fDepth;
    float   fTemperature;
    
    float   fXOffset;  // M3_OFFSET
    float   fYOffset;
    float   fZOffset;
    float   fXRotOffset;
    float   fYRotOffset;
    float   fZRotOffset;
	INT32U dwMounting;
    
    double  dbLatitude;
    double  dbLongitude;
    float   fTXWST;
    
	unsigned char bHeadSensorsVersion;
	unsigned char bHeadHWStatus;
    INT8U   byReserved1;
    INT8U   byReserved2;
    
    float fInternalSensorHeading;
	float fInternalSensorPitch;
	float fInternalSensorRoll;
	M3_ROTATOR_OFFSETS aAxesRotatorOffsets[3];

    INT16U nStartElement;
    INT16U nEndElement;
    char   strCustomText1[32];
    char   strCustomText2[32];
    float  fLocalTimeOffset;
    
    unsigned char  reserved[3876];
    
    
 } Data_Header_Struct;

// forward declaration; implementation is at the end of this file, out of the way.
std::ostream& operator<<(std::ostream& strm, const Data_Header_Struct& hdr);

//-----------------------------------------------------------------------------
// DataSourceM3::DataSourceM3
// open a connection to the M3 host program
// expecting a host address in xxx.xxx.xxx.xxx form
DataSourceM3::DataSourceM3(std::string const &host_addr)
{
    
    m3_host_.sin_family = AF_INET;
    m3_host_.sin_addr.s_addr = inet_addr(host_addr.c_str());
    // TODO: maybe make the port an arg
    // Port 20001 is default for M3 host
    m3_host_.sin_port = htons(20001);
    
} // DataSourceM3::DataSourceM3

DataSourceM3::~DataSourceM3() { close(input_); }

//-----------------------------------------------------------------------------
// DataSourceM3::connect
// connect to the M3 host
int DataSourceM3::connect()
{
     input_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // NOTE:  Have to indicate external connect with "::"
    //        to distguish from member function connect()
    // NOTE:  ::connect() will block for some time interval, waiting for connection
    //        and eventually time out.  POSIX does not specify interval.
    if ( ::connect(input_, (struct sockaddr *) &m3_host_, sizeof(struct sockaddr)) < 0 )
    {
        nims_perror("DataSourceM3::connect() failed");
        close (input_);
        input_ = -1;
        return -1;
    }
    return 0;

} // DataSourceM3::connect


//-----------------------------------------------------------------------------
// DataSourceM3::GetPing
// get the next ping from the source
int DataSourceM3::GetPing(Frame* pframe)
{
    if ( input_ == -1 ) {
        NIMS_LOG_ERROR << ("DataSourceM3::GetPing() Not connected to source.");
        return -1;
    }
    
    // Initialize variables
    Packet_Header_Struct packet_header;
    Data_Header_Struct   header;
    Packet_Footer_Struct packet_footer;

    //memset(&packet_header, 0, sizeof(Packet_Header_Struct));
    //memset(&header,        0, sizeof(Data_Header_Struct));
    //memset(&packet_footer, 0, sizeof(Packet_Footer_Struct));

    INT32U num_bytes_bf_data = 0;
    Ipp32fc_Type* bfData = nullptr;
    
    ssize_t bytes_read = 0;
    
    /*
    NIMS_LOG_DEBUG << __func__;
    NIMS_LOG_DEBUG << "packet header size is " << sizeof(Packet_Header_Struct);
    NIMS_LOG_DEBUG << "data header size is " << sizeof(Data_Header_Struct);
    NIMS_LOG_DEBUG << "packet footer size is " << sizeof(Packet_Footer_Struct);
    */
    // Read the packet header.
     bytes_read = recv(input_, (char *)&packet_header, sizeof(packet_header),MSG_WAITALL);
     if ( bytes_read != sizeof(packet_header) ) {
         nims_perror("incorrect number of bytes from recv()");
         NIMS_LOG_ERROR << "bytes_read =  " << bytes_read;
         ERROR_MSG_EXIT("Error reading header.");
     }
     
     NIMS_LOG_DEBUG << "bytes_read =  " << bytes_read;

    // Check header sync words.
    if ( (packet_header.sync_word_1 != HDR_SYNC_INT16U_1) ||
         (packet_header.sync_word_2 != HDR_SYNC_INT16U_2) ||
         (packet_header.sync_word_3 != HDR_SYNC_INT16U_3) ||
         (packet_header.sync_word_4 != HDR_SYNC_INT16U_4) )
        ERROR_MSG_EXIT("Error reading sync words.");
        
    // Check packet type.
    if ( packet_header.data_type != PKT_DATA_TYPE_BEAMFORMED )
        ERROR_MSG_EXIT("Error wrong packet type.");

   // NIMS_LOG_DEBUG << "packet header body size is " << ntohl(packet_header.packet_body_size);
    
    // Read data header.
    bytes_read = recv(input_, (char *)&header, sizeof(header),MSG_WAITALL);
    if ( bytes_read != sizeof(header) ) {
        nims_perror("incorrect number of bytes from recv()");
        NIMS_LOG_ERROR << "bytes_read =  " << bytes_read;
        ERROR_MSG_EXIT("Error reading data header.");
    }
/*
    NIMS_LOG_DEBUG << "bytes_read =  " << bytes_read;
    NIMS_LOG_DEBUG << "num samples is " << header.nNumSamples;
    NIMS_LOG_DEBUG << "num beams is " << header.nNumBeams;
    NIMS_LOG_DEBUG << "header:" << endl << header;
*/
    num_bytes_bf_data = sizeof(Ipp32fc_Type)*(header.nNumSamples)*(header.nNumBeams);
    //NIMS_LOG_DEBUG << "num_bytes_bf_data = " << num_bytes_bf_data;

    bfData = (Ipp32fc_Type*)malloc(num_bytes_bf_data);
    if ( bfData == nullptr )
        ERROR_MSG_EXIT("Error allocating memory for data.");
        
    //NIMS_LOG_DEBUG << "reading data...";
    bytes_read = recv(input_, (char *)bfData, num_bytes_bf_data, MSG_WAITALL);
    if ( bytes_read != num_bytes_bf_data ) {
        nims_perror("incorrect number of bytes from recv()");
        NIMS_LOG_ERROR << "bytes_read =  " << bytes_read;
        ERROR_MSG_EXIT("Error reading data.");
    }
    //NIMS_LOG_DEBUG << "bytes_read =  " << bytes_read;

    // Read packet footer.
    bytes_read = recv(input_, (char *)&packet_footer, sizeof(packet_footer), MSG_WAITALL);
    if ( bytes_read != sizeof(packet_footer) ) {
        nims_perror("incorrect number of bytes from recv()");
        NIMS_LOG_ERROR << "bytes_read =  " << bytes_read;
        ERROR_MSG_EXIT("Error reading footer.");
    }
    
    //NIMS_LOG_DEBUG << "bytes_read =  " << bytes_read;
    //NIMS_LOG_DEBUG << "packet footer body size is " << packet_footer.packet_body_size;
    
    // Check packet size.
    if ( packet_header.packet_body_size != packet_footer.packet_body_size )
        ERROR_MSG_EXIT("Error header and footer body size do not match.");
        
    // Okay, good to go.
    
    //NIMS_LOG_DEBUG << "    extracting header";
    
    strncpy(pframe->header.device, "Kongsberg M3 Multibeam sonar", 
            sizeof(pframe->header.device));
    pframe->header.version = header.dwVersion;
    pframe->header.ping_num = header.dwPingNumber;
    pframe->header.ping_sec = header.dwTimeSec;
    pframe->header.ping_millisec = header.dwTimeMillisec;
    pframe->header.soundspeed_mps = header.fVelocitySound;
    pframe->header.num_samples = header.nNumSamples;
    pframe->header.range_min_m = header.fNearRange;
    pframe->header.range_max_m = header.fFarRange;
    pframe->header.winstart_sec = header.fSWST;
    pframe->header.winlen_sec = header.fSWL;
    pframe->header.num_beams = header.nNumBeams;
    for (int m=0; m<header.nNumBeams; ++m)
    {
        if (kMaxBeams == m) break; // max in frame_buffer.h
        pframe->header.beam_angles_deg[m] = header.fBeamList[m];
    }
    pframe->header.freq_hz = header.dwSonarFreq;
    pframe->header.pulselen_microsec = header.dwPulseLength;
    pframe->header.pulserep_hz = header.fPulseRepFreq;
   
    //NIMS_LOG_DEBUG << "    extracting data";
    // copy data to frame as real intensity value
    size_t frame_data_size = sizeof(framedata_t)*(header.nNumSamples)*(header.nNumBeams);
    pframe->malloc_data(frame_data_size);
    if ( pframe->size() != frame_data_size )
        ERROR_MSG_EXIT("Error allocating memory for frame data.");
    /*
    NOTE: The data is stored as

    00 01 ... 0(M-1)
    10 11 ... 1(M-1)
    .
    .
    .
    (N-1)0 (N-1)1 ... (N-1)(M-1)

    where N is the number of beams and M is the number of samples.  The first 
    column is near range and the first row is the minimum beam (leftmost) 
    beam.  I'm assuming row major order, although this is not specified in
    the M3 data format documentation.

    The Frame data is stored as
     00 01 ... 0(N-1)
    10 11 ... 1(N1)
    .
    .
    .
    (M-1)0 (M-1)1 ... (M-1)(N-1)
    */
    framedata_t* fdp = pframe->data_ptr();
    for (int n = 0; n < header.nNumBeams; ++n) // beam
    {
        for (int m = 0; m < header.nNumSamples; ++m) // sample
        {
            fdp[m*(header.nNumBeams) + n] = ABS_IQ( bfData[n*(header.nNumSamples) + m] );
            
        }
    }
    
	free(bfData);
    return 0;
} // DataSourceM3::GetPing

/*
//-----------------------------------------------------------------------------
// DataSourceM3::ReadPings
// read the specified number of pings
size_t DataSourceM3::ReadPings() 
{
    return 0;
    
} // DataSourceM3::ReadPings
*/

std::ostream& operator<<(std::ostream& strm, const Data_Header_Struct& hdr)
{
strm << "dwVersion = " << hdr.dwVersion << endl;
strm << "dwSonarID = " << hdr.dwSonarID << endl;
strm << "dwSonarInfo[0] = " << hdr.dwSonarInfo[0] << endl;
strm << "dwSonarInfo[1] = " << hdr.dwSonarInfo[1] << endl;
strm << "dwSonarInfo[2] = " << hdr.dwSonarInfo[2] << endl;
strm << "dwSonarInfo[3] = " << hdr.dwSonarInfo[3] << endl;
strm << "dwSonarInfo[4] = " << hdr.dwSonarInfo[4] << endl;
strm << "dwSonarInfo[5] = " << hdr.dwSonarInfo[5] << endl;
strm << "dwSonarInfo[6] = " << hdr.dwSonarInfo[6] << endl;
strm << "dwSonarInfo[7] = " << hdr.dwSonarInfo[7] << endl;
strm << "dwTimeSec = " << hdr.dwTimeSec << endl;
strm << "dwTimeMillisec = " << hdr.dwTimeMillisec << endl;
// convert to date time string
struct tm *ptm;
ptm = gmtime((time_t*)&(hdr.dwTimeSec));
strm << asctime(ptm) << endl;


strm << "fVelocitySound = " << hdr.fVelocitySound << endl;
strm << "nNumSamples = " << hdr.nNumSamples << endl;
strm << "fNearRange = " << hdr.fNearRange << endl;
strm << "fFarRange = " << hdr.fFarRange << endl;
strm << "fSWST = " << hdr.fSWST << endl;
strm << "fSWL = " << hdr.fSWL << endl;
strm << "nNumBeams = " << hdr.nNumBeams << endl;
strm << "fBeamList[0] = " << hdr.fBeamList[0] << endl;
strm << "fBeamList[1] = " << hdr.fBeamList[1] << endl;
strm << "fBeamList[2] = " << hdr.fBeamList[2] << endl;
strm << "fBeamList[10] = " << hdr.fBeamList[10] << endl;
strm << "fBeamList[11] = " << hdr.fBeamList[11] << endl;
strm << "fBeamList[12] = " << hdr.fBeamList[12] << endl;
strm << "fBeamList[nNumBeams-2] = " << hdr.fBeamList[hdr.nNumBeams-2] << endl;
strm << "fBeamList[nNumBeams-1] = " << hdr.fBeamList[hdr.nNumBeams-1] << endl;
strm << "fImageSampleInterval = " << hdr.fImageSampleInterval << endl;
strm << "wImageDestination = " << hdr.wImageDestination << endl;
strm << "dwModeID = " << hdr.dwModeID << endl;
strm << "nNumHybridPRI = " << hdr.nNumHybridPRI << endl;
strm << "nHybridIndex = " << hdr.nHybridIndex << endl;
strm << "nPhaseSeqLength = " << hdr.nPhaseSeqLength << endl;
strm << "iPhaseSeqIndex = " << hdr.iPhaseSeqIndex << endl;
strm << "nNumImages = " << hdr.nNumImages << endl;
strm << "iSubImageIndex = " << hdr.iSubImageIndex << endl;

strm << "dwSonarFreq = " << hdr.dwSonarFreq << endl;
strm << "dwPulseLength = " << hdr.dwPulseLength << endl;
strm << "dwPingNumber = " << hdr.dwPingNumber << endl;

strm << "fRXFilterBW = " << hdr.fRXFilterBW << endl;
strm << "fRXNominalResolution = " << hdr.fRXNominalResolution << endl;
strm << "fPulseRepFreq = " << hdr.fPulseRepFreq << endl;
strm << "strAppName = " << hdr.strAppName << endl;
strm << "strTXPulseName = " << hdr.strTXPulseName << endl;
strm << "sTVGParameters.A = " << hdr.sTVGParameters.A << endl;
strm << "sTVGParameters.B = " << hdr.sTVGParameters.B << endl;
strm << "sTVGParameters.C = " << hdr.sTVGParameters.C << endl;
strm << "sTVGParameters.L = " << hdr.sTVGParameters.L << endl;
strm << "fCompassHeading = " << hdr.fCompassHeading << endl;
strm << "fMagneticVariation = " << hdr.fMagneticVariation << endl;
strm << "fPitch = " << hdr.fPitch << endl;
strm << "fRoll = " << hdr.fRoll << endl;
strm << "fDepth = " << hdr.fDepth << endl;
strm << "fTemperature = " << hdr.fTemperature << endl;

strm << "sOffsets.fXOffset = " << hdr.fXOffset << endl;
strm << "sOffsets.fYOffset = " << hdr.fYOffset << endl;
strm << "sOffsets.fZOffset = " << hdr.fZOffset << endl;
strm << "sOffsets.fXRotOffset = " << hdr.fXRotOffset << endl;
strm << "sOffsets.fYRotOffset = " << hdr.fYRotOffset << endl;
strm << "sOffsets.fZRotOffset = " << hdr.fZRotOffset << endl;
strm << "sOffsets.dwMounting = " << hdr.dwMounting << endl;

strm << "dbLatitude = " << hdr.dbLatitude << endl;
strm << "dbLongitude = " << hdr.dbLongitude << endl;
strm << "fTXWST = " << hdr.fTXWST << endl;

strm << "bHeadSensorsVersion = " << hdr.bHeadSensorsVersion << endl;
strm << "bHeadHWStatus = " << hdr.bHeadHWStatus << endl;

strm << "fInternalSensorHeading = " << hdr.fInternalSensorHeading << endl;
strm << "fInternalSensorPitch = " << hdr.fInternalSensorPitch << endl;
strm << "fInternalSensorRoll = " << hdr.fInternalSensorRoll << endl;
//M3_ROTATOR_OFFSETS strm << "aAxesRotatorOffsets[3] << endl;
strm << "nStartElement = " << hdr.nStartElement << endl;
strm << "nEndElement = " << hdr.nEndElement << endl;
strm << "strCustomText1 = " << hdr.strCustomText1 << endl;
strm << "strCustomText2 = " << hdr.strCustomText2 << endl;
strm << "fLocalTimeOffset = " << hdr.fLocalTimeOffset << endl;

//unsigned char  reserved[3876]; // NOTE: on p10 of spec, 
//unsigned char  reserved[3946]; // in load_image_data.c, 3876+70

return strm;

}



