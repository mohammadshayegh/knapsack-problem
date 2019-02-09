#include <iostream>
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream>

using namespace std;

int cpuCores(){
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

template<class T>
bool sharedMemory(TCHAR *name,HANDLE& h,int size,T* &buf){

    h = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size*sizeof(T),name);
    buf = (T*)MapViewOfFile(h,FILE_MAP_ALL_ACCESS,0,0,size*sizeof(T));

    if (h == NULL)
    {
        cout << " Error ocured ! \n error : ";
        GetLastError();
        cout << endl;
        return 0;
    }
    else
    {
        return 1;
    }
}

bool processCreator(STARTUPINFO *si,PROCESS_INFORMATION *pi,LPCWSTR path){

    ZeroMemory( si, sizeof(STARTUPINFO));
    si->cb = sizeof(si);
    ZeroMemory( pi, sizeof(PROCESS_INFORMATION));

    return CreateProcess(path,NULL,NULL,NULL,FALSE,0,NULL,NULL,si,pi);
}

template<class B>
void deAlocMemory(HANDLE h,B *buf){
    UnmapViewOfFile(buf);
    CloseHandle(h);
}

template <class T>
void initializer(T* p,T* b){
    ifstream data;
    data.open("----- Enter dta.txt file path on your system -----");

    for(int i=0 ; data >> p [i] >> b[i] ;i++);
    data.close();

}
int main()
{
    int cores = cpuCores(),mTime;
    cout << " enter time in seconds : " ;
    cin >> mTime;
    cout << " Wait... " << endl ;

    //knap profits
    TCHAR pName[] = TEXT("knap_pro");
    HANDLE pH ;
    float *pBuf ;
    sharedMemory(pName,pH,100,pBuf);

    //knap weights
    TCHAR wName[] = TEXT("knap_weight");
    HANDLE wH ;
    float *wBuf ;
    sharedMemory(wName,wH,100,wBuf);

    //knap info
    TCHAR iName[] = TEXT("knap_info");
    HANDLE iH ;
    float *iBuf ;
    sharedMemory(iName,iH,2+2*cores,iBuf);

    initializer(pBuf,wBuf);

    iBuf[2*cores] = mTime;
    iBuf[2*cores+1] = 100;

    srand(time(0));
    for(int i=0; i<cores; i++)
        iBuf[i] = rand();

    /*
         *  iBuf[0] - iBuf[ cores-1 ]       => rand seeds
         *  iBuf[cores] - iBuf[ 2*cores-1 ] => address
         *  iBuf[2*cores]                   => time (in seconds)
         *  iBuf[2*cores+1]                 => knapsack max weight
    */

    HANDLE *pi_hProcesses = new HANDLE[cores];

    for(int i=0; i<cores; i++)
    {
        STARTUPINFO *si = new STARTUPINFO;
        PROCESS_INFORMATION *pi = new PROCESS_INFORMATION;

        processCreator(si,pi,L"----- Enter Source.exe file path on your system -----");
        pi_hProcesses[i] = pi->hProcess;
        iBuf[cores+i] = pi->dwProcessId;

    }

    for(int i=0; i<cores; i++)
        WaitForSingleObject(pi_hProcesses[i],mTime*10e3);

    cout << endl << iBuf[0] << " - " << iBuf[1] << " - "<< iBuf[2]<< " - " << iBuf[3] << endl ;

    _getch();
    deAlocMemory(pH,pBuf);
    deAlocMemory(wH,wBuf);
    deAlocMemory(iH,iBuf);

    return 0;
}


