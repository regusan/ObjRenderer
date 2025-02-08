#include "TransformMat.hpp"
namespace Transform
{

    Matrix4f MakeRotMatX(const float &degree)
    {
        float th = degree / 180.0 * M_PI;
        Matrix4f retval = Matrix4f();

        retval << 1, 0, 0, 0,
            0, cos(th), -sin(th), 0,
            0, sin(th), cos(th), 0,
            0, 0, 0, 1;

        return retval;
    }
    Matrix4f MakeRotMatY(const float &degree)
    {
        float th = degree / 180.0 * M_PI;
        Matrix4f retval = Matrix4f();
        retval << cos(th), 0, sin(th), 0,
            0, 1, 0, 0,
            -sin(th), 0, cos(th), 0,
            0, 0, 0, 1;
        return retval;
    }
    Matrix4f MakeRotMatZ(const float &degree)
    {
        float th = degree / 180.0 * M_PI;
        Matrix4f retval = Matrix4f();
        retval << cos(th), -sin(th), 0, 0,
            sin(th), cos(th), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
        return retval;
    }
    Matrix4f MakeRotMat(const Vector3f &rotaiton)
    {
        return MakeRotMatZ(rotaiton.z()) * MakeRotMatY(rotaiton.y()) * MakeRotMatX(rotaiton.x());
    }
    Matrix4f MakeMatOffset(const Vector3f &offset)
    {
        Matrix4f retval = Matrix4f();
        retval << 1, 0, 0, offset.x(),
            0, 1, 0, offset.y(),
            0, 0, 1, offset.z(),
            0, 0, 0, 1;
        return retval;
    }
    Matrix4f MakeMatScale(const Vector3f &scale)
    {
        Matrix4f retval = Matrix4f();
        retval << scale.x(), 0, 0, 0,
            0, scale.y(), 0, 0,
            0, 0, scale.z(), 0,
            0, 0, 0, 1;
        return retval;
    }

    Vector4f GetPositionFromMat(const Matrix4f &mat)
    {
        return Vector4f(mat(0, 3), mat(1, 3), mat(2, 3), 1);
    }
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