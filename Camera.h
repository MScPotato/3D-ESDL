#pragma once

#define CAMSPEED 0.005f

class Camera
{
private:
	
	XMFLOAT3 DefaultForward;
	XMFLOAT3 DefaultRight;
	XMFLOAT3 DefaultUp;

	XMFLOAT3 camForward;
	XMFLOAT3 camRight;
	XMFLOAT3 camUp;

	XMFLOAT4X4 camRotationMatrix;
	float camYaw;
	float camPitch;

	XMFLOAT3 tempForward;
	float tempPitch;

	float moveBackForward;
	float moveLeftRight;

	XMFLOAT3 camTarget;
	XMFLOAT3 camPosition;
	XMFLOAT4X4 camView;

	POINT currMousePos;
	RECT window;
	float mouseSense, boost;
	float mouseX, mouseY;

	int showMouse;

	HWND wndHandle;

	bool cursorLock;
	float cursorLockPosX, cursorLockPosY;

	//ID3D11Buffer* camBuffer;

public:
	Camera(float width, float height, HWND wndHandle);
	//Init(IDXGISwapChain* gSwapChain, ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
	~Camera();

	void UpdateCamera(float dt);
	void UpdateView();
	XMFLOAT4X4 getView()const;
	XMFLOAT3 getPosition()const;


	float dotThese(XMFLOAT3 v1, XMFLOAT3 v2);
};

