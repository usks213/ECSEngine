/*****************************************************************//**
 * \file   EditorManager.h
 * \brief  エディタマネージャー
 * 
 * \author USAMI KOSHI
 * \date   2021/07/30
 *********************************************************************/
#pragma once

#include <Engine/Object.h>
#include <Engine/ECS/Base/World.h>
#include <Engine/ECS/ComponentData/TransformComponentData.h>
#include <Engine/ECS/ComponentData/CameraComponentData.h>


class Engine;

class EditorManager final
{
	friend class Engine;
public:

	EditorManager(Engine* pEngine) :
		m_pEngine(pEngine), m_isRunTime(false)
	{
	}

	~EditorManager() = default;
	EditorManager(const EditorManager&) = delete;
	EditorManager(EditorManager&&) = delete;

public:

	void initialize();
	void finalize();
	void update();

private:
	void dispHierarchy();
	void dispAsset();
	void dispInspector();
	void dispWorld();

	void DispChilds(const InstanceID parentID);

	void updateTransform();
	void updateView();

	void componentInspector(std::size_t typeHash, void* data);
	void EditTransform(ecs::World* pWorld, ecs::Camera& camera, ecs::Transform& transform);

	void CreateFBX();
private:
	Engine* m_pEngine;
	bool m_isRunTime;

	struct ObjectInfo
	{
		TypeID typeID = std::numeric_limits<TypeID>::max();
		InstanceID instanceID = std::numeric_limits<InstanceID>::max();
	};
	ObjectInfo m_selectObjectInfo;

	struct EditorCamera
	{
		ecs::Camera camera;
		ecs::Transform transform;
		EditorCamera() :
			camera(), transform(0)
		{}
	};
	EditorCamera m_editorCamera;
	POINT m_oldMousePos;
};
