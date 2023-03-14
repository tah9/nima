#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <atomic>

class fixed_thread_pool {
private:
    struct data {
        std::mutex mtx_;
        std::condition_variable cond_;
        bool is_shutdown_ = false;
        std::queue<std::function<void()>> tasks_;
//        short int unDoWorks = 0;
        std::atomic<int> unDoWorks = {0};
    };
    std::shared_ptr<data> data_;
public:
    explicit fixed_thread_pool(size_t thread_count)
            : data_(std::make_shared<data>()) {
        for (size_t i = 0; i < thread_count; ++i) {
            std::thread([=] {//原写法data=data_


//                std::cout << "thread-" << t << std::endl << std::flush;
                //初次上锁
                std::unique_lock<std::mutex> lk(data_->mtx_);
                std::thread::id this_id = std::this_thread::get_id();
                unsigned int t = *(unsigned int *) &this_id;
//                std::cout << "lock-thread-" << t << std::endl << std::flush;

                while (true) {
//                    std::cout << data_->unDoWorks << "-thread->" << t << "锁定" << std::endl << std::flush;
                    if (!data_->tasks_.empty()) {
                        auto current = std::move(data_->tasks_.front());
                        data_->tasks_.pop();
                        lk.unlock();//解锁，队列为空
//                        std::cout << "thread-" << t << "解锁" << std::endl << std::flush;

                        current();//执行，还没添加上去，此时队列为空，结束时会添加上去新的任务

                        lk.lock();//该次任务执行结束
//                        std::cout << "thread-" << t << "执行完毕，上锁" << std::endl << std::flush;
                        data_->unDoWorks--;
                        if (data_->unDoWorks == 0) {
                            data_->cond_.notify_all();
                        }
                    } else if (data_->is_shutdown_) {
                        break;
                    } else {
//                        std::cout << "thread-" << t << "挂起" << std::endl << std::flush;
                        data_->cond_.wait(lk);
                    }
                }
            }).detach();
        }
    }

    fixed_thread_pool() = default;

    fixed_thread_pool(fixed_thread_pool &&) = default;

    ~fixed_thread_pool() {
        if ((bool) data_) {
            {
                std::lock_guard<std::mutex> lk(data_->mtx_);
                data_->is_shutdown_ = true;
            }
            data_->cond_.notify_all();
        }
        printf("~fixed_thread_pool()");
    }

    void waitFinish(unsigned int ms = 1000) {
//        data_->mtx_.lock();
//        while (true) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(150));
//            std::cout << data_.use_count() << std::endl << std::flush;
//        }


//        while (data_->unDoWorks||!data_->tasks_.empty()) {
////            data_->mtx_.unlock();
//            std::cout << "waitFinish-锁定" << std::endl << std::flush;
//            usleep(ms * 1000);
////            data_->mtx_.try_lock();
//        }
//        std::cout << "waitFinish-结束锁定" << std::endl << std::flush;
//        std::cout << "undowork" << data_->unDoWorks << std::flush;

//        usleep(ms * 1000);

        //阻塞主线程，等所有任务完成再回调。
//        std::unique_lock<std::mutex> lk(data_->mtx_);
//        data_->cond_.wait(lk, [this] { return data_->unDoWorks == 0; });

//        bool finish = false;
//        while (!finish) {
//            usleep(ms * 1000);
//        }
    }

    template<class F>
    void execute(F &&task) {
        {
            std::lock_guard<std::mutex> lk(data_->mtx_);
            data_->unDoWorks++;
            data_->tasks_.emplace(std::forward<F>(task));
        }
        data_->cond_.notify_one();
    }

};