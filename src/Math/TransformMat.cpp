#include "TransformMat.hpp"
namespace Transform
{

    Matrix4f MakeRotMatX(const float &degree)
    {
        float rad = degree * M_PI / 180.0f;
        return Affine3f(AngleAxisf(rad, Vector3f::UnitX())).matrix();
    }

    Matrix4f MakeRotMatY(const float &degree)
    {
        float rad = degree * M_PI / 180.0f;
        return Affine3f(AngleAxisf(rad, Vector3f::UnitY())).matrix();
    }

    Matrix4f MakeRotMatZ(const float &degree)
    {
        float rad = degree * M_PI / 180.0f;
        return Affine3f(AngleAxisf(rad, Vector3f::UnitZ())).matrix();
    }
    Matrix4f MakeRotMat(const Vector3f &rotaiton)
    {
        return MakeRotMatZ(rotaiton.z()) * MakeRotMatY(rotaiton.y()) * MakeRotMatX(rotaiton.x());
    }
    Matrix4f MakeMatOffset(const Vector3f &offset)
    {
        return Affine3f(Translation3f(offset)).matrix();
    }
    Matrix4f MakeMatScale(const Vector3f &scale)
    {
        return Eigen::Affine3f(Eigen::Scaling(scale)).matrix();
    }

    Vector4f GetPositionFromMat(const Matrix4f &mat)
    {
        return Vector4f(mat(0, 3), mat(1, 3), mat(2, 3), 1);
    }

    Vector3f GetScaleFromMat(const Matrix4f &mat)
    {
        return Vector3f(mat.block<3, 3>(0, 0).colwise().norm());
    }
    /*
        Matrix4f GetRotationFromMat(const Matrix4f &mat)
        {
            Matrix3f rotation = mat.block<3, 3>(0, 0);
            Vector3f scale = GetScaleFromMat(mat);
            return rotation.array().colwise() / scale.array(); // スケールを除去
        }
        */
    Matrix4f ResetScale(const Matrix4f &mat)
    {

        Vector3f nX = Vector3f(mat(0, 0), mat(1, 0), mat(2, 0)).normalized();
        Vector3f nY = Vector3f(mat(0, 1), mat(1, 1), mat(2, 1)).normalized();
        Vector3f nZ = Vector3f(mat(0, 2), mat(1, 2), mat(2, 2)).normalized();

        Matrix4f normaled = Matrix4f(mat);
        normaled << nX.x(), nY.x(), nZ.x(), mat(0, 3),
            nX.y(), nY.y(), nZ.y(), mat(1, 3),
            nX.z(), nY.z(), nZ.z(), mat(2, 3),
            0, 0, 0, 1;
        return normaled;
    }

    Matrix4f ResetPosition(const Matrix4f &mat)
    {
        return SetPosition(mat, Vector4f(0, 0, 0, 1));
    }

    Matrix4f SetPosition(const Matrix4f &mat, const Vector4f &pos)
    {
        Matrix4f transformed = Matrix4f(mat);
        transformed(0, 3) = pos.x();
        transformed(1, 3) = pos.y();
        transformed(2, 3) = pos.z();
        return transformed;
    }
    Vector4f GetForwardVector(const Matrix4f &mat)
    {
        return mat.col(2);
    }
    Vector4f GetRightVector(const Matrix4f &mat)
    {
        return mat.col(0);
    }
    Vector4f GetUpVector(const Matrix4f &mat)
    {
        return mat.col(1);
    }
}