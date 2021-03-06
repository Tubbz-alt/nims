/*
 *  Nekton Interaction Monitoring System (NIMS)
 *
 *  test_blueview.cpp
 *
 *  Created by Shari Matzner on 01/26/2016.
 *  Copyright 2015 Pacific Northwest National Laboratory. All rights reserved.
 *
 */
#include <iostream> // cout, cin, cerr
#include <fstream>  // ifstream, ofstream
#include <string>   // for strings
#include <signal.h>
#include <limits>

#include <opencv2/opencv.hpp>

#include <boost/filesystem.hpp>
#include "yaml-cpp/yaml.h"

#include "data_source_blueview.h"
#include "frame_buffer.h"
#include "nims_ipc.h"
#include "log.h"

using namespace std;
using namespace cv;
using namespace boost;
namespace fs = boost::filesystem;

// TODO: Somehow the defined sonar types need to 
//       be accessible to users for the config file,
//       maybe for user interface
#define NIMS_SONAR_BLUEVIEW 2



int main (int argc, char * argv[]) {
	    
    string cfgpath, log_level;
    if ( parse_command_line(argc, argv, cfgpath, log_level) != 0 ) return -1;
    setup_logging(string(basename(argv[0])), cfgpath, log_level);
   setup_signal_handling();
	
	//--------------------------------------------------------------------------
	// DO STUFF
	NIMS_LOG_DEBUG << "Starting " << argv[0];
	
	int sonar_type;
	string sonar_host_addr;
	string fb_name;
    try 
    {
        YAML::Node config = YAML::LoadFile(cfgpath);
        sonar_type = config["SONAR_TYPE"].as<int>();
        sonar_host_addr = config["SONAR_HOST_ADDR"].as<string>();
        fb_name = config["FRAMEBUFFER_NAME"].as<string>();
     }
     catch( const std::exception& e )
    {
        NIMS_LOG_ERROR << "Error reading config file." << e.what();
        NIMS_LOG_ERROR << desc;
        return -1;
    }
    
      
    // create fb before datasource, so tracker doesn't bail out
    // when it tries to open a nonexistent writer queue
    FrameBufferWriter fb(fb_name);
    if ( -1 == fb.Initialize() )
    {
       NIMS_LOG_ERROR << "Error initializing frame buffer writer.";
       return -1;
    }
   
 	DataSource *input; // This is a virtual class.   
    
	// TODO: Define an enum or ?  Need to match config file definition.
	switch ( sonar_type )
	{
	    case NIMS_SONAR_BLUEVIEW :
            NIMS_LOG_DEBUG << "opening BlueView sonar as datasource";
            input = new DataSourceBlueView(sonar_host_addr);
            break;
        default :
             NIMS_LOG_ERROR << "invalid sonar type: " << sonar_type;
             return -1;
             break;
     } // switch SONAR_TYPE    
   
    // TODO:  May want to check errno to see why connect failed.
    do
    {
        input->connect();
        
        // may get SIGINT at any time to reload config
        if (sigint_received) {
            NIMS_LOG_WARNING << "exiting due to SIGINT";
            break;
        }
        
        // no need to sleep if we connected
        if (input->is_good())
            break;
        
        NIMS_LOG_DEBUG << "waiting to connect to source...";
        sleep(5);
        
   } while ( !input->is_good() );
    
   if (0 == sigint_received) 
   {
       NIMS_LOG_DEBUG << "connected to source!";

      // the framedata_t (frame_buffer.h) is either float or double
      int cv_type = sizeof(framedata_t)==4 ? CV_32FC1 : CV_64FC1;

       size_t frame_count=0;
       while ( input->more_data() )
       {
           cout << "Press any ENTER to get a ping..." << std::flush;
           std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
           
           Frame frame;
           if ( -1 == input->GetPing(&frame) ) break;
    
           // if we get INT during a recv(), GetPing returns -1 
           if (sigint_received) {
               NIMS_LOG_WARNING << "exiting due to SIGINT";
               break;
           }
    
           NIMS_LOG_DEBUG << "got frame!";
           NIMS_LOG_DEBUG << frame.header << endl;
           // Create a cv::Mat wrapper for the ping data
          
            Mat ping_data(frame.header.num_samples, frame.header.num_beams,cv_type,frame.data_ptr());
           double min_val,max_val;
           minMaxIdx(ping_data, &min_val, &max_val);
           NIMS_LOG_DEBUG << "frame values from " << min_val << " to " << max_val;
           Mat im;
           ping_data.convertTo(im,CV_16U);
          imwrite("PingData.png", im);

        
           ++frame_count;
        }
    }
        
	NIMS_LOG_DEBUG << "Ending runloop; SIGINT: " << sigint_received;
    return 0;
}
