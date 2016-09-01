//
// Created by Masutangu on 16/8/20.
//

#ifndef SAF_SINGLETON_H
#define SAF_SINGLETON_H

namespace saf {

//非线程安全
template <typename T>
class Singleton {
public:
    static T* instance();
private:
    static T _instance;
};

template <typename T>
T Singleton<T>::_instance;

template <typename T>
T* Singleton<T>::instance() {
    return &_instance;
}

}



#endif //SAF_SINGLETON_H
