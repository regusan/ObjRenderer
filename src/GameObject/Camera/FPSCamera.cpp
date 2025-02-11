#include "FPSCamera.hpp"

FPSCamera::FPSCamera(/* args */)
{
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::matUpdate()
{
    this->mat = MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeMatOffset(this->location);
}
void FPSCamera::OnUpdateInput(const XEvent &event)
{
    XEvent cpevent = event;
    switch (event.type)
    {
    case KeyPress:
        // キーが押された場合
        {
            KeySym key = XLookupKeysym(&cpevent.xkey, 0);
            // キーごとに処理を分ける
            switch (key)
            {
            case XK_w:
                SetPosition(this->location - GetForwardVector(this->mat.inverse()).head<3>() * this->speed);
                break;
            case XK_s:
                SetPosition(this->location + GetForwardVector(this->mat.inverse()).head<3>() * this->speed);
                break;
            case XK_a:
                SetPosition(this->location + GetRightVector(this->mat.inverse()).head<3>() * this->speed);
                break;
            case XK_d:
                SetPosition(this->location - GetRightVector(this->mat.inverse()).head<3>() * this->speed);
                break;
            case XK_e:
                SetPosition(this->location + GetUpVector(this->mat.inverse()).head<3>() * this->speed);
                break;
            case XK_q:
                SetPosition(this->location - GetUpVector(this->mat.inverse()).head<3>() * this->speed);
                break;

            default:
                break;
            }
        }
        break;
    }
}
