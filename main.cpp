#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>
#include <thread>
#include <sys/stat.h>
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <sys/types.h>
#include <io.h>
#include <future>
#include "./fixed_thread_pool.cpp"
#include <future>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <fstream>
#include <Windows.h>

using namespace std;
vector<std::string> files;




//void *operator new(std::size_t size) {
//    printf("分配了：%d字节", size);
//    return malloc(size);
//}


fixed_thread_pool *pool;
std::atomic<int> finishNum;
std::atomic<int> works;
std::atomic<int> fileIndex;
vector<future<void>> v_future;
string *arr = new string[1000];

void getFiles(const string &path) {
    works++;
//    string path = *(string *) arg;
//    std::cout << path.c_str() << endl << flush;
    std::thread::id this_id = std::this_thread::get_id();
    unsigned int t = *(unsigned int *) &this_id;
    int fileSize = 0;
//    std::cout << "thread-" << t << "path" << path.c_str() << std::endl << std::flush;
//文件句柄，win10用long long，win7用long就可以了
    long long hFile = 0;
//文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
//如果是目录,迭代之 //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    string temp = path + "\\" + fileinfo.name;
//                    getFiles(temp);
//                    std::packaged_task<void(string)> task(getFiles);
//                    v_future.emplace_back(std::move(task.get_future()));
                    pool->execute([=] { getFiles(temp); });
//                    task.get_future().wait();
                }
            } else {
                fileSize++;
                string f = path + "\\" + fileinfo.name;
                arr[fileIndex++] = f;
                std::cout<<"fileIndex> "<<fileIndex<<f<<" path end"<<endl;

//                cout <<"thread - "<<t<< "  vector  " << f << endl << flush;
//                cout <<"thread - function"<<t<< flush;
//                printf("path %s\n", f.c_str());
                files.push_back(f);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
        ::finishNum++;
        string msg =
                "\n file> " + std::to_string(fileSize) + "  thread > " + std::to_string(t) + " " + path.c_str();
        std::cout << msg;
//        std::cout <<<<  <<  << t <<" "<< path.c_str() <<" "<< endl ;

//        std::cout << "before-thread-close-" << t << "path" << path.c_str() << std::endl << std::flush;
//        promise.set_value();
//        std::cout << "thread-close-" << t << "path" << path.c_str() << std::endl << std::flush;

    }
}


long int getMs() {
    //struct timeval tp;
    //gettimeofday(&tp, nullptr);
    //return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void Stringsplit(const string &str, const char split, vector<string> &res) {
    if (str == "") return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + split;
    size_t pos = strs.find(split);

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos) {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(split);
    }
}

int main(void) {

//    string wan,test;
//    ifstream readFile("D:\\BigFile\\huaweibak\\wanzheng.txt");
//    ifstream r2("D:\\BigFile\\huaweibak\\test.txt");
//    readFile >> wan;
//    r2 >> test;
//    vector<string> vwan,vtest;
//    Stringsplit(wan, '*',vwan);
//    Stringsplit(test, '*',vtest);
//
//    vwan.pop_back();
//    vtest.pop_back();
//    cout<<vwan.size()<<endl;
//    cout<<vtest.size()<<endl;
////    for (auto all_in :vwan) {
//        for (auto name :vwan) {
//            if(test.find(name) == string :: npos)
//            cout<<name<<endl<<flush;
//        }
////    }
//
//    return 0;
    files.resize(1000);

    //检测硬件并发特性(此句只是用来显示计算机支持的并发线程数量)
    std::cout << std::thread::hardware_concurrency() << std::endl;
    string path = "D:\\apache-tomcat-8.5.78";

    long start = getMs();

//    getFiles(path);
//
//    return 0;

    pool = new fixed_thread_pool(4);
//    std::packaged_task<void(string)> task(getFiles);
//    v_future.emplace_back(std::move(task.get_future()));
    pool->execute([=] { getFiles(path); });
//    getFiles(path);
    std::thread::id this_id = std::this_thread::get_id();
    unsigned int t = *(unsigned int *) &this_id;
    printf("主线程%d\n", t);

    pool->waitFinish();
//    sleep(3);


//    //调用ofstream类创建myout对象
//    ofstream myout("D:\\BigFile\\huaweibak\\wanzheng.txt", ios::in | ios::out | ios::app);
//
//    if (myout.is_open()) {
//        /*在屏幕上输入，输出到文件*/
//        string Char;
////        cin >> Char;
//        for (string name:files) {
//            Char += (name + "*");
//        }
//        myout << Char << endl;
//    }
//    myout.close();
//
//
//    cout << "over" << endl;
//    sleep(2);
    //Sleep(2);
//    while (true) {
//    cout << " size " << files.size() << endl << flush;
//    cout << " works " << works.load(std::memory_order_relaxed) << endl << flush;
//    cout << " endSize " << finishNum.load(std::memory_order_relaxed) << endl << flush;

//        std::cout << data_.use_count() << std::endl << std::flush;
//    }
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        sleep(2);
        cout << " fileIndex " << fileIndex.load(std::memory_order_relaxed) << endl << flush;
        cout << " lastPath " << arr[fileIndex-1] << endl << flush;
        cout << " size " << files.size() << endl << flush;
        cout << " works " << works.load(std::memory_order_relaxed) << endl << flush;
        cout << " endSize " << finishNum.load(std::memory_order_relaxed) << endl << flush;
    }
    cout << "getMsTime" << getMs() - start << endl << flush;
    cout << "all tasks done" << " size " << files.size() << endl << flush;

    return 0;
}