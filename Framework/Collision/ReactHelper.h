#pragma once



#include "reactphysics3d/reactphysics3d.h"
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <vector>
using namespace reactphysics3d;
using namespace DirectX;
//class DirectX::XMVECTOR;
//class reactphysics3d::Transform;


 DirectX::XMVECTOR GetRePhyTranslation(reactphysics3d::Transform);
 DirectX::XMVECTOR GetRePhyRotationQuat(reactphysics3d::Transform);
 DirectX::XMVECTOR GetRePhyRotation(reactphysics3d::Transform);


