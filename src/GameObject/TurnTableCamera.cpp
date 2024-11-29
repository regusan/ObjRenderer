#include "TurnTableCamera.hpp"

TurnTableCamera::TurnTableCamera(/* args */)
{
}
void TurnTableCamera::matUpdate()
{
    mat = MakeMatOffset(this->location) * MakeMatOffset(Vector3f(0, 0, this->radius)) * MakeRotMat(this->rotation);
}
TurnTableCamera::~TurnTableCamera()
{
}
void TurnTableCamera::OnUpdateInput(const XEvent &event)
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
            case XK_Escape:
                std::cout << "Escape key pressed!" << std::endl;
                break;

            case XK_Return:
                std::cout << "Enter key pressed!" << std::endl;
                break;

            case XK_Up:
                std::cout << "Up arrow key pressed!" << std::endl;
                this->SetRadius(this->radius * 0.9);
                break;

            case XK_Down:
                std::cout << "Down arrow key pressed!" << std::endl;
                this->SetRadius(this->radius * 1.1);
                break;

            case XK_Left:
                std::cout << "Left arrow key pressed!" << std::endl;
                break;

            case XK_Right:
                std::cout << "Right arrow key pressed!" << std::endl;
                break;

            default:
                std::cout << "Other key pressed: " << XKeysymToString(key) << std::endl;
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
void TurnTableCamera::SetRotation(Vector3f rotate)
{
    this->rotation = rotate;
}
void TurnTableCamera::SetPosition(Vector3f position)
{
    this->location = position;
}

Matrix4f TurnTableCamera::getMat()
{
    this->matUpdate();
    return mat;
}