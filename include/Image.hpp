//
// Created by 闻永言 on 2021/12/22.
//

#ifndef RENDERER_IMAGE_HPP
#define RENDERER_IMAGE_HPP

template<typename T>
class Image {
public:
    int x; // width of image
    int y; // height of image
    int channel;

    T *data;

    /**
     *
     * @param _x width
     * @param _y height
     * @param channel
     */
    Image(int _x, int _y, int _channel) {
        x = _x;
        y = _y;
        channel = _channel;
        data = new T[x * y * channel];
    }

    ~Image() {
        delete[] data;
        data = nullptr;
    }

    /**
     *
     * @param d
     */
    void setData(const T *d) {
        if (!data) {
            data = new T[x * y * channel];
        }
        for (int i = 0; i < x * y * channel; i++) {
            data[i] = d[i];
        }
    }

    /**
     *
     * @param value
     */
    void reset(T value) {
        if (data) {
            for (int i = 0; i < x * y * channel; i++) {
                data[i] = value;
            }
        }
    }
};

#endif //RENDERER_IMAGE_HPP
