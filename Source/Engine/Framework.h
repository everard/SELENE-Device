// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

/**
 * \mainpage SELENE Device
 * This software is cross-platform 3D graphics rendering engine, licensed under the
 * MIT License (see LICENSE.txt for details).
 */

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "Core/FileManager/FileManager.h"

#include "Core/Math/CollisionManager.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Volume.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Circle.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Line.h"
#include "Core/Math/Box.h"
#include "Core/Math/Ray.h"

#include "Core/Resources/MeshAnimation/MeshAnimationFactory.h"
#include "Core/Resources/MeshAnimation/MeshAnimation.h"
#include "Core/Resources/Texture/TextureFactory.h"
#include "Core/Resources/Texture/Texture.h"
#include "Core/Resources/Mesh/MeshFactory.h"
#include "Core/Resources/Mesh/Mesh.h"

#include "Core/Material/Material.h"

#include "Scene/Nodes/ParticleSystem.h"
#include "Scene/Nodes/Camera.h"
#include "Scene/Nodes/Actor.h"
#include "Scene/Nodes/Light.h"
#include "Scene/Scene.h"

#include "Network/Message.h"

#include "GUI/TextBox.h"
#include "GUI/Button.h"
#include "GUI/Label.h"
#include "GUI/GUI.h"

#endif
