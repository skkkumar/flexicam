/* 
Author: Sriram Kumar 

Scenario : 

1. Initialize flexicam robot ports using yarp.

2. serial communication
*/

#include <string>
#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>
using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace boost::system;
using namespace boost::asio;

class serialComm{
	private:
		string portName;
		int buff_size;
		serial_port* sp;
	public:
		serialComm(string pn = "/dev/pts/26",int bs = 100) {
			buff_size = bs;
			portName = pn;
		}
		void open ()
		{
			static io_service ios;
			sp = new serial_port(ios, portName);						
		}
		void close ()
		{
			sp->close();
		}
		void sendData(string message){
			sp->write_some(buffer(message));
		}
		string readData()
		{
			char c;
			std::string result;
			for(;;)
			{
				read(*sp,buffer(&c,1));
				switch(c)
			    	{
					case '\r':
					    break;
					case '\n':
					    return result;
					default:
					    result+=c;
			    	}
			}
		}

};


/***************************************************/
class CtrlModule: public RFModule
{
protected:
  Port yawIn,yawOut,headPitchIn,headPitchOut,neckPitchIn,neckPitchOut,rollIn,rollOut;
  RpcServer rpcPort;
  
  Mutex mutex;
  int startup_context_id;
  serialComm robot;
public:
  /***************************************************/
  bool configure(ResourceFinder &rf)
  {
    string name=rf.find("name").asString().c_str();
    yawIn.open("/"+name+"/yaw:i");
    yawOut.open("/"+name+"/yaw:o");
    headPitchIn.open("/"+name+"/head_pitch:i");
    headPitchOut.open("/"+name+"/head_pitch:o");
    neckPitchIn.open("/"+name+"/neck_pitch:i");
    neckPitchOut.open("/"+name+"/neck_pitch:o");
    rollIn.open("/"+name+"/roll:i");
    rollOut.open("/"+name+"/roll:o");
    
    rpcPort.open("/"+name+"/service");
    attach(rpcPort);


    robot = serialComm();
    robot.open();
    return true;
  }
  
  /***************************************************/
  bool interruptModule()
  {
    yawIn.interrupt();
    yawOut.interrupt();
    headPitchIn.interrupt();
    headPitchOut.interrupt();
    neckPitchIn.interrupt();
    neckPitchOut.interrupt();
    rollIn.interrupt();
    rollOut.interrupt();
    return true;
  }
  
  /***************************************************/
  bool close()
  {
    yawIn.close();
    yawOut.close();
    headPitchIn.close();
    headPitchOut.close();
    neckPitchIn.close();
    neckPitchOut.close();
    rollIn.close();
    rollOut.close();
    robot.close();
    return true;
  }
  
  /***************************************************/
  bool respond(const Bottle &command, Bottle &reply)
  {
    string cmd=command.get(0).asString().c_str();
    if (cmd=="look_down")
    {
     // look_down();
      reply.addString("Yep! I'm looking down now!");
      return true;
    }/*
    else if (cmd=="roll")
    {
      if (cogL[0] || cogR[0])
      {
	roll(cogL,cogR);
	reply.addString("Yeah! I've made it roll like a charm!");
      }
      else
	reply.addString("I don't see any object!");
      
      return true;
    }
    else if (cmd=="home")
    {
      home();
      reply.addString("I've got the hard work done! Going home.");
      return true;
    }
    else
      return RFModule::respond(command,reply);*/
  }
  
  /***************************************************/
  double getPeriod()
  {
    return 0.0;     // sync upon incoming images
  }
  
  /***************************************************/
  bool updateModule()
  {
	string data = robot.readData();
	if (data.length() != 0)
		cout << data << endl << flush; 
	// robot.sendData("hello world");
	return true; 
  }
};


/***************************************************/
int main(int argc, char *argv[])
{   
  Network yarp;
  if (!yarp.checkNetwork())
    return -1;
  CtrlModule mod;
  ResourceFinder rf;
  rf.setVerbose(true);
  rf.setDefault("name","flexicam");
  rf.configure(argc,argv);
  return mod.runModule(rf);
}



