#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <atomic>
#include <future>
class fixed_thread_pool {
private:
    struct data {
        std::mutex mtx_;
        std::condition_variable cond_;
        std::condition_variable main_cond_;
        bool is_shutdown_ = false;
        std::queue<std::function<void()>> tasks_;
        short int unDoWorks = 0;
//        std::atomic<int> unDoWorks = {0};
    };
    std::shared_ptr<data> data_;
public:
    explicit fixed_thread_pool(size_t thread_count)
            : data_(std::make_shared<data>()) {
        for (size_t i = 0; i < thread_count; ++i) {
            std::thread([this] {

                std::unique_lock<std::mutex> lk(data_->mtx_);
                while (true) {
                    if (!data_->tasks_.empty()) {
                        auto fun = std::move(data_->tasks_.front());
                        data_->tasks_.pop();
                        lk.unlock();

                        fun();

                        lk.lock();
                        if (--data_->unDoWorks == 0) {
                            data_->main_cond_.notify_one();
                        }
                    } else if (data_->is_shutdown_) {
                        break;
                    } else {
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

    void waitFinish() {
        //挂起主线程，等所有任务完成再回调。
        std::unique_lock<std::mutex> lk(data_->mtx_);
        data_->main_cond_.wait(lk, [this] { return data_->unDoWorks == 0; });
    }

    template<class F, class... Args>
    void execute(F &&fun, Args &&...args) {
        {
            std::lock_guard<std::mutex> lk(data_->mtx_);
            ++data_->unDoWorks;
            data_->tasks_.emplace(std::move(std::bind(std::forward<F>(fun),
                                                      std::forward<Args>(args)...)));
        }
        data_->cond_.notify_one();
    }

};