#pragma once

#include "astro/math/math.hpp"
#include <cmath>

namespace astro {
namespace core {
namespace camera {
    
class PerspectiveCamera {
public:
    PerspectiveCamera(int width, int height, float fov_deg, float znear = 0.1f, float zfar = 100.0f)
        : width(width), height(height), znear(znear), zfar(zfar), fov_deg(fov_deg) {
            computeProjectionMatrix();
        }
    ~PerspectiveCamera() = default;

    // Getters
    const math::Mat4f& getViewMatrix() { return ViewMatrix; }
    const math::Mat4f& getProjectionMatrix() { return ProjectionMatrix; }
    const math::Vec3f& getEye() { return m_eye; }
    const math::Vec3f& getTarget() { return m_target; }
    const math::Vec3f& getUp() { return m_up; }
    
    // Functions
    void lookAt(const math::Vec3f& eye, const math::Vec3f& target, const math::Vec3f& up) {
        m_eye = eye;
        m_target = target;
        m_up = up;

        math::Vec3f zaxis = math::normalize(eye - target); // Forward
        math::Vec3f xaxis = math::normalize(math::cross(up, zaxis)); // Right
        math::Vec3f yaxis = math::cross(zaxis, xaxis); // Up

        ViewMatrix = math::Mat4f::Identity();
        ViewMatrix(0,0) = xaxis.x; ViewMatrix(0,1) = xaxis.y; ViewMatrix(0,2) = xaxis.z; ViewMatrix(0,3) = -math::dot(xaxis, eye);
        ViewMatrix(1,0) = yaxis.x; ViewMatrix(1,1) = yaxis.y; ViewMatrix(1,2) = yaxis.z; ViewMatrix(1,3) = -math::dot(yaxis, eye);
        ViewMatrix(2,0) = zaxis.x; ViewMatrix(2,1) = zaxis.y; ViewMatrix(2,2) = zaxis.z; ViewMatrix(2,3) = -math::dot(zaxis, eye);
    }
    

private:
    int width;
    int height;
    float znear;
    float zfar;
    float fov_deg;
    
    math::Vec3f m_eye;
    math::Vec3f m_target;
    math::Vec3f m_up;

    
    math::Mat4f ViewMatrix = math::Mat4f::Identity();
    math::Mat4f ProjectionMatrix = math::Mat4f::Identity();
    
    void computeProjectionMatrix() {
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        float fov_rad = fov_deg * (PI / 180.0f);
        float f = 1.0f / std::tan(fov_rad / 2.0f);
        
        math::Mat4f proj(0);
        proj(0,0) = f / aspect_ratio;
        proj(1,1) = f;

        proj(2,2) = (zfar + znear) / (znear - zfar);
        proj(2,3) = (2.0f * zfar * znear) / (znear - zfar);

        proj(3,2) = -1.0f;
        
        ProjectionMatrix = proj;
    }
};

}
}
}