#pragma once

namespace containers
{
    /**
    * \brief Thread-Safe Queue using Condition Variables
    * \tparam T The type of the stored elements. The behavior is undefined if T is not the same type as Container::value_type. (since C++17)
    * \tparam Container 	The type of the underlying container to use to store the elements.
    * The container must satisfy the requirements of SequenceContainer.
    * Additionally, it must provide the following functions with the usual semantics:\n
    * back()\n
    * front()\n
    * push_back()\n
    * pop_front()\n
    * pop_back()\n
    * The standard containers std::deque and std::list satisfy these requirements.
    */
    template <class T, class Container = std::deque<T>>
    class ThreadSafeQueue
    {
    public:
        using value_type = typename Container::value_type;
        using container_type = Container;

        ThreadSafeQueue() = default;
        virtual ~ThreadSafeQueue() = default;

        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue(const ThreadSafeQueue&&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&&) = delete;

        /**
         * \brief Clears entire ThreadSafeQueue
         */
        void clear();

        /**
         * \brief Inserts element at the end
         * \param data Variable to insert
         */
        void push(const T& data);

        /**
         * \brief Inserts element at the end
         * \param data Variable to insert
         */
        void push(T&& data);

        /**
         * \return The number of elements
         */
        std::size_t size();

        /**
         * \brief Checks whether the underlying container is empty
         * \return true if the underlying container is empty, false otherwise
         */
        bool is_empty();

        /**
         * \brief Accessing the first variable and removing it from the ThreadSafeQueue
         * \param value Variable for assigning the received value
         * \return true if the success, false otherwise
         */
        [[nodiscard]]
        bool try_pop(T& value);

        /**
         * \brief Waits for the ThreadSafeQueue to be released and gets access to the first variable and its removal from the ThreadSafeQueue
         * \param value Variable for assigning the received value
         */
        void wait_and_pop(T& value);

        [[nodiscard]]
        bool try_steal(value_type& res);

        [[nodiscard]]
        const T& front();

    protected:
        Container container_;
        std::mutex mutex_;
        std::condition_variable condition_;
    };

    template <class T, class Container>
    void ThreadSafeQueue<T, Container>::clear()
    {
        std::unique_lock<std::mutex> loc(mutex_);

        Container empty;
        std::swap(empty, container_);
    }

    template <class T, class K>
    void ThreadSafeQueue<T, K>::push(const T& data)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        container_.push_back(data);
        lock.unlock();
        condition_.notify_one();
    }

    template <class T, class Container>
    void ThreadSafeQueue<T, Container>::push(T&& data)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        container_.push_back(std::move(data));
        lock.unlock();
        condition_.notify_one();
    }

    template <class T, class K>
    std::size_t ThreadSafeQueue<T, K>::size()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return container_.size();
    }

    template <class T, class K>
    bool ThreadSafeQueue<T, K>::is_empty()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return container_.empty();
    }

    template <class T, class K>
    bool ThreadSafeQueue<T, K>::try_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (container_.empty()) return false;

        value = std::move(container_.front());
        container_.pop_front();

        return true;
    }

    template <class T, class K>
    void ThreadSafeQueue<T, K>::wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (container_.empty()) condition_.wait(lock);

        value = std::move(container_.front());
        container_.pop_front();
    }

    template <class T, class Container>
    bool ThreadSafeQueue<T, Container>::try_steal(value_type& res)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (container_.empty()) return false;

        res = std::move(container_.back());
        container_.pop_back();

        return true;
    }

    template <class T, class Container>
    const T& ThreadSafeQueue<T, Container>::front()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return container_.front();
    }
}