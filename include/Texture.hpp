//
// Created by 闻永言 on 2021/9/11.
//

#ifndef RENDERER_TEXTURE_HPP
#define RENDERER_TEXTURE_HPP

#include "Type.hpp"
#include "Util.hpp"
#include "Image.hpp"
#include "opencv2/highgui.hpp"
#include <vector>

template<typename T>
class Mipmap;

namespace Renderer {
    enum SamplerType {
        NORMAL, BILINEAR, TRILINEAR, ANISOTROPIC
    };

    enum Direction {
        CUBE_LEFT = 0, CUBE_FRONT = 1, CUBE_RIGHT = 2, CUBE_BACK = 3, CUBE_TOP = 4, CUBE_BOTTOM = 5
    };

    static const float4 TextureNormalOffset(0.5f, 0.5f, 0.5f, 0); // tangent space normal texture decode offset
}

template<typename T>
class Texture2D {
public:
    Texture2D() = default;

    /**
     *
     * @param data
     */
    explicit Texture2D(Image<T> *data): image(data) {
    }

    /**
     *
     * @param _size
     * @param channel
     */
    explicit Texture2D(int _size, int channel = 1): size(_size), image(new Image<T>(size, size, channel)),
                                                    mipmap(nullptr) {
    }

    /**
     *
     * @param texture_file
     */
    explicit Texture2D(const std::string &texture_file): mipmap(nullptr) {
        cv::Mat img = cv::imread(texture_file, cv::ImreadModes::IMREAD_UNCHANGED);
        assert(img.rows == img.cols && ((img.rows & (img.rows - 1)) == 0)); // size must be 2^N
        size = img.cols;
        image = new Image<T>(size, size, img.channels());
        image->setData(img.data);
    }

    ~Texture2D() {
        delete image;
        delete mipmap;
    }

    /**
     *
     * @oaram texture_uv
     * @param samplerType
     * @return
     */
    float4 sample(const float2 &texture_uv, Renderer::SamplerType samplerType = Renderer::NORMAL) const {
        switch (samplerType) {
            case Renderer::NORMAL:
                return sampleNormal(texture_uv);
            case Renderer::BILINEAR:
                return sampleBilinear(texture_uv);
            default:
                return {};
        }
    }

    /**
     *
     * @param textureCoord
     * @param ddx
     * @param ddy
     * @param samplerType
     * @return
     */
    virtual float4
    sample(const float2 &textureCoord, const float2 &ddx, const float2 &ddy, Renderer::SamplerType samplerType) const {
        if (mipmap) {
            return mipmap->sample(textureCoord, ddx, ddy, samplerType);
        }
        // error
        return {0, 0, 0, 1.f};
    }

    /**
     * Generate mipmap of texture.
     */
    void initializeMipmap() {
        if (!mipmap) {
            mipmap = new Mipmap<T>(this);
            delete image; // to save memory
        }
    }

    /**
     * Reset data.
     * @param value
     */
    void reset(T value) {
        if (image) {
            image->reset(value);
        }
    }

    /**
     *
     * @return size
     */
    int getSize() const {
        return size;
    }

    /**
     *
     * @return channel
     */
    int getChannel() const {
        return image->channel;
    }

    /**
     *
     * @return
     */
    T *getData() const {
        return image->data;
    }

    /**
     *
     * @param index
     * @return
     */
    inline T &operator[](int index) {
        return image->data[index];
    }

private:
    int size;
    Image<T> *image;
    Mipmap<T> *mipmap;

