// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSOBJECT_HPP
#define NAZARA_PHYSOBJECT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Nazara/Physics/Geom.hpp>

class NzPhysWorld;
struct NewtonBody;

class NAZARA_PHYSICS_API NzPhysObject
{
	public:
		NzPhysObject(NzPhysWorld* world, const NzMatrix4f& mat = NzMatrix4f::Identity());
		NzPhysObject(NzPhysWorld* world, NzPhysGeomRef geom, const NzMatrix4f& mat = NzMatrix4f::Identity());
		NzPhysObject(const NzPhysObject& object);
		NzPhysObject(NzPhysObject&& object);
		~NzPhysObject();

		void AddForce(const NzVector3f& force, nzCoordSys coordSys = nzCoordSys_Global);
		void AddForce(const NzVector3f& force, const NzVector3f& point, nzCoordSys coordSys = nzCoordSys_Global);
		void AddTorque(const NzVector3f& torque, nzCoordSys coordSys = nzCoordSys_Global);

		void EnableAutoSleep(bool autoSleep);

		NzBoxf GetAABB() const;
		NzVector3f GetAngularVelocity() const;
		const NzPhysGeomRef& GetGeom() const;
		float GetGravityFactor() const;
		NewtonBody* GetHandle() const;
		float GetMass() const;
		NzVector3f GetMassCenter(nzCoordSys coordSys = nzCoordSys_Local) const;
		const NzMatrix4f& GetMatrix() const;
		NzVector3f GetPosition() const;
		NzQuaternionf GetRotation() const;
		NzVector3f GetVelocity() const;

		bool IsAutoSleepEnabled() const;
		bool IsMoveable() const;
		bool IsSleeping() const;

		void SetAngularVelocity(const NzVector3f& angularVelocity);
		void SetGeom(NzPhysGeomRef geom);
		void SetGravityFactor(float gravityFactor);
		void SetMass(float mass);
		void SetMassCenter(const NzVector3f& center);
		void SetPosition(const NzVector3f& position);
		void SetRotation(const NzQuaternionf& rotation);
		void SetVelocity(const NzVector3f& velocity);

		NzPhysObject& operator=(const NzPhysObject& object);
		NzPhysObject& operator=(NzPhysObject&& object);

	private:
		void UpdateBody();
		static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);
		static void TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex);

		NzMatrix4f m_matrix;
		NzPhysGeomRef m_geom;
		NzVector3f m_forceAccumulator;
		NzVector3f m_torqueAccumulator;
		NewtonBody* m_body;
		NzPhysWorld* m_world;
		float m_gravityFactor;
		float m_mass;
};

#endif // NAZARA_PHYSOBJECT_HPP
