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
#include <boost/regex.hpp>
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
		serial_port* sp;
	public:
		serialComm(string pn = "/dev/pts/27") {
			portName = pn;
		}
		void open ()
		{
			static io_service ios;
			sp = new serial_port(ios, portName);
			sendData("s");  // TO START THE ROBOT
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
			cout << result << endl << flush;
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


class port2serial : public RateThread {
	ResourceFinder &rf;
	BufferedPort<Bottle> *b_port;
	serialComm *robot;
	public:
		port2serial(ResourceFinder &_rf, BufferedPort<Bottle> &b_p, serialComm &r) : RateThread(20), rf(_rf) {b_port = &b_p;robot = &r;}
		void readfromport(){
			cout << "rfp" << endl << flush;
			Bottle *message = b_port->read();
			string data = message->get(0).asString().c_str();
			cout << data << endl << flush;
	  	}



		bool threadInit()
		{
		  setRate(int(20.0));
		  return true;
		}

		void run()
		{readfromport();cout << "after reading from port" << endl << flush;}

    void interrupt()
    {
    }


bool respComd(const Bottle &Command){
    return true;
}

void threadRelease(){}

};



/***************************************************/
class CtrlModule: public RFModule
{
protected:
  BufferedPort<Bottle> yawIn,yawOut,headPitchIn,headPitchOut,neckPitchIn,neckPitchOut,rollIn,rollOut;
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
cout << "before starting robot" << endl << flush;

    robot = serialComm();
    robot.open();
cout << "Before creating the object" << endl << flush;
    port2serial p2s_roll(rf,rollOut,robot), p2s_headPitch(rf,headPitchOut,robot), p2s_neckPitch(rf,neckPitchOut,robot), p2s_yaw(rf,yawOut,robot);
cout << "After creating the object" << endl << flush;
    start_thread(rf, p2s_roll);

    start_thread(rf, p2s_headPitch);

    start_thread(rf, p2s_yaw);

    start_thread(rf, p2s_neckPitch);
    return true;
  }
  bool start_thread (ResourceFinder &rf,port2serial &p2s){
	if (!p2s.start())
	{
		cout << "Not able to start the thread" << endl << flush;
		return false;
	}
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
  

  void writetoport(BufferedPort<Bottle> &b_port, int d){
	Bottle& data = b_port.prepare();
	data.addInt(d);
	b_port.write();
  }





  bool serial2bottle(){
	string data = robot.readData();
	if (data.length() != 0){
		boost::regex motormatch("([0-9]+) ([0-9]+)");
		boost::regex tokenizer("([0-9]+)");
                int matches[2];
		if(regex_match(data, motormatch)){
			boost::sregex_iterator m1(data.begin(), data.end(), tokenizer);
			boost::sregex_iterator m2;
			for (int i = 0; m1 != m2; ++m1, i++){
				matches[i] = atoi(m1->str().c_str());
			}
			switch (matches[0]){
				case 1:
					writetoport(rollIn,matches[1]);
					break;
				case 2:
					writetoport(headPitchIn,matches[1]);
					break;
				case 3:
					writetoport(neckPitchIn,matches[1]);
					break;
				case 4:
					writetoport(yawIn,matches[1]);
					break;
				default:
					cout << "Given condition not in the database" << endl << flush;
					break;
			}
		} else {
		cout<<"Error : Data not match"<<endl<<flush;
		return false;
		}
	return true;
  	}
  return true;
  }

  /***************************************************/
  bool updateModule()
  {
	if (serial2bottle()){
		return true;	
	}
	else {
		cout << "Error in serial2bottle"<<endl<<flush;
		return false;
	}
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