    /**
     *
     * @param x
     * @param y
     * @return
     */
    float4 sampleNormal(int x, int y) const {
        x = x % size;
        y = y % size;
        x = x < 0 ? size + x : x;
        y = y < 0 ? size + y : y;
        int index = image->channel * (y * size + x);
        switch (image->channel) {
            case 1: {
                return {(float) image->data[index] * Renderer::Inv255, 1.f, 1.f, 1.f};
            }
            case 3: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) image->data[index + 2] * Renderer::Inv255,
                    (float) image->data[index + 1] * Renderer::Inv255,
                    (float) image->data[index] * Renderer::Inv255, 1.f
                };
            }
            case 4: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) image->data[index + 2] * Renderer::Inv255,
                    (float) image->data[index + 1] * Renderer::Inv255,
                    (float) image->data[index] * Renderer::Inv255, (float) image->data[index + 3] * Renderer::Inv255
                };
            }
        }
        return {1.f, 1.f, 1.f, 1.f};
    }

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sampleNormal(const float2 &texture_uv) const {
        int x = (int) (texture_uv.x() * (float) size - 0.5f) % size;
        int y = (int) (texture_uv.y() * (float) size - 0.5f) % size;
        x = x < 0 ? size + x : x;
        y = y < 0 ? size + y : y;
        int index = image->channel * (y * size + x);
        switch (image->channel) {
            case 1: {
                return {(float) image->data[index] * Renderer::Inv255, 1.f, 1.f, 1.f};
            }
            case 3: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) image->data[index + 2] * Renderer::Inv255,
                    (float) image->data[index + 1] * Renderer::Inv255,
                    (float) image->data[index] * Renderer::Inv255, 1.f
                };
            }
            case 4: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) image->data[index + 2] * Renderer::Inv255,
                    (float) image->data[index + 1] * Renderer::Inv255,
                    (float) image->data[index] * Renderer::Inv255, (float) image->data[index + 3] * Renderer::Inv255
                };
            }
        }
        return {1.f, 1.f, 1.f, 1.f};
    }

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sampleBilinear(const float2 &texture_uv) const {
        float u_p = texture_uv.x() * (float) size - 0.5f;
        float v_p = texture_uv.y() * (float) size - 0.5f;
        float iu0 = floor(u_p);
        float iv0 = floor(v_p);
        float iu1 = iu0 + 1.f;
        float iv1 = iv0 + 1.f;

        float4 color_0 = sampleNormal((int) iu0, (int) iv1);
        float4 color_1 = sampleNormal((int) iu1, (int) iv1);
        float4 color_2 = sampleNormal((int) iu0, (int) iv0);
        float4 color_3 = sampleNormal((int) iu1, (int) iv0);
        float ratio_u = iu1 - u_p;
        float ratio_v = iv1 - v_p;
        float4 color_x0 = Renderer::lerp(ratio_u, color_1, color_0);
        float4 color_x1 = Renderer::lerp(ratio_u, color_3, color_2);
        return Renderer::lerp(ratio_v, color_x0, color_x1);
    }
};

template<typename T>
class TextureCube {
public:
    /**
     *
     * @param size
     * @param channel
     */
    explicit TextureCube(int size, int channel = 1): faceSize(size) {
        for (auto &i: data) {
            i = new Texture2D<T>(faceSize, 1);
        }
    }

    /**
     *
     * @param textureFile
     */
    explicit TextureCube(const std::string &textureFile) {
        cv::Mat img = cv::imread(textureFile, cv::ImreadModes::IMREAD_UNCHANGED);
        assert(img.rows % 3 == 0 && img.cols % 4 == 0 && img.rows / 3 == img.cols / 4);
        assert(((img.rows / 3) & ((img.rows / 3) - 1)) == 0); // size must be 2^N
        faceSize = img.cols / 4;

        const int offset[6] =
        {
            faceSize * img.cols, faceSize * (img.cols + 1), faceSize * (img.cols + 2), faceSize * (img.cols + 3),
            faceSize, faceSize * (2 * img.cols + 1)
        };
        for (int i = 0; i < 6; i++) {
            data[i] = new Texture2D<T>(faceSize, img.channels());
            for (int r = 0; r < faceSize; r++) {
                for (int c = 0; c < faceSize; c++) {
                    int offsetFace = (r * img.cols + c) * img.channels();
                    int offsetTex = (r * faceSize + c) * img.channels();
                    for (int channel = 0; channel < img.channels(); channel++) {
                        (*data[i])[offsetTex + channel] = img.data[img.channels() * offset[i] + offsetFace + channel];
                    }
                }
            }
        }
    }

