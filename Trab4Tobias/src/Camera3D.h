#ifndef ___CAMERA3D__H___
#define ___CAMERA3D__H___

#include "Vector3.h"
#include "Matrix4x4.h"
#include "gl_canvas2d.h"
#include <cmath>

class Camera3D {
public:
    bool usePerspectiveProjection = true;
    float orbitAngleY = 0.0f;
    float orbitAngleX = 0.3f;
    float distance = 200.0f;
    Vector3 lookAt = {0,0,0};
    float fovY = 60.0f;
    Vector3 currentWorldPos;

    Matrix4x4 buildViewMatrix() {
        Vector3 eye(
            distance * cos(orbitAngleX) * sin(orbitAngleY),
            distance * sin(orbitAngleX),
            distance * cos(orbitAngleX) * cos(orbitAngleY)
        );
        eye = eye + lookAt; 
        this->currentWorldPos = eye;

        Vector3 target = lookAt;
        Vector3 up = Vector3(0,1,0);

        Vector3 zaxis = eye - target; zaxis.normalize();
        Vector3 xaxis = Vector3::cross(up, zaxis); xaxis.normalize();
        Vector3 yaxis = Vector3::cross(zaxis, xaxis);

        Matrix4x4 view;
        view.m[0][0] = xaxis.x; view.m[0][1] = xaxis.y; view.m[0][2] = xaxis.z; view.m[0][3] = -Vector3::dot(xaxis, eye);
        view.m[1][0] = yaxis.x; view.m[1][1] = yaxis.y; view.m[1][2] = yaxis.z; view.m[1][3] = -Vector3::dot(yaxis, eye);
        view.m[2][0] = zaxis.x; view.m[2][1] = zaxis.y; view.m[2][2] = zaxis.z; view.m[2][3] = -Vector3::dot(zaxis, eye);
        view.m[3][0] = 0;       view.m[3][1] = 0;       view.m[3][2] = 0;       view.m[3][3] = 1;
        
        return view;
    }

    Matrix4x4 buildProjectionMatrix(float screenWidth, float leftAreaWidth, float screenHeight) {
        Matrix4x4 proj;
        float divisionX = screenWidth * leftAreaWidth;
        float rightAreaWidth_calc = screenWidth - divisionX;
        float aspect = (screenHeight == 0) ? 1.0f : rightAreaWidth_calc / screenHeight;
        float nearPlane = 0.1f, farPlane = 10000.0f;

        if (usePerspectiveProjection) {
            float top = tan(fovY * 0.5f * PI / 180.0f) * nearPlane;
            float right = top * aspect;
            
            if (std::abs(right) < 0.0001f) right = 0.0001f * (right > 0 ? 1 : -1);
            if (std::abs(top) < 0.0001f) top = 0.0001f * (top > 0 ? 1 : -1);

            proj.m[0][0] = nearPlane / right;
            proj.m[1][1] = nearPlane / top;
            proj.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
            proj.m[2][3] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
            proj.m[3][2] = -1.0f;
            proj.m[3][3] = 0.0f;
        } else {
            float orthoHeight = distance * 0.5f;
            float orthoWidth = orthoHeight * aspect;
            
            proj.m[0][0] = 1.0f / orthoWidth;
            proj.m[1][1] = 1.0f / orthoHeight;
            proj.m[2][2] = -2.0f / (farPlane - nearPlane);
            proj.m[2][3] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        }
        return proj;
    }
};

#endif // ___CAMERA3D__H___