#include "TurnTableCamera.hpp"

TurnTableCamera::TurnTableCamera(/* args */) : FPSCamera()
{
}
TurnTableCamera::TurnTableCamera(json args) : FPSCamera(args)
{
}

TurnTableCamera::~TurnTableCamera()
{
}
void TurnTableCamera::matUpdate()
{
    this->mat = MakeMatOffset(Vector3f(0, 0, this->radius)) * MakeRotMatX(this->rotation.z()) * MakeRotMatY(this->rotation.y()) * MakeMatOffset(this->location);
}
void TurnTableCamera::OnUpdateInput(const XEvent &event)
{
    FPSCamera::OnUpdateInput(event);
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
            case XK_Up:
                this->SetRadius(this->radius * 0.9);
                std::cout << "CameraBoolRadius:" << this->radius << std::endl;
                break;

            case XK_Down:
                this->SetRadius(this->radius * 1.1);
                std::cout << "CameraBoolRadius:" << this->radius << std::endl;
                break;

            case XK_Left:
                break;

            case XK_Right:
                break;

            default:
                break;
            }
        }
        break;
    }
}
void TurnTableCamera::SetRadius(float radius)
{
    this->radius = radius;
}