    ~TextureCube() {
        for (auto &i: data) {
            delete i;
        }
    }

    /**
     *
     * @param faceIndex
     * @param texture_uv
     * @return
     */
    float4 sample(int faceIndex, const float2 &texture_uv) const {
        int x = (int) (texture_uv.x() * (float) faceSize - 0.5f) % faceSize;
        int y = (int) (texture_uv.y() * (float) faceSize - 0.5f) % faceSize;
        x = x < 0 ? faceSize + x : x;
        y = y < 0 ? faceSize + y : y;
        int index = data->channel * (y * faceSize + x);
        auto *offset = data[faceIndex];

        switch (data->channel) {
            case 1: {
                return {(float) offset->data[index] * Renderer::Inv255, 1.f, 1.f, 1.f};
            }
            case 3: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) offset->data[index + 2] * Renderer::Inv255,
                    (float) offset->data[index + 1] * Renderer::Inv255, (float) offset->data[index] * Renderer::Inv255,
                    1.f
                };
            }
            case 4: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) offset->data[index + 2] * Renderer::Inv255,
                    (float) offset->data[index + 1] * Renderer::Inv255, (float) offset->data[index] * Renderer::Inv255,
                    (float) offset->data[index + 3] * Renderer::Inv255
                };
            }
        }
        return {1.f, 1.f, 1.f, 1.f};
    }

    /**
     * Reset data.
     * @param value
     */
    void reset(T value) {
        for (auto &face: data) {
            face.reset(value);
        }
    }

    /**
     *
     * @return size
     */
    int getSize() const {
        return faceSize;
    }

    /**
     *
     * @return channel
     */
    int getChannel() const {
        return data->channel;
    }

    /**
     *
     * @param index
     * @return
     */
    inline T &operator()(int faceIndex, int index) {
        return (*data[faceIndex])[index];
    }

private:
    int faceSize;
    Texture2D<T> *data[6];
};

template<typename T>
class Mipmap : public Texture2D<T> {
public:
    /**
     *
     * @param texture
     */
    explicit Mipmap(Texture2D<T> *texture) {
        maxLevel = 0;
        int size = texture->getSize();
        maxSize = size;
        int channels = texture->getChannel();

        while (size != 1) {
            maxLevel++;
            size = size >> 1;
        }

        // add level 0
        auto *image_0 = new Image<T>(maxSize, maxSize, channels);
        image_0->setData(texture->getData());
        data.push_back(image_0);

        size = maxSize;
        for (int i = 0; i < maxLevel; i++) {
            size = size >> 1;
            auto *image = new Image<T>(size, size, channels);

            data.push_back(image);
            for (int row = 0; row < size; row++) {
                for (int col = 0; col < size; col++) {
                    int offset = (row * size + col) * channels;
                    int up_offset = (4 * row * size + 2 * col) * channels;
                    for (int c = 0; c < channels; c++) {
                        T d_0 = data[i]->data[up_offset + c];
                        T d_1 = data[i]->data[up_offset + c + channels];
                        T d_2 = data[i]->data[up_offset + c + 2 * size * channels];
                        T d_3 = data[i]->data[up_offset + c + 2 * size * channels + channels];
                        image->data[offset + c] = (d_0 + d_1 + d_2 + d_3) / T(4);
                    }
                }
            }
        }
    }

