#include "pch.h"
#include "Camera.h"

Camera::Camera(float width, float height, HWND wndHandle)
{
	this->camPosition = { 0.f, 0.f, -2.f };
	this->DefaultForward = { 0.0f, 0.0f, 1.0f };
	this->DefaultRight = { 1.0f, 0.0f, 0.0f };
	this->DefaultUp = { 0.0f, 1.0f, 0.0f };

	this->camForward = DefaultForward;
	this->camRight = DefaultRight;
	this->camUp = DefaultUp; //XMVector3Cross(camForward, camRight);
	
	this->moveBackForward = 0.0f;
	this->moveLeftRight = 0.0f;

	this->camYaw = 0.0f;
	this->camPitch = -90.0f;
	this->camTarget = { 0.f, 0.f, 1.f };

	this->tempForward = DefaultForward;
	this->tempPitch = camPitch;

	GetWindowRect(wndHandle, &window);

	GetCursorPos(&currMousePos);
	this->mouseSense = CAMSPEED * 20;
	this->mouseX = window.right / 2;
	this->mouseY = window.bottom / 2;

	this->showMouse = 0;

	//this->wndHandle = wndHandle;

	this->cursorLock = false;
	this->cursorLockPosX = 0;
	this->cursorLockPosY = 0;

	XMStoreFloat4x4(&camView, XMMatrixLookAtLH(
		{ camPosition.x, camPosition.y, camPosition.z },
		{ camTarget.x, camTarget.y, camTarget.z },
		{ camUp.x, camUp.y, camUp.z }));
}


Camera::~Camera()
{
}

