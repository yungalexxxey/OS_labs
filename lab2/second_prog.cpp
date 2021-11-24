#include <iostream>
#include <pthread.h>
#include <vector>
#include <assert.h>
#include <chrono>

void sort_betcher(std::vector<int> &arr, int maxth, int res,int split);
void *sort_thread(void *init);
void sort(std::vector<int> &arr, int num,int split);
pthread_mutex_t mutex;

int compare(int &a, int &b)
{

    if (a > b)
    {
            pthread_mutex_lock(&mutex);
        std::swap(a, b);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    return 0;
}

int chet(std::vector<int> &arr,int split)
{   
   // pthread_mutex_lock(&mutex);
    int result = 0;
    for (unsigned int i = 0; i < arr.size() - 1; i += 2*split)
    {
        result += compare(arr[i], arr[i + 1]);
    }
        for(unsigned int i=2;i<arr.size()-1;i+=2*split)
            result += compare(arr[i], arr[i + 1]);
    //pthread_mutex_unlock(&mutex);
    if (result > 0)
        return 1;
    else
        return 0;
}

int nechet(std::vector<int> &arr,int split)
{
  //  pthread_mutex_lock(&mutex);
    int result = 0;
    for (unsigned int i = 1; i < arr.size() - 1; i +=  2*split)
    {   
        result += compare(arr[i], arr[i + 1]);
    }
    for(unsigned int i=3;i<arr.size()-1;i+=2*split)
            result += compare(arr[i], arr[i + 1]);
   // pthread_mutex_unlock(&mutex);
    if (result > 0)
        return 1;
    else
        return 0;
}

struct sort_starter
{
    std::vector<int> &arr;
    int deep;
    int result;
    int split;
};

void *sort_thread(void *init)
{
    sort_starter *start = (sort_starter *)init;
    sort_betcher(start->arr, start->deep, start->result,start->split);
    return NULL;
}

void sort_betcher(std::vector<int> &arr, int deep, int res,int split)
{
    res = chet(arr,split)+nechet(arr,split);
    if (res)
    {   
        if (deep-- > 0)
        {
            pthread_t thread;
            sort_starter arg = {arr, deep, res,2*split};
            int ret = pthread_create(&thread, NULL, sort_thread, &arg);
            assert(ret == 0);
            sort_betcher(arr, deep, res,split);
            pthread_join(thread, NULL);
        }
        else
            sort(arr, 1,split);
    }
}

void sort(std::vector<int> &arr, int res,int split)
{   
    
    res = chet(arr,split) + nechet(arr,split) + chet(arr,split) + nechet(arr,split);
    if (res)
        sort(arr, res,split);
}

int main()
{

    int maxth;
    while (true)
    {
        std::cout << "Insert max num of threads: ";
        std::cin >> maxth;
        if (maxth > 0)
            break;
        else
            printf("Maxnum must be more than 0\n");
    }

    std::vector<int> arr;
    int count;
    while (true)
    {
        std::cout << "Insert length of vector: ";
        std::cin >> count;
        if (count > 0)
            break;
        else
            printf("Length must be more than 0\n");
    }
    srand(17327);
    if (count == 2)
    {
        for (int i = 0; i < count; i++)
            arr.push_back(rand() % 1000);
        compare(arr[0], arr[1]);
    }
    else
    {
        for (int i = 0; i < count; i++)
            arr.push_back(rand() % 1000);
    }
    for (unsigned int i = 0; i < arr.size(); i++)
        printf("%d ", arr[i]);
    printf("\n");
    printf("\n");
    auto first_time = std::chrono::high_resolution_clock::now();
    sort_betcher(arr, maxth, 1,1);

    for (unsigned int i = 0; i < arr.size(); i++)
        printf("%d ", arr[i]);
    printf("\n");
    auto last_time = std::chrono::high_resolution_clock::now();
    auto myperiod = std::chrono::duration_cast<std::chrono::milliseconds>(last_time - first_time).count();
    std::cout << "Time: " << myperiod << " milliseconds" << std::endl;
    return 0;
}