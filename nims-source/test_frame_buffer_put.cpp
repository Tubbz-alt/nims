/*
 *  Nekton Interaction Monitoring System (NIMS)
 *
 *  test_frame_buffer_put.cpp
 *  
 *  Created by Shari Matzner on 04/23/2015.
 *  Copyright 2015 Pacific Northwest National Laboratory. All rights reserved.
 *
 */
#include <iostream>   // cout, cin, cerr
//#include <fstream>  // ifstream, ofstream
#include <string>     // for strings
#include <thread>     // sleep
#include <chrono>     // time stuff
#include <exception>  // exception class


//#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "yaml-cpp/yaml.h"

#include "frame_buffer.h"

using namespace std;
using namespace boost;
namespace po = boost::program_options;
//namespace fs = boost::filesystem;

int main (int argc, char * const argv[]) {
	//--------------------------------------------------------------------------
    // PARSE COMMAND LINE
	//
	po::options_description desc;
	desc.add_options()
	("help",                                                    "print help message")
  ("cfg,c", po::value<string>()->default_value("config.yaml"),         "path to config file")
	("buffer,b", po::value<string>()->default_value( "test" ),    "name of frame buffer")
	;
	po::variables_map options;
    try
    {
        po::store( po::parse_command_line( argc, argv, desc ), options );
    }
    catch( const std::exception& e )
    {
        cerr << "Sorry, couldn't parse that: " << e.what() << endl;
        cerr << desc << endl;
        return -1;
    }
	
	po::notify( options );
	
    if( options.count( "help" ) > 0 )
    {
        cerr << desc << endl;
        return 0;
    }
	
  string buffer_name = options["buffer"].as<string>();
  
  YAML::Node config = YAML::LoadFile(options["cfg"].as<string>());
    
	//--------------------------------------------------------------------------
	// DO STUFF
	cout << endl << "Starting " << argv[0] << endl;
	
	Frame new_frame;
	try 
	{
    // create as the writer
	FrameBufferWriter fb(buffer_name, config["FB_WRITER_QUEUE"].as<string>()); 
  
	//if (!fb.IsOpen())
	//{
	    //perror(argv[0]);
	    //return -1;
	//}
	int frame_index = -1;
	std::this_thread::sleep_for (std::chrono::seconds(10));
	for (int k=0; k<7; ++k)
	{
	    frame_index = fb.PutNewFrame(new_frame);
	    cout << argv[0] << ": " << "put frame " << frame_index << endl;
	    std::this_thread::sleep_for (std::chrono::seconds(1));
	}
	   
    }
	catch( const std::exception& e )
	{
        cerr << argv[0] << e.what() << endl;
	}
    
	cout << endl << "Ending " << argv[0] << endl << endl;
    return 0;
}
