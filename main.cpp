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
#include "./SimpleThreadPool.cpp"
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
#include <forward_list>
#include <map>
#include <sys/time.h>

using namespace std;
vector<std::string> files;
int bing = std::thread::hardware_concurrency();

forward_list<string> fList;
multimap<long, string> multiMap;



//void *operator new(std::size_t size) {
//    printf("分配了：%d字节", size);
//    return malloc(size);
//}


fixed_thread_pool *pool;
std::atomic<int> finishNum;
std::atomic<int> works;
std::atomic<int> fileIndex;
mutex m;

struct Node {
    Node *next;
    string path;

    Node(string p) : path(std::move(p)) {}
};

class Link {
public:
    Node *head, *end;
    long len;

    Link() : head(new Node("")) {
        end = head;
    }


    void push(Node *&&nNode) {
        end->next = std::forward<Node *>(nNode);
        end = end->next;
        len++;
    }

    long size() const {
        return len;
    }

};

Link *mLink = new Link();

void getFiles(const string &path) {
    Link *tempLink = new Link();
    works++;
    std::thread::id this_id = std::this_thread::get_id();
    unsigned int t = *(unsigned int *) &this_id;
    int fileSize = 0;
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
                    pool->execute(getFiles, temp);
//                    task.get_future().wait();
                }
            } else {
                fileSize++;
                string f = path + "\\" + fileinfo.name;
//                arr[fileIndex++] = f;
                fileIndex++;
//                std::cout << "fileIndex> " << fileIndex << f << " path end" << endl;
//                m.lock();
                tempLink->push(new Node(f));
//                multiMap.insert(std::pair<long, std::string>(fileinfo.time_write, f));
//                m.unlock();
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
        ::finishNum++;


        m.lock();
        mLink->end->next = tempLink->head->next;
        mLink->end = tempLink->end;
        mLink->len += tempLink->len;
        m.unlock();


        string msg =
                "\n file> " + std::to_string(fileSize) + "  thread > " + std::to_string(t) + " " + path.c_str();
//        std::cout << msg;
//        std::cout <<<<  <<  << t <<" "<< path.c_str() <<" "<< endl ;

//        std::cout << "before-thread-close-" << t << "path" << path.c_str() << std::endl << std::flush;
//        promise.set_value();
//        std::cout << "thread-close-" << t << "path" << path.c_str() << std::endl << std::flush;

    }
}


long int getMs() {
    struct timeval tp;
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
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

class a {

};

mutex tmux;

void ttt() {
//    ::tmux.unlock();
//    printf("ttt lock");
//    tmux.lock();
//    tmux.unlock();
    cout << "ttt lock" << flush;
}


int main(void) {
    vector<string> tes;
    tes.resize(7777);
    for (int i = 0; i < 20 * 10000; ++i) {
        tes.push_back(std::to_string(i) + "897897897878979898");
        if (i % 1000 == 0) {
            cout << tes.capacity() << endl << flush;
        }
    }
    cout << tes.size() << flush << endl;
    tes.shrink_to_fit();
    cout << tes.capacity() << flush << endl;
    tes.emplace_back("sdfsfd");
    cout << tes.size() << flush << endl;
    tes.clear();
    cout << tes.size() << flush << endl;

    return 0;
    //检测硬件并发特性(此句只是用来显示计算机支持的并发线程数量)
    std::cout << bing << std::endl;
    string path = "D:\\Download";

    long start = getMs();

//    getFiles(path);
//
//    return 0;

    pool = new fixed_thread_pool(bing);
    std::packaged_task<void(string)> task(getFiles);

//    v_future.emplace_back(std::move(task.get_future()));
    pool->execute(getFiles, path);
//    pool->execute([=] { getFiles(path); });
//    getFiles(path);
    std::thread::id this_id = std::this_thread::get_id();
    unsigned int t = *(unsigned int *) &this_id;
    printf("主线程%d\n", t);

    pool->waitFinish();
//    auto vf=pool->v_future;
//    for (auto &item:vf) {
//        item.get_future().get();
//    }
    cout << " size " << multiMap.size() << endl << flush;
    cout << " size " << mLink->size() << endl << flush;
    cout << " works " << works.load(std::memory_order_relaxed) << endl << flush;
    cout << " endSize " << finishNum.load(std::memory_order_relaxed) << endl << flush;
//    }
    cout << "bing " << bing << " getMsTime " << getMs() - start << endl << flush;

    start = getMs();
//    for (multimap<long, string>::reverse_iterator p = multiMap.rbegin(); p != multiMap.rend(); p++)
////    for (multimap<long, string>::iterator p = multiMap.begin(); p != multiMap.end(); p++)
//        cout<< p->second << endl;
    Node *h = mLink->head->next;
    while (h) {
//        cout << h->path << "\n";
        h = h->next;
    }
    cout << "遍历 getMsTime " << getMs() - start << endl << flush;

    return 0;
}