    /**
     *
     * @param textureFile
     */
    Mipmap(const std::string &textureFile) {
        cv::Mat img = cv::imread(textureFile, cv::ImreadModes::IMREAD_UNCHANGED);
        assert(img.rows == img.cols && ((img.rows & (img.rows - 1)) == 0)); // size must be 2^N

        maxLevel = 0;
        int size = img.rows;
        maxSize = size;
        int channels = img.channels();
        while (size != 1) {
            maxLevel++;
            size = size >> 1;
        }

        // add level 0
        auto *image_0 = new Image<T>(maxSize, maxSize, img.channels());
        image_0->setData(img.data);
        data.push_back(image_0);

        size = img.rows;
        for (int i = 0; i < maxLevel; i++) {
            size = size >> 1;
            auto *image = new Image<T>(size, size, channels);

            data.push_back(image);
            for (int row = 0; row < size; row++) {
                for (int col = 0; col < size; col++) {
                    int offset = (row * size + col) * channels;
                    int up_offset = (4 * row * size + 2 * col) * channels;
                    for (int c = 0; c < channels; c++) {
                        T d_0 = data[i]->data[up_offset + c];
                        T d_1 = data[i]->data[up_offset + c + channels];
                        T d_2 = data[i]->data[up_offset + c + 4 * size];
                        T d_3 = data[i]->data[up_offset + c + 4 * size + channels];
                        image->data[offset + c] = (d_0 + d_1 + d_2 + d_3) / T(4);
                    }
                }
            }
        }
    }

    ~Mipmap() {
        for (auto p: data) {
            delete[] p;
        }
    }

    /**
     *
     * @param textureCoord
     * @param ddx
     * @param ddy
     * @param samplerType
     * @return
     */
    float4 sample(
        const float2 &textureCoord, const float2 &ddx, const float2 &ddy, Renderer::SamplerType samplerType) const {
        float ux = (ddx - textureCoord).lpNorm<2>();
        float uy = (ddy - textureCoord).lpNorm<2>();
        float level = log(fmax(ux, uy) * (float) maxSize) * Renderer::InvLog2;
        level = level > (float) maxLevel ? (float) maxLevel - 0.001f : level;
        int high = (int) (level) + 1;
        int low = (int) (log(fmin(ux, uy) * (float) maxSize) * Renderer::InvLog2);
        if (level < 0) {
            level = 0;
            high = 0;
        }
        low = low < 0 ? 0 : low;

        switch (samplerType) {
            case Renderer::NORMAL:
                return sampleNormal(textureCoord, low);
            case Renderer::BILINEAR:
                return sampleBilinear(textureCoord, low);
            case Renderer::TRILINEAR:
                return sampleTrilinear(textureCoord, level, low);
            case Renderer::ANISOTROPIC:
                return sampleAnisotropic(textureCoord, level, low, high);
            default:
                return {};
        }
    }

private:
    int maxLevel;
    int maxSize; // level 0 texture size

    std::vector<Image<T> *> data;

