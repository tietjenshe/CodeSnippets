#pragma once
#include "engine/Entity.h"

#include "engine/components/Transform.h"
#include "engine/components/Renderer.h"
#include "engine/components/Rigidbody.h"
#include "engine/components/Audio.h"

namespace spacedust {
	/// @brief The turret entity that tracks and shoots the player ship once it enters its range
	class Turret : public engine::Entity {
	public:
		Turret(engine::guid_t entityID, engine::EntityComponentManager& entityManager,
			engine::Transform& shipTransform,
			engine::Transform* parent = nullptr,
			glm::vec3 position = { 0, 0, 0 },
			glm::quat rotation = glm::quat(1, 0, 0, 0),
			glm::vec3 scale = { 1, 1, 1 });

	private:
		void update();
		void rotateTowardsTarget();
		void shoot();
		void destroyTurret();
		void onCollisionEntered(engine::Physicsbody* otherBody);

		// components
		engine::Transform& m_transform;
		engine::Renderer& m_renderer;
		engine::Rigidbody& m_rigidbody;
		engine::Audio& m_firingAudio;

		// stats
		int m_health = 5;

		// targeting
		engine::Transform& m_target;
		float m_range = 100.0f;
		float m_turningRate = 90.0f;

		// shooting
		glm::vec4 m_bulletColor = glm::vec4(255 / 255, 90.0f / 255, 90.0f / 255, 1);
		float m_firingSpeed = 35.0f;
		float m_firingRate = 2.0f;
		float m_firingCooldown = 0;
	};
}