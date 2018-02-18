#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
using namespace std;

int Terget_temp=50;
string System_temp_file="/sys/class/thermal/thermal_zone0/temp";
string System_load_freq_file="/sys/devices/system/cpu/intel_pstate/max_perf_pct";
int step=10;
int ISTERESYS=5; //in degrees
int sleep_Time=1000;
bool loop=true;

void my_handler(int s){
    printf("Caught signal %d\n",s);
    loop=false;
    //exit(1);
}

string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        cout<<"Error";
        throw;
    }
    pclose(pipe);
    return result;
}

int main()
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    while(loop) {
        //read temp
        string result = exec(("cat " + System_temp_file).c_str());
        int CPUtemp = atoi(result.c_str()) / 1000;
        int currentPercent = atoi(exec(("cat " + System_load_freq_file).c_str()).c_str());
        int newPerc = currentPercent;
        if (CPUtemp >= Terget_temp) {
            int newPerc = newPerc - step;
        }
        else if(CPUtemp<Terget_temp-ISTERESYS)
        {
            int newPerc = newPerc + step;
            if(newPerc>100)
                newPerc=100;
        }
        cout << "From  " << currentPercent << "% To: " << newPerc<<"%";
        result = exec(("echo " + to_string(newPerc) + ">" + System_load_freq_file).c_str());
        cout << "message: " << result;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_Time));
    }
}