    /**
     *
     * @param x
     * @param y
     * @param level
     * @return
     */
    float4 sampleNormal(int x, int y, int level) const {
        level = level < 0 ? 0 : level;
        level = level > maxLevel ? maxLevel : level;
        int size = maxSize >> level;
        x = x % size;
        y = y % size;
        x = x < 0 ? size + x : x;
        y = y < 0 ? size + y : y;
        int index = data[level]->channel * (y * size + x);
        switch (data[level]->channel) {
            case 1: {
                return {(float) data[level]->data[index] * Renderer::Inv255, 1.f, 1.f, 1.f};
            }
            case 3: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) data[level]->data[index + 2] * Renderer::Inv255,
                    (float) data[level]->data[index + 1] * Renderer::Inv255,
                    (float) data[level]->data[index] * Renderer::Inv255, 1.f
                };
            }
            case 4: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) data[level]->data[index + 2] * Renderer::Inv255,
                    (float) data[level]->data[index + 1] * Renderer::Inv255,
                    (float) data[level]->data[index] * Renderer::Inv255,
                    (float) data[level]->data[index + 3] * Renderer::Inv255
                };
            }
        }
        return {1.f, 1.f, 1.f, 1.f};
    }

    /**
     *
     * @param textureCoord
     * @param level
     * @return
     */
    float4 sampleNormal(const float2 &textureCoord, int level) const {
        level = level < 0 ? 0 : level;
        level = level > maxLevel ? maxLevel : level;
        int size = maxSize >> level;
        int x = (int) (textureCoord.x() * (float) size - 0.5f) % size;
        int y = (int) (textureCoord.y() * (float) size - 0.5f) % size;
        x = x < 0 ? size + x : x;
        y = y < 0 ? size + y : y;
        int index = data[level]->channel * (y * size + x);
        switch (data[level]->channel) {
            case 1: {
                return {(float) data[level]->data[index] * Renderer::Inv255, 1.f, 1.f, 1.f};
            }
            case 3: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) data[level]->data[index + 2] * Renderer::Inv255,
                    (float) data[level]->data[index + 1] * Renderer::Inv255,
                    (float) data[level]->data[index] * Renderer::Inv255, 1.f
                };
            }
            case 4: {
                // OpenCV color channel: GBR -> RGB
                return {
                    (float) data[level]->data[index + 2] * Renderer::Inv255,
                    (float) data[level]->data[index + 1] * Renderer::Inv255,
                    (float) data[level]->data[index] * Renderer::Inv255,
                    (float) data[level]->data[index + 3] * Renderer::Inv255
                };
            }
        }
        return {1.f, 1.f, 1.f, 1.f};
    }

    /**
     *
     * @param textureCoord
     * @param level
     * @return
     */
    float4 sampleBilinear(const float2 &textureCoord, int level) const {
        level = level < 0 ? 0 : level;
        level = level > maxLevel ? maxLevel : level;
        int size = maxSize >> level;
        float u_p = textureCoord.x() * (float) size - 0.5f;
        float v_p = textureCoord.y() * (float) size - 0.5f;
        float iu0 = floor(u_p);
        float iv0 = floor(v_p);
        float iu1 = iu0 + 1.f;
        float iv1 = iv0 + 1.f;

        float4 color_0 = sampleNormal((int) iu0, (int) iv1, level);
        float4 color_1 = sampleNormal((int) iu1, (int) iv1, level);
        float4 color_2 = sampleNormal((int) iu0, (int) iv0, level);
        float4 color_3 = sampleNormal((int) iu1, (int) iv0, level);
        float ratio_u = iu1 - u_p;
        float ratio_v = iv1 - v_p;
        float4 color_x0 = Renderer::lerp(ratio_u, color_1, color_0);
        float4 color_x1 = Renderer::lerp(ratio_u, color_3, color_2);
        return Renderer::lerp(ratio_v, color_x0, color_x1);
    }

    /**
     *
     * @param textureCoord
     * @param level
     * @param low
     * @return
     */
    float4 sampleTrilinear(const float2 &textureCoord, float level, int low) const {
        if (level < 0.000001f) {
            return sampleBilinear(textureCoord, low);
        }
        float4 low_color = sampleBilinear(textureCoord, low);
        float4 high_color = sampleBilinear(textureCoord, low + 1);
        return Renderer::lerp(level - (float) low, low_color, high_color);
    }

    /**
     *
     * @param textureCoord
     * @param level
     * @param low
     * @param high
     * @return
     */
    float4 sampleAnisotropic(const float2 &textureCoord, float level, int low, int high) const {
        int size = maxSize >> (int) level;
        float u_p = textureCoord.x() * (float) size - 0.5f;
        float v_p = textureCoord.y() * (float) size - 0.5f;
        float iu0 = floor(u_p);
        float iv0 = floor(v_p);
        float iu1 = iu0 + 1.f;
        float iv1 = iv0 + 1.f;

        float4 color_0 = sampleNormal((int) iu0, (int) iv1, level);
        float4 color_1 = sampleNormal((int) iu1, (int) iv1, level);
        float4 color_2 = sampleNormal((int) iu0, (int) iv0, level);
        float4 color_3 = sampleNormal((int) iu1, (int) iv0, level);
        float ratio_u = iu1 - u_p;
        float ratio_v = iv1 - v_p;
        float4 color_x0 = Renderer::lerp(ratio_u, color_1, color_0);
        float4 color_x1 = Renderer::lerp(ratio_u, color_3, color_2);
        return Renderer::lerp(ratio_v, color_x0, color_x1);
    }
};

#endif //RENDERER_TEXTURE_HPP