void Camera::UpdateCamera(float dt)
{
	if (GetAsyncKeyState('W') < 0)
	{
		camPosition = {
		camPosition.x + camForward.x * CAMSPEED * dt,
		camPosition.y + camForward.y * CAMSPEED * dt,
		camPosition.z + camForward.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState('S'))
	{
		camPosition = {
			camPosition.x - camForward.x * CAMSPEED * dt,
				camPosition.y - camForward.y * CAMSPEED * dt,
				camPosition.z - camForward.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState('D') < 0)
	{
		camPosition = {
			camPosition.x + camRight.x * CAMSPEED * dt,
				camPosition.y + camRight.y * CAMSPEED * dt,
				camPosition.z + camRight.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState('A'))
	{
		camPosition = {
			camPosition.x - camRight.x * CAMSPEED * dt,
				camPosition.y - camRight.y * CAMSPEED * dt,
				camPosition.z - camRight.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		camPosition = {
			camPosition.x + DefaultUp.x * CAMSPEED * dt,
				camPosition.y + DefaultUp.y * CAMSPEED * dt,
				camPosition.z + DefaultUp.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState(VK_LCONTROL))
	{
		camPosition = {
			camPosition.x - DefaultUp.x * CAMSPEED * dt,
				camPosition.y - DefaultUp.y * CAMSPEED * dt,
				camPosition.z - DefaultUp.z * CAMSPEED * dt
		};
	}

	if (GetAsyncKeyState('E') && !cursorLock)
	{
		camYaw = 10 * CAMSPEED * dt;
		XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
			XMMatrixRotationRollPitchYaw(0.f, XMConvertToRadians(camYaw), 0.f)));
		XMStoreFloat3(&camForward, XMVector3Normalize(XMLoadFloat3(&camForward)));
	}

	if (GetAsyncKeyState('Q') && !cursorLock)
	{
		camYaw = -10 * CAMSPEED * dt;
		XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
			XMMatrixRotationRollPitchYaw(0.f, XMConvertToRadians(camYaw), 0.f)));
		XMStoreFloat3(&camForward, XMVector3Normalize(XMLoadFloat3(&camForward)));
	}

	tempForward = camForward;
	tempPitch = camPitch;

	if (GetAsyncKeyState('Z') && !cursorLock)
	{
		tempPitch = 10 * CAMSPEED * dt;
		XMStoreFloat3(&tempForward, XMVector3TransformCoord(XMLoadFloat3(&tempForward),
			XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(tempPitch))));
		if (-0.95f < dotThese(tempForward, camUp) && dotThese(tempForward, camUp) < 0.95f)
		{
			camPitch = tempPitch;
			camForward = tempForward;
			XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
				XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(camPitch))));
			XMStoreFloat3(&camForward, XMVector3Normalize(XMLoadFloat3(&camForward)));
		}
	}

	if (GetAsyncKeyState('X') && !cursorLock)
	{
		tempPitch = -10 * CAMSPEED * dt;
		XMStoreFloat3(&tempForward, XMVector3TransformCoord(XMLoadFloat3(&tempForward),
			XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(tempPitch))));
		if (-0.95f < dotThese(tempForward, camUp) && dotThese(tempForward, camUp) < 0.95f)
		{
			camPitch = tempPitch;
			camForward = tempForward;
			XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
				XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(camPitch))));
			XMStoreFloat3(&camForward, XMVector3Normalize(XMLoadFloat3(&camForward)));
			
		}
	}

	if (GetAsyncKeyState('F'))
	{
		this->camPosition = { 0.f, 0.f, -2.f };
		this->camTarget = { 0.f, 0.f, 1.f };
		this->camForward = DefaultForward;
		this->camRight = DefaultRight;
		this->camUp = DefaultUp;
	}

	GetCursorPos(&currMousePos);	
	//GetWindowRect(wndHandle, &window);
	
	if ((currMousePos.x < window.right && currMousePos.x > window.left) && (currMousePos.y < window.bottom && currMousePos.y > window.top))
	{
		if(GetAsyncKeyState(VK_RBUTTON))
		{
			if (!cursorLock)
			{
				cursorLockPosX = currMousePos.x;
				cursorLockPosY = currMousePos.y;
				cursorLock = true;
			}
			camYaw += fmodf((currMousePos.x - cursorLockPosX), 360.f);
			camYaw *= mouseSense;

			tempPitch = camPitch;
			tempForward = camForward;

			tempPitch += (currMousePos.y - cursorLockPosY);
			tempPitch *= mouseSense;

			XMStoreFloat3(&tempForward, XMVector3TransformCoord(XMLoadFloat3(&tempForward),
				XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(tempPitch))));

			if (-0.95f < dotThese(tempForward, camUp) && dotThese(tempForward, camUp) < 0.95f)
			{
				camPitch = tempPitch;
				camForward = tempForward;

				XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
					XMMatrixRotationAxis(XMLoadFloat3(&camRight), XMConvertToRadians(camPitch))));
				XMStoreFloat3(&camForward, XMVector3Normalize(XMLoadFloat3(&camForward)));
			}
			
			if (showMouse >= 0)
				showMouse = ShowCursor(false);
			SetCursorPos(cursorLockPosX, cursorLockPosY);

			XMStoreFloat3(&camForward, XMVector3TransformCoord(XMLoadFloat3(&camForward),
				XMMatrixRotationAxis(XMLoadFloat3(&camUp), XMConvertToRadians(camYaw))));
			
		}
		else if (showMouse < 0)
		{
			showMouse = ShowCursor(true);
			cursorLock = false;
		}
		//Röra sig framåt med musen!
		if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON))
		{
			camPosition = {
				camPosition.x + camForward.x * CAMSPEED * dt,
				camPosition.y + camForward.y * CAMSPEED * dt,
				camPosition.z + camForward.z * CAMSPEED * dt
			};
		}
	}

	XMStoreFloat3(&camRight, XMVector3Cross(XMLoadFloat3(&camUp), XMLoadFloat3(&camForward)));	
	
	camTarget = { 
		camForward.x + camPosition.x, 
		camForward.y + camPosition.y, 
		camForward.z + camPosition.z };

	UpdateView();
}

void Camera::UpdateView()
{
	XMStoreFloat4x4(&camView,
		XMMatrixLookAtLH(
			XMLoadFloat3(&camPosition),
			XMLoadFloat3(&camTarget),
			XMLoadFloat3(&camUp)
		));
}

XMFLOAT4X4 Camera::getView()const
{
	return this->camView;
}

XMFLOAT3 Camera::getPosition()const
{
	return this->camPosition;
}

float Camera::dotThese(XMFLOAT3 v1, XMFLOAT3 v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

