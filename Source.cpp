#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <vector>

using namespace std;

int cpuCores(){
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}
template <class B>
void deAlocMemory(HANDLE h,B *buf){
    UnmapViewOfFile(buf);
    CloseHandle(h);
}

bool isSmaller(float curr,float max){
    return curr <= max;
}
bool findIt(vector<int> &in1,int &val)
{
    for(int i=0; i<in1.size();i++){
        if(in1[i] == val)
            return 1;
    }
    return 0;
}
int main(){

    int cores = cpuCores(),
            seed = 0, prog_index = 0, startTime = time(0);
    float maxWeight=0,maxTime=0;
    DWORD pi_id = GetCurrentProcessId(); //current process id

    //profits
    TCHAR knap_max_pro_name[] = TEXT("knap_pro");
    HANDLE knap_max_pro_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,knap_max_pro_name);
    float *knap_max_pro_arr = (float *)MapViewOfFile(knap_max_pro_handle,FILE_MAP_READ,0,0,100*sizeof(float));

    //weights
    TCHAR knap_max_weight_name[] = TEXT("knap_weight");
    HANDLE knap_max_weight_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,knap_max_weight_name);
    float *knap_max_weight_arr = (float *)MapViewOfFile(knap_max_weight_handle,FILE_MAP_READ,0,0,100*sizeof(float));

    //program initial infos
    TCHAR knap_info[] = TEXT("knap_info");
    HANDLE knap_info_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,knap_info);
    float *knap_info_arr = (float *)MapViewOfFile(knap_info_handle,FILE_MAP_ALL_ACCESS,0,0,(2+2*cores)*sizeof(float));

    //Assigs
    maxTime = knap_info_arr[2*cores];
    maxWeight = knap_info_arr[2*cores+1];

    //finging seeds
    for(int i=cores; i<2*cores; i++){
        if(knap_info_arr[i] == pi_id)
        {
            seed = knap_info_arr[i-cores] ;
            knap_info_arr[i-cores] = 0;
            prog_index = i-cores;
        }

    }

    srand(seed);

    /******************************************/

    int currP = 0; //knap_info_arr[prog_index]

    int spentTime = time(0) - startTime,
        index = 0,
        wight = 0,
        prof  = 0,
        ww    = 0;
    vector<int> in1,in2;

    while(isSmaller(spentTime,maxTime))
    {
        do{
            index = rand()%100;
        }while(findIt(in1,index));

        wight += knap_max_weight_arr[index];
        prof  += knap_max_pro_arr[index];
        in1.push_back(index);

        if(isSmaller(wight,maxWeight))
        {
            if(isSmaller(currP,prof))
            {
                currP = prof;
                ww = wight;
                in2 = in1;
            }
        }
        else
        {
            wight = 0;
            prof = 0;
            in1.clear();
        }

        spentTime = time(0) - startTime;
    }

    knap_info_arr[prog_index] = currP;

//    for (auto i = in2.begin(); i != in2.end(); ++i)
//    {
//        std::cout << *i << ' ';
//    }

    //Unamapping and closing handles
    deAlocMemory(knap_info_handle,knap_info_arr);
    deAlocMemory(knap_max_pro_handle,knap_max_pro_arr);
    deAlocMemory(knap_max_weight_handle,knap_max_weight_arr);

    return 0;
